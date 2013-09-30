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

Well played guys.  Well played. After all, `cat`s like to play with `strings`... `;)`

After the CTF, I heard that IDA could've shown you this as well.  *Note to self: install IDA next time.*

### Hash ###

This question was very interesting. We were given some source code which was a broken hashing function. We were told that the password we needed to extract was 8 characters long and random.

Basically, this is what the hashing function did:

1. Cut the input in to 16-character (aka byte) blocks, zero-padded.
2. Each set of 4 characters was converted into a 'word' using some strange `union` hacks.
3. The 4 words were expanded to 8 words.
4. The 8 words were then compressed into 4 words using xors.
5. It was then digested into hex.

The real vulnerability in this function was points 3 and 4. After many pages and several pens, @cyphar managed to find exactly what was happening in the hashing function:

Example 8-character password block:
```
+-------------------------------+
|a|b|c|d|e|f|g|h| | | | | | | | |
+-+-+-+-------------------------+
```

Broken into words:
```
+----+----+----+----+
|abcd|efgh|    |    |
+----+----+----+----+

Let x = "abcd" as a word
Let y = "efgh" as a word
```

The algorithm rotates numbers several times. This is where the lowest-significant bit becomes the most significant, and the rest is shifted down.
```
x'  = single rotated number
x'' = double rotated number
```

The algorithm then expands the array into an 8-block array. It does this by rotating and xoring the original values, like so:
```
+-+-+-+-+-----+-----+-----------+-----------+
|a|b|c|d|a'^c'|b'^d'|c'^(a'^c')'|d'^(b'^d')'|
+-+-+-+-+-----+-----+-----------+-----------+
```

The problem with this system is that c and d are full of 0s if the input was <= 8 chars (like the question states). The blocks then look like this:

```
+-+-+-+-+--+--+---+---+
|a|b| | |a'|b'|a''|b''|
+-+-+-+-+--+--+---+---+

Whoops.
```

The hashing then compresses the above 8-word block into a 4-word block. This is the basis of most hashing algorithms, but there is a flaw which was compounded by the above flaw.
The actual compressing was essentially just an xor of the above values. The second and fourth words of the current hash block are xored with the current word being inserted.

Hashing, in steps:
```
  +--------+--------+-----+-----+
1 |   x    |        |     |     |
2 |   y    |   x    |     |     |
3 |   x'   |   y    |  x  |     |
4 |   y'   |   x'   |  y  |  x  |
5 |   x'   |   y'   |  x' |  y  |
6 |   y'   |   x'   |  y' |  x' |
7 |x^x'^x''|   y'   |  x' |  y' |
8 |y^y'^y''|x^x'^x''|  y' |  x' |
  +--------+--------+-----+-----+
```

The compression is broken in steps 5 and 6. In steps 5 and 6, two of the values being xored are identical, producing the third value. All we now needed was to create a program to reverse rotate the last two octets of the hash, and we were done.
@cyphar's final solution is in `unhash.c`.
