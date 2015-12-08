#!/usr/bin/env pypy3

import sys
from functools import lru_cache

sys.setrecursionlimit(10000)

def cache(f):
	return lru_cache(maxsize=None)(f)

def is_prime(n):
	if n & 1 is 0 and n is not 2:
		return False
	for i in range(3, int(n**.5) + 1, 2):
		if n % i is 0:
			return False
	return True

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

def epicfail(memes):
	if memes <= 1:
		return 0
	#if is_prime(memes):
	if memes in primes:
		return bill(memes - 1) + 1
	return such(memes - 1)

def binom5(n):
	if n < 5:
		return 0
	if n is 5:
		return 1
	return n*(n-1)*(n-2)*(n-3)*(n-4) // 120

@cache
def such(memes):
	wow = binom5(memes)
	if wow % 7 is 0:
		wew = bill(memes - 1) + 1
	else:
		wew = epicfail(memes - 1)
	return wew + wow

def fib_magic(n):
	l = len(fibs)
	if l > n:
		return  # fibs[n]

	a, b = fibs[-2], fibs[-1]
	for _ in range(l - 1, n):
		a, b = b, (a+b) % 987654321
		fibs.append(b)
	#return a

fibs_mod3 = [0, 1, 1, 2, 0, 2, 2, 1]
fibs = [0, 1]
fib_magic(13379446)

def bill(memes):
	if fibs_mod3[memes % 8] is 0:
		wew = such(memes - 1) + 1
	else:
		wew = epicfail(memes - 1)
	return wew + fibs[memes]

if __name__ == '__main__':
	for i in range(3, 13379447, 2000):
		epicfail(i)
		print(i)

	print(epicfail(13379447))
