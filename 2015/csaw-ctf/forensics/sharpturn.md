## sharpturn
Writeup by @auscompgeek.

### Introduction
> I think my SATA controller is dying.
>
> HINT: `git fsck -v`

The given hint was added later during the CTF.

First, we realise that the given tarball is a bare Git repo.  This happens to be
extremely easy if your shell prompt displays Git repo information.  Otherwise,
one could recognise the Git repo structure.

It turns out this repo is actually corrupted.

NB: a bare repo can be cloned to construct a non-bare repo with a working tree.

### Tools needed
* `git` <sup>1</sup>
* a text processing tool

### Tips and background
A couple of useful `git` subcommands:
* `cat-file`
* `hash-object`

Knowledge of Git internals is also useful, but not necessary.

### Recovery

Let's follow the hint.
```
$ git fsck -v
Checking HEAD link
Checking object directory
Checking directory ./objects/2b
Checking directory ./objects/2e
Checking directory ./objects/35
Checking directory ./objects/4a
Checking directory ./objects/4c
Checking directory ./objects/7c
Checking directory ./objects/a1
Checking directory ./objects/cb
Checking directory ./objects/d5
Checking directory ./objects/d9
Checking directory ./objects/e5
Checking directory ./objects/ef
Checking directory ./objects/f8
error: sha1 mismatch 354ebf392533dce06174f9c8c093036c138935f3
error: 354ebf392533dce06174f9c8c093036c138935f3: object corrupt or missing
Checking commit 4a2f335e042db12cc32a684827c5c8f7c97fe60b
Checking tree 4c0555b27c05dbdf044598a0601e5c8e28319f67
Checking tree 2bd4c81f7261a60ecded9bae3027a46b9746fa4f
Checking commit 2e5d553f41522fc9036bacce1398c87c2483c2d5
Checking commit 7c9ba8a38ffe5ce6912c69e7171befc64da12d4c
Checking tree a1607d81984206648265fbd23a4af5e13b289f83
Checking tree cb6c9498d7f33305f32522f862bce592ca4becd5
Checking commit d57aaf773b1a8c8e79b6e515d3f92fc5cb332860
error: sha1 mismatch d961f81a588fcfd5e57bbea7e17ddae8a5e61333
error: d961f81a588fcfd5e57bbea7e17ddae8a5e61333: object corrupt or missing
Checking blob e5e5f63b462ec6012bc69dfa076fa7d92510f22f
Checking blob efda2f556de36b9e9e1d62417c5f282d8961e2f8
error: sha1 mismatch f8d0839dd728cb9a723e32058dcc386070d5e3b5
error: f8d0839dd728cb9a723e32058dcc386070d5e3b5: object corrupt or missing
Checking connectivity (32 objects)
Checking a1607d81984206648265fbd23a4af5e13b289f83
Checking e5e5f63b462ec6012bc69dfa076fa7d92510f22f
Checking 4a2f335e042db12cc32a684827c5c8f7c97fe60b
Checking cb6c9498d7f33305f32522f862bce592ca4becd5
Checking 4c0555b27c05dbdf044598a0601e5c8e28319f67
Checking 2bd4c81f7261a60ecded9bae3027a46b9746fa4f
Checking 2e5d553f41522fc9036bacce1398c87c2483c2d5
Checking efda2f556de36b9e9e1d62417c5f282d8961e2f8
Checking 354ebf392533dce06174f9c8c093036c138935f3
missing blob 354ebf392533dce06174f9c8c093036c138935f3
Checking d57aaf773b1a8c8e79b6e515d3f92fc5cb332860
Checking f8d0839dd728cb9a723e32058dcc386070d5e3b5
missing blob f8d0839dd728cb9a723e32058dcc386070d5e3b5
Checking d961f81a588fcfd5e57bbea7e17ddae8a5e61333
missing blob d961f81a588fcfd5e57bbea7e17ddae8a5e61333
Checking 7c9ba8a38ffe5ce6912c69e7171befc64da12d4c
```

A couple of the blobs seem to be corrupted.  Let's poke through the repo a bit.
```
$ git log --oneline
4a2f335e0 All done now! Should calculate the flag..assuming everything went okay.
d57aaf773 There's only two factors. Don't let your calculator lie.
2e5d553f4 It's getting better!
7c9ba8a38 Initial commit! This one should be fun.
$ git ls-tree HEAD^^
100644 blob 354ebf392533dce06174f9c8c093036c138935f3	sharp.cpp
$ git cat-file blob 354ebf39
```
```cpp
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main(int argc, char **argv)
{
	(void)argc; (void)argv; //unused

	std::string part1;
	cout << "Part1: Enter flag:" << endl;
	cin >> part1;

	int64_t part2;
	cout << "Part2: Input 51337:" << endl;
	cin >> part2;

	std::string part3;
	cout << "Part3: Watch this: https://www.youtube.com/watch?v=PBwAxmrE194" << endl;
	cin >> part3;

	std::string part4;
	cout << "Part4: C.R.E.A.M. Get da _____: " << endl;
	cin >> part4;

	return 0;
}

```

Part 2 seems to be a bit wrong.  Let's fix that to be 31337:
```
$ git cat-file blob 354ebf39 | sed s/51337/31337/ | git hash-object --stdin
354ebf392533dce06174f9c8c093036c138935f3
```

Success!  Let's keep going.

```
$ git ls-tree HEAD^
100644 blob d961f81a588fcfd5e57bbea7e17ddae8a5e61333	sharp.cpp
$ git diff HEAD^^ HEAD^
diff --git a/sharp.cpp b/sharp.cpp
index 354ebf392..d961f81a5 100644
--- a/sharp.cpp
+++ b/sharp.cpp
@@ -24,6 +24,23 @@ int main(int argc, char **argv)
 	cout << "Part4: C.R.E.A.M. Get da _____: " << endl;
 	cin >> part4;

+	uint64_t first, second;
+	cout << "Part5: Input the two prime factors of the number 270031727027." << endl;
+	cin >> first;
+	cin >> second;
+
+	uint64_t factor1, factor2;
+	if (first < second)
+	{
+		factor1 = first;
+		factor2 = second;
+	}
+	else
+	{
+		factor1 = second;
+		factor2 = first;
+	}
+
 	return 0;
 }

```

Well, 270031727027 is definitely not semi-prime.  I'll take a bet that one of
the digits got corrupt.  I'll fix this in Python this time.

```python
#!/usr/bin/env python3

import hashlib

# The decompressed blob, with part 2 corrected.
FILE = b'''blob 806\x00#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main(int argc, char **argv)
{
	(void)argc; (void)argv; //unused

	std::string part1;
	cout << "Part1: Enter flag:" << endl;
	cin >> part1;

	int64_t part2;
	cout << "Part2: Input 31337:" << endl;
	cin >> part2;

	std::string part3;
	cout << "Part3: Watch this: https://www.youtube.com/watch?v=PBwAxmrE194" << endl;
	cin >> part3;

	std::string part4;
	cout << "Part4: C.R.E.A.M. Get da _____: " << endl;
	cin >> part4;

	uint64_t first, second;
	cout << "Part5: Input the two prime factors of the number 270031727027." << endl;
	cin >> first;
	cin >> second;

	uint64_t factor1, factor2;
	if (first < second)
	{
		factor1 = first;
		factor2 = second;
	}
	else
	{
		factor1 = second;
		factor2 = first;
	}

	return 0;
}

'''
DIGITS = b'0123456789'
CORRECT_HASH = 'd961f81a588fcfd5e57bbea7e17ddae8a5e61333'
WRONG_NUMBER = b'270031727027'
START_IDX = FILE.index(WRONG_NUMBER)


def main():
	for i, d in enumerate(WRONG_NUMBER):
		for r in DIGITS:
			if d == r:
				continue
			trial = bytearray(FILE)
			trial[START_IDX + i] = r
			if hashlib.sha1(trial).hexdigest() == CORRECT_HASH:
				print(trial[9:-1].decode())
				return


if __name__ == '__main__':
	main()
```

We quickly find that the correct number is 272031727027.

The final blob also has another corrupt byte.  For brevity, I won't include a
full dissection here, but `&lag` should be `flag`.

Patching the final blob, this should give us the correct flag!

### Notes ###
Turns out `git fsck` is extremely useful for checking that a Git repo is valid.

<sup>1</sup> Whilst I say `git` is required, it is possible to do this challenge
without `git`, but it would be extremely painful. :stuck_out_tongue_winking_eye:
