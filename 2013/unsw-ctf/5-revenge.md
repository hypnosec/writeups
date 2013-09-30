The revenge
-----------

We had to exploit the hell out of a webapp, basically.  Needless to say, with @infosec-au on our team, we succeeded.

### Hidden message ###

Every page had the same hidden message.  This was shown when a certain key combination was pressed, the event listener created through jQuery in the horrid `magic.js` file included by every page:

```js
$(function () {
    $(document).keydown(function (e) {
        if (e.ctrlKey && e.which == 77) {
            $('#secret').show(1500);
            $('#secret').unbreakablecrypto();
        };
    });
});
```

So, `ctrl-m` or just read the source of the page (which contained the key).


### Log in ###

Yup, just log on in.  Well, it wasn't that simple.  Again, from `magic.js`:

```js
$(function () {
    $("[class^=enable_user]").click(function (event) {
        $.get("mod_tasks.php", {
            action: 'enable_user',
            user_id: $(this).attr('value')
        });
    });
});
```

As it turns out, anyone can run that script. So you make a user, it tells you the ID of the user and just go to `/mod_tasks.php?action=enable_user&id=<id>`. Bam! Challenge completed.

### Like a thief in the night

Steal the `passhash` of the moderator with user ID 1.

This required an SQL injection.  Poking around whilst logged in, there was SQLi on the `sex` parameter in `control.php`. In the words of @cyphar: "we used and abused sex `;)`".

After the CTF was over, people discussed having a user's `class > 100`, which apparently allowed the user/moderator to get the `passhash` of any user.

### Become a moderator ###

This involved HTTP injection and CSRF in the messages previews.  It can be noted from `magic.js` that users are made administrators using a GET request:

```js
$(function () {
    $("[class^=make_administrator]").click(function (event) {
        $.get("mod_tasks.php", {
            action: 'make_administrator',
            user_id: $(this).attr('value')
        });
    });
});
```

The admin would periodically check his email, and we could inject custom HTML, which was *not* escaped in the message preview. A few `<meta>`, `<img>` and `<iframe>` tags later, we had moderator access.

### D-d-d-d-do you have full access?

I assume this was in the admin panel (I couldn't see this as I wasn't there in person).
