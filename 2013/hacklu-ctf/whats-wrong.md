# What's wrong with this?

> We managed to get this package of the robots servers. We managed to determine that it is some kind of compiled bytecode. But something is wrong with it. Our usual analysis failed - so we have to hand this over to you pros. We only know this: The program takes one parameter and it responds with "Yup" if you have found the secret code, with "Nope" else. We expect it should be obvious how to execute it.
>
> Here is the challenge: https://ctf.fluxfingers.net/static/downloads/whats_wrong/hello.tar.gz

After extracting the tarball, we see a Python 2.7 interpreter at `./py` with a symlink `./hello`.

In the library, there's a `__main__hello__.pyc`.
Trying to run this with the real CPython 2.7 errors.

Attempting to decompile it in the normal fashion results in something very weird.
There definitely is something wrong with this.

We still have the interpreter at `./py` however.
Let's install `uncompyle2` in here and chuck it the `__main__hello__.pyc`

... and it decompiles successfully! Yay!

```python
# 2013.10.23 15:39:09 EST
#Embedded file name: chall.py
import sys
from hashlib import sha256
import dis, multiprocessing, UserList

def encrypt_string(s):
    new_str = []
    for index, c in enumerate(s):
        if index == 0:
            new_str.append(rot_chr(c, 10))
        else:
            new_str.append(rot_chr(c, ord(new_str[index - 1])))

    return ''.join(new_str)


def rot_chr(c, amount):
    return chr((ord(c) - 33 + amount) % 94 + 33)


SECRET = 'w*0;CNU[\\gwPWk}3:PWk"#&:ABu/:Hi,M'
if encrypt_string(sys.argv[1]) == SECRET:
    print 'Yup'
else:
    print 'Nope'
```

Little devils trying to trick us with all those imports!

This looks simple enough to decrypt. Let's reverse the algorithm.

Final solution:

```python
def rot_chr(c, amount):
    return chr((ord(c) - 33 + amount) % 94 + 33)

SECRET = 'w*0;CNU[\\gwPWk}3:PWk"#&:ABu/:Hi,M'

def decrypt_string(s):
    new_str = []
    for index, c in enumerate(s):
        if index == 0:
            new_str.append(rot_chr(c, -10))
        else:
            new_str.append(rot_chr(c, -ord(s[index - 1])))

    return ''.join(new_str)

print(decrypt_string(SECRET))
```

-- @auscompgeek
