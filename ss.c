#include "ss.h"
#include "numtheory.h"
#include <stdlib.h>

//
// Generates the components for a new SS key.
//
// Provides:
//  p:  first prime
//  q: second prime
//  n: public modulus/exponent
//
// Requires:
//  nbits: minimum # of bits in n
//  iters: iterations of Miller-Rabin to use for primality check
//  all mpz_t arguments to be initialized
//
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
	// [nbits/5, (2 * nbits)/5)
	uint64_t lower = nbits / 5;
    uint64_t upper = (2 * nbits) / 5;
    uint64_t pbits = 0;
    uint64_t qbits = 0;
	
	// get pbits in range(lower, upper)
    while (pbits == 0 || qbits == 0) {
        pbits = (random() % (upper - lower + 1)) + lower;
        qbits = nbits - 2 * pbits;
    }
	
	make_prime(p, pbits, iters);
	make_prime(q, qbits, iters);
	
	// n = p * p * q
	mpz_mul(n, p, p);
	mpz_mul(n, n, q);
}

//
// Generates components for a new SS private key.
//
// Provides:
//  d:  private exponent
//  pq: private modulus
//
// Requires:
//  p:  first prime number
//  q: second prime number
//  all mpz_t arguments to be initialized
//
void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
	// private modulus pq = p * q
	mpz_mul(pq, p, q);

	// private exponent d = (public_key^-1) mod lcm(p-1, q-1)
	mpz_t n, lcm, p_1, q_1;
	mpz_inits(n, lcm, p_1, q_1, NULL);
	
	// public key (p * p * q)
	mpz_mul(n, p, p);
    mpz_mul(n, n, q);
	
	// lcm(p - 1, q - 1)
	mpz_sub_ui(p_1, p, 1);
	mpz_sub_ui(q_1, q, 1);
	mpz_lcm(lcm, p_1, q_1);
	
	mod_inverse(d, n, lcm);

	mpz_clears(n, lcm, p_1, q_1, NULL);
}

//
// Export SS public key to output stream
//
// Requires:
//  n: public modulus/exponent
//  username: login name of keyholder ($USER)
//  pbfile: open and writable file stream
//
void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
	gmp_fprintf(pbfile, "%Zx\n", n);
    fprintf(pbfile, "%s\n", username);
}

//
// Export SS private key to output stream
//
// Requires:
//  pq: private modulus
//  d:  private exponent
//  pvfile: open and writable file stream
//
void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
	gmp_fprintf(pvfile, "%Zx\n", pq);
	gmp_fprintf(pvfile, "%Zx\n", d);
}

//
// Import SS public key from input stream
//
// Provides:
//  n: public modulus
//  username: $USER of the pubkey creator
//
// Requires:
//  pbfile: open and readable file stream
//  username: requires sufficient space
//  all mpz_t arguments to be initialized
//
void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
	gmp_fscanf(pbfile, "%Zx\n", n);
    fscanf(pbfile, "%s\n", username);
}

//
// Import SS private key from input stream
//
// Provides:
//  pq: private modulus
//  d:  private exponent
//
// Requires:
//  pvfile: open and readable file stream
//  all mpz_t arguments to be initialized
//
void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
	gmp_fscanf(pvfile, "%Zx\n", pq);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

//
// Encrypt number m into number c
//
// Provides:
//  c: encrypted integer
//
// Requires:
//  m: original integer
//  n: public exponent/modulus
//  all mpz_t arguments to be initialized
//
void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
	// c = m^N mod N
	mpz_powm(c, m, n, n);
}

//
// Encrypt an arbitrary file
//
// Provides:
//  fills outfile with the encrypted contents of infile
//
// Requires:
//  infile: open and readable file stream
//  outfile: open and writable file stream
//  n: public exponent and modulus
//
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
	// k=⌊(log2(sqrt n)−1)/8⌋
	uint64_t k;
	mpz_t root, m, c;
	mpz_inits(root, m, c, NULL);
	
	long int log2; 

	mpz_root(root, n, 2);
	mpz_get_d_2exp(&log2, root);
	k = (log2 - 1) / 8;

	uint8_t block[k];
	block[0] = 0xFF;

	while (!feof(infile)) {
		size_t bytes_read;
		bytes_read = fread(block + 1, 1, k - 1, infile);
		mpz_import(m, bytes_read + 1, 1, sizeof(block[0]), 1, 0, block);
		ss_encrypt(c, m, n);
		gmp_fprintf(outfile, "%Zx\n", c);
	}

	mpz_clears(root, m, c, NULL);
}

//
// Decrypt number c into number m
//
// Provides:
//  m: decrypted/original integer
//
// Requires:
//  c: encrypted integer
//  d: private exponent
//  pq: private modulus
//  all mpz_t arguments to be initialized
//
void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
	// m = c^d mod pq
	mpz_powm(m, c, d, pq);
}
//
// Decrypt a file back into its original form.
//
// Provides:
//  fills outfile with the unencrypted data from infile
//
// Requires:
//  infile: open and readable file stream to encrypted data
//  outfile: open and writable file stream
//  d: private exponent
//  pq: private modulus
//
void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
	// k=⌊(log2(sqrt pq)−1)/8⌋
    uint64_t k;
    mpz_t root, m, c;
    mpz_inits(root, m, c, NULL);
    
    long int log2;

    mpz_root(root, pq, 2);
    mpz_get_d_2exp(&log2, root);
    k = (log2 - 1) / 8;

    uint8_t block[k];
    
	while (!feof(infile)) {
		gmp_fscanf(infile, "%Zx\n", c);
		ss_decrypt(m, c, d, pq);
		size_t bytes_read;
		mpz_export(block, &bytes_read, 1, sizeof(block[0]), 1, 0, m);
		fwrite(block + 1, 1, bytes_read - 1, outfile);
	}

	mpz_clears(root, m, c, NULL);
}
