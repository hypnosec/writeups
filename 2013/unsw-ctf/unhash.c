/* unhash.c: a solution to the hash problem
 * Copyright (c) 2013 Cyphar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* ^^^^^^^^^^^^^^^^^^^^^^ */
/* just legal boilerplate */

#include <stdio.h>
#include <ctype.h>

/* typedefs */
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* a truly messed up way to deal
 * with things in different sized
 * chunks */
typedef union {
	uint8_t bytes[16];
	uint32_t words[4];
} hash_t;

/* reversed rotation */
uint32_t brot(uint32_t x, int y) {
	return x << y | x >> (32 - y);
}

/* converts since hex char into int */
uint32_t from_hex(char a) {
	if('0' <= a && a <= '9')
		return a - '0';
	else
		return tolower(a) - 'a' + 10;
}

/* converts 0xff to int */
uint32_t tonibble(char a, char b) {
	return from_hex(a) << 4 | from_hex(b);
}

/* main 1337 h4ck3rz function */
int main(void) {
	char inp[32]; /* don't scream at me. */
	hash_t original;

	/* get 32 char hash */
	int i;
	for(i = 0; i < 32; i++)
		inp[i] = getchar();

	/* flush input buffer */
	while(getchar() != '\n');

	/* initialise bytes to 0 */
	for(i = 0; i < 16; i++)
		original.bytes[i] = 0;

	/* copy over the chars in nibbles */
	for(i = 0; i < 32; i+=2)
		original.bytes[i/2] = tonibble(inp[i], inp[i+1]);

	/* The "exploit" in the algorithm: an input of
	 * <= 8 chars will not be correctly "compressed"
	 * and the last two octets will be the rotated
	 * original input. Too easy. */
	hash_t crack;
	crack.words[0] = brot(original.words[3], 1);
	crack.words[1] = brot(original.words[2], 1);

	/* print out the characters */
	for(i = 0; i < 8; i++)
		printf("%c", crack.bytes[i]);

	/* newlines are pretty */
	printf("\n");
	return 0;
}
