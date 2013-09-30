Reverse Engineering / Crypto
----------------------------

Our team only managed to solve rope and hash, both solved by @cyphar.  Great work man.

### Rope ###

For this challenge, we were given an ELF binary.  There was a hint given, something along the lines of twine, thread, and something (something like that).

Obviously, I took that to be `strings`.  Naturally, I did `strings rope`:

```
$ strings ./rope
/lib/ld-linux.so.2
__gmon_start__
libc.so.6
_IO_stdin_used
wprintf
__libc_start_main
GLIBC_2.0
GLIBC_2.2
PTRh
UWVS
[^_]
;*2$"
```

Wasn't very useful.

*A couple of hours passed...*

@cyphar then did `cat rope`.  Turns out, the key is not displayed by strings, but only by `cat`.

```
$ cat ./rope
ELF@4<4         (44  TTT
[insert binary garbage here]
EBUG MODE: The key is: 1Ks<redacted>69s     [insert more garbage here]
```

Well played guys.  Well played.

After the CTF, I heard that IDA could've shown you this as well.  *Note to self: install IDA next time.*

### hash

TODO: ask @cyphar about this.  This was solved during my nap.  :(
