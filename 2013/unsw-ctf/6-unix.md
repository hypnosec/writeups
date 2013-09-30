Unixy Stuff / Misc
------------------

Snoop through the Ubuntu box running in Amazon EC2 and find the flags.  The box was highly locked down with no write access to any directory.

### Banned ###

Hilariously, I (we) got access through `su` on the second user given for banned2.  `su ctf -c sh`, anyone?

After the CTF, we found out that it was a simple `^C`. Apparently, `.profile` can be skipped that easily.  :facepalm:

### Banned 2 ##

We didn't actually solve this, but we were pretty close.

You needed to look at `/root/backup_encrypted.sh`.  You would find that:

* a large portion of the script is run in `sudo` regularly; and
* a backup is stored at `/var/log/bak`, which was world readable.

Once the encryption password is obtained from the `sudo` command line in the process listing, you just had to mount the backup using that password on your local system.
