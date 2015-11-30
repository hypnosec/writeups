## danklang
Writeup by @auscompgeek.

### tl;dr
* Translate main.dc into a sane and measured programming language.
* Optimise some functions by working out what they actually do.
* Avoid hitting the stack limit, somehow.

### Introduction
> if you see this task while scroling
>
> you have been visited by the reversing task of the 9447 ctf<br/>
> good flags and points will come to you
>
> but only if you submit '9447{\`dankcode main.dc`}' to this task.

* [main.dc][] - md5 5c146bdc240527609ffcf236d04be83b

This challenge involves a subset of a (joke?) language called [greentext][].

**NB**: You should have a good understanding of maths, programming, and Python
before reading this.

[main.dc]: https://github.com/ctfs/write-ups-2015/blob/master/9447-ctf-2015/reversing/danklang/main-5c146bdc240527609ffcf236d04be83b.dc
[greentext]: https://github.com/jfeng41/greentext

### Translation

The constructs used in main.dc are fairly basic. Most operators are the
same as any C-based language's operators with the exception of `is` and `isn't`.

For brevity, here is a sed script that mostly converts main.dc to Python:
```sed
#!/bin/sed -f
s/>wewlad \(.*\)/def \1:/
s/>wew /wew = /g
s/>tfw/return/
s/>be \(\S*\) like /\1 = /g
s/>implying \(.*\)/if \1:/
s/>or not/else:/
s/\s*>done implying$//
s/:\^)/True/g
s/:\^(/False/g
s/isn't/!=/g
s/is/==/g
s/>mfw /print /g
s/>be me$/if __name__ == '__main__':/
s/\s*>thank mr skeltal$//
```

This results in an empty `if` block, which needs to be fixed.
Here is a complete, search-and-replace translation to Python:

```python
def fail(memes, calcium):
	dank = True
	if calcium < memes:
		if memes % calcium == 0:
			dank = False
		else:
			wew = fail(memes, calcium + 1)
			dank = wew
	return dank

def epicfail(memes):
	wow = 0
	dank = True
	if memes > 1 :
		wew = fail(memes, 2)
		dank = wew
		if dank:
			wew = bill(memes - 1)
			wow = wew + 1
		else:
			wew = such(memes - 1)
			wow = wew
	return wow

def dootdoot(memes, seals):
	doritos = 0
	if seals > memes:
		pass
	else:
		if seals == 0:
			doritos = 1
		else:
			if seals == memes:
				doritos = 1
			else:
				wew = dootdoot(memes - 1, seals - 1)
				doritos = wew
				wew = dootdoot(memes - 1, seals)
				doritos = wew + doritos
	return doritos

def such(memes):
	wew = dootdoot(memes, 5)
	wow = wew
	if wow % 7 == 0:
		wew = bill(memes - 1)
		wow = wow + 1
	else:
		wew = epicfail(memes - 1)
	wow = wew + wow
	return wow

def brotherman(memes):
	hues = 0
	if memes != 0:
		if memes < 3:
			hues = 1
		else:
			wew = brotherman(memes - 1)
			hues = wew
			wew = brotherman(memes - 2)
			hues = wew + hues
	hues = hues % 987654321
	return hues

def bill(memes):
	wew = brotherman(memes)
	wow = wew
	if wow % 3 == 0:
		wew = such(memes - 1)
		wow = wow + 1
	else:
		wew = epicfail(memes - 1)
	wow = wew + wow
	return wow

if __name__ == '__main__':
	memes = 13379447
	wew = epicfail(memes)
	print(wew)
```

### Optimisation

If you tried to execute this blindly, you would end up hitting the stack limit.
Even with infinite memory, this would still take longer than the CTF to run.

The main thing we need to do is work out what some of these functions are doing.
Not how they do it, but what they do. Let's start by removing some recursion.

#### `fail(memes, calcium)`

```python
def fail(memes, calcium):
	while calcium < memes:
		if memes % calcium == 0:
			return False
		calcium += 1
	return True
```

It can be observed that, if `fail()` is called with `calcium=2`, this is a basic
primality test using trial division. We can hence swap calls to `fail(memes, 2)`
with a faster primality test. There are two ways of doing this:

* Precompute a set of all primes up to 13379447.
* Write a more optimised trial division primality test.

Since we will need to test several primes during execution, it may be best to
generate a set of primes using a [prime sieve][]. A basic sieve we can use is
the [sieve of Eratosthenes][eratosthenes].

[prime sieve]: https://en.wikipedia.org/wiki/Prime_sieve
[eratosthenes]: https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes

Here is an implementation of the sieve of Eratosthenes in Python:
```python
def eratosthenes(n):
	n1 = n + 1
	s = set(range(3, n1, 2))
	for p in range(3, int(n**.5) + 1, 2):
		if p not in s:
			continue
		for j in range(p*p, n1, p):
			if j in s:
				s.remove(j)
	s.add(2)
	return s

primes = eratosthenes(13379447)

# replace `fail(memes, 2)` with `memes in primes` in epicfail()
```

#### `dootdoot(memes, seals)`

```python
def dootdoot(memes, seals):
	doritos = 0
	if seals <= memes:
		if seals == 0 or seals == memes:
			doritos = 1
		else:
			doritos = dootdoot(memes - 1, seals - 1) + dootdoot(memes - 1, seals)
	return doritos
```

This may be fairly difficult to see, but this computes an element in
Pascal's triangle. You may know from high school that Pascal's triangle is also
a list of binomial coefficients (or "combinations"), which can be computed as:

![nCr=n!/(n-r)!r!](https://cloud.githubusercontent.com/assets/128854/11462584/8184895a-976a-11e5-9c2f-8d163a0f05e9.png)

We can hence rewrite this as:
```python
import math

def binom(n, r):
	if n < r:
		return 0
	if r in (0, n):
		return 1
	return reduce(int.__mul__, range(n, n - r, -1)) // math.factorial(r)
```

But why stop here? `dootdoot()` is only ever called outside of itself with
`seals=5`. We don't need `reduce()` and `factorial()`, we can do that ourselves!

```python
def binom5(n):
	if n < 5:
		return 0
	if n == 5:
		return 1
	return n*(n-1)*(n-2)*(n-3)*(n-4) // 120

def such(memes):
	wow = binom5(memes)
	if wow % 7 == 0:
		...
	...
```

#### `brotherman(memes)`

```python
def brotherman(memes):
	if memes == 0:
		return 0
	if memes <= 2:
		return 1
	hues = brotherman(memes - 2) + brotherman(memes - 1)
	return hues % 987654321
```

This is one of the more classic examples of recursion. This should hopefully
look familiar to you as calculating the *n*th Fibonacci number. Almost.

This function actually calculates the *n*th Fibonacci number, modulo 987654321.

There are three things we could do this time:

* Precompute the Fibonacci sequence (mod 987654321) up to 13379446.
  (The largest number that `brotherman()` will get is 13379446.)
* Memoize `brotherman()`. This will effectively be the same as precomputing,
  except it'll be computed on demand. (This is dynamic programming.)
* Rewrite `brotherman()` without recursion. There are many iterative
  implementations of `fib(n)`, modifying them to calculate modulo 987654321 is
  left as an exercise to the reader.

Let's precompute this baby.

```python
def fib_magic(n):
	a, b = fibs[-2], fibs[-1]
	for _ in range(len(fibs) - 1, n):
		a, b = b, (a+b) % 987654321
		fibs.append(b)

fibs = [0, 1]
fib_magic(13379446)

# replace `brotherman(memes)` with `fibs[memes]` in bill()
```

##### `bill(memes)`

But why stop there? (We could, but hey.)

`bill()` checks `brotherman(n) % 3`. As it turns out, 987654321 is divisible by 3.
What happens when we take the Fibonacci sequence, modulo 3?

As it turns out, the Fibonacci sequence (mod 3) repeats, and it's not too
difficult to see why: 0, 1, 1, 2, 0, 2, 2, 1, 0, 1, 1, 2, 0, 2, 2, 1, ...

Instead of calculating a large number, then taking that modulo 3, we could check
our small list of Fibonacci numbers (mod 3):

```python
fibs_mod3 = [0, 1, 1, 2, 0, 2, 2, 1]

def bill(memes):
	if fibs_mod3[memes % 8] == 0:
		...
```

#### Halting the recursion

There are a couple of ways of removing the recursion in
`epicfail()`, `such()`, and `bill()`:

1. Build a state machine in `epicfail()`.
2. Memoize any of the three functions, and populate the cache.

I was rather lazy, so I went with 2. Building a state machine is left as an
exercise to the reader. :stuck_out_tongue_winking_eye:

Python 3 has a handy `lru_cache()` decorator in `functools`.

```python
from functools import lru_cache

@lru_cache(maxsize=None)
def such(memes):
	...
```

### Solution
My full solution can be found in [main.py](main.py).

The flag is `9447{2992959519895850201020616334426464120987}`.
