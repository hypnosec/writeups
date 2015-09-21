## Transfer
Writeup by @auscompgeek.

> I was sniffing some web traffic for a while, I think i finally got something interesting. Help me find flag through all these packets.
>
> net_756d631588cb0a400cc16d1848a5f0fb.pcap

Let's cut to the chase.

The provided pcap contains a TCP connection from 192.168.15.133 to 192.168.15.135:80.
There are no packets from 192.168.15.135, besides ACKs, in this connection.

The first PSH packet in the connection contains some Python:
```python
import string
import random
from base64 import b64encode, b64decode

FLAG = 'flag{xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx}'

enc_ciphers = ['rot13', 'b64e', 'caesar']
# dec_ciphers = ['rot13', 'b64d', 'caesard']

def rot13(s):
	_rot13 = string.maketrans(
    	"ABCDEFGHIJKLMabcdefghijklmNOPQRSTUVWXYZnopqrstuvwxyz",
    	"NOPQRSTUVWXYZnopqrstuvwxyzABCDEFGHIJKLMabcdefghijklm")
	return string.translate(s, _rot13)

def b64e(s):
	return b64encode(s)

def caesar(plaintext, shift=3):
    alphabet = string.ascii_lowercase
    shifted_alphabet = alphabet[shift:] + alphabet[:shift]
    table = string.maketrans(alphabet, shifted_alphabet)
    return plaintext.translate(table)

def encode(pt, cnt=50):
	tmp = '2{}'.format(b64encode(pt))
	for cnt in xrange(cnt):
		c = random.choice(enc_ciphers)
		i = enc_ciphers.index(c) + 1
		_tmp = globals()[c](tmp)
		tmp = '{}{}'.format(i, _tmp)

	return tmp

if __name__ == '__main__':
	print encode(FLAG, cnt=?)
```

The rest of the TCP stream contains the ciphertext, which begins with
`2Mk16Sk5iakYxVFZoS1RsWnZXbFZaYjFaa1prWmFkMDVWVGs1U2IyODFXa1ZuTUZadU1Y`.

The provided Python code first prepends '2' to the plaintext, base64 encodes it,
then goes into a loop.  For each iteration of the loop:

1. pick a random cipher from rot13, base64, and a Caesar shift of 3;
2. perform the cipher on the current `pt`; then
3. prepend the cipher index (1 for rot13, 2 for base64, 3 for Caesar) to `pt`.

All three ciphers are trivially reversible, and which cipher is used for each
iteration is given by the index, so reversing the algorithm is fairly trivial.

### Solution
```python
#!/usr/bin/env python2

import string
import random
from base64 import b64encode, b64decode

FLAG = open('net-encoded.txt').read()

enc_ciphers = ['rot13', 'b64e', 'caesar']
dec_ciphers = ['rot13', 'b64d', 'caesard']

def rot13(s):
	_rot13 = string.maketrans(
		"ABCDEFGHIJKLMabcdefghijklmNOPQRSTUVWXYZnopqrstuvwxyz",
		"NOPQRSTUVWXYZnopqrstuvwxyzABCDEFGHIJKLMabcdefghijklm")
	return string.translate(s, _rot13)

def b64e(s):
	return b64encode(s)

def b64d(s):
	return b64decode(s)

def caesar(plaintext, shift=3):
	alphabet = string.ascii_lowercase
	shifted_alphabet = alphabet[shift:] + alphabet[:shift]
	table = string.maketrans(alphabet, shifted_alphabet)
	return plaintext.translate(table)

def caesard(ciphertext, shift=3):
	return caesar(ciphertext, shift=-shift)

def encode(pt, cnt=50):
	tmp = '2{}'.format(b64encode(pt))
	for cnt in xrange(cnt):
		c = random.choice(enc_ciphers)
		i = enc_ciphers.index(c) + 1
		_tmp = globals()[c](tmp)
		tmp = '{}{}'.format(i, _tmp)

	return tmp

def decode(ct):
	while ct[0].isdigit():
		i = int(ct[0]) - 1
		c = dec_ciphers[i]
		ct = globals()[c](ct[1:])
	return ct

if __name__ == '__main__':
	print decode(FLAG)
```

Our final flag is `flag{li0ns_and_tig3rs_4nd_b34rs_0h_mi}`.
