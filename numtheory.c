#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include "randstate.h"

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
	mpz_t t, aa, bb;
	mpz_inits(t, aa, bb, NULL);
	mpz_set(aa, a);
	mpz_set(bb, b);

	// while bb != 0
	while (mpz_cmp_ui(bb, 0) != 0) {
		// t <- bb
		mpz_set(t, bb);
		// bb <- aa mod bb
		mpz_mod(bb, aa, bb);
		// aa <- t
		mpz_set(aa, t);
	}
	
	mpz_set(g, aa);
	mpz_clears(t, aa, bb, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
	mpz_t r, t, q, r_prime, t_prime, tmp;
	mpz_inits(r, t, q, r_prime, t_prime, tmp, NULL);
	
	// (r, r_prime) <- (n, a)
	mpz_set(r, n);
	mpz_set(r_prime, a);
	
	// (t, t_prime) <- (0, 1)
	mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);

	while (mpz_cmp_ui(r_prime, 0) != 0) {
		// q <- [r, r_prime]
		mpz_fdiv_q(q, r, r_prime);
		
		// r_prime = r - (q * r_prime)
		mpz_set(tmp, r_prime);
		mpz_mul(r_prime, q, r_prime);
		mpz_sub(r_prime, r, r_prime);
		mpz_set(r, tmp);


		// t_prime = t - (q * t_prime)
		mpz_set(tmp, t_prime);
		mpz_mul(t_prime, q, t_prime);
		mpz_sub(t_prime, t, t_prime);
		mpz_set(t, tmp);
	}

	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_set_ui(o, 0);
	}

	else if (mpz_cmp_si(t, 0) < 0) {
		mpz_add(o, t, n);
	}
    
	else {
		mpz_set(o, t);
	}

	mpz_clears(r, t, q, r_prime, t_prime, tmp, NULL);
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
	mpz_t p, dd;
	mpz_inits(p, dd, NULL);
	
	// p <- a
	mpz_set(p, a);
	mpz_set(dd, d);
	// o <- 1
	mpz_set_ui(o, 1);
    
	// while d > 0
	while (mpz_cmp_ui(dd, 0) > 0) {
		if (mpz_odd_p(dd)) {
			// o <- (o * p) mod n
			mpz_mul(o, o, p);
			mpz_mod(o, o, n);
		}
		// p <- (p * p) mod n
		mpz_mul(p, p, p);
		mpz_mod(p, p, n);
		// dd <- [dd / 2]
		mpz_div_ui(dd, dd, 2); 
	}
	
	mpz_clears(p, dd, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {
	mpz_t n_mod_2, r, r_mod_2, s, temp_random, n_minus_3, a, y, 
		n_minus_1, j, s_minus_1, two;
    mpz_inits(n_mod_2, r, r_mod_2, s, temp_random, n_minus_3, a, y, 
		n_minus_1, j, s_minus_1, two, NULL);

    // set n_mod_2 to n % 2
    mpz_mod_ui(n_mod_2, n, 2);

    // if (n < 2) or (n != 2 && n % 2 == 0) then return false
    if ((mpz_cmp_si(n, 2) == -1) ||
        ((mpz_cmp_si(n, 2) != 0) && (mpz_cmp_si(n_mod_2, 0) == 0))) {
        return false;
    }

    // if n == 2 or n == 3 then return true
    if ((mpz_cmp_si(n, 2) == 0) || (mpz_cmp_si(n, 3) == 0)) {
        return true;
    }

	// write n - 1 = 2^s * r such that r is odd
    mpz_set_si(s, 0);
    mpz_sub_ui(r, n, 1);
    bool done = false;
    while (!done) {
        mpz_mod_ui(r_mod_2, r, 2);
        if (mpz_cmp_si(r_mod_2, 0) == 0) {
            mpz_fdiv_q_ui(r, r, 2);
            mpz_add_ui(s, s, 1);
        }
        else {
            done = true;
        }
    }

    mpz_sub_ui(n_minus_1, n, 1);
    mpz_sub_ui(s_minus_1, s, 1);
    mpz_set_ui(two, 2);
	uint64_t i;
    
	for (i = 0; i < iters; i++) {
        // get random number between (2, n-2)
		// invoking mpz_urandomm with (n-3) will give random number between (0, n-4), add 2 to make it in range (2, n-2)
        mpz_sub_ui(n_minus_3, n, 3);
        mpz_urandomm(temp_random, state, n_minus_3);
        mpz_add_ui(a, temp_random, 2);

		pow_mod(y, a, r, n);

        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus_1) != 0) {
            mpz_set_si(j, 1);

            while ((mpz_cmp(j, s_minus_1) <= 0) &&
                   (mpz_cmp(y, n_minus_1) != 0)) {
                pow_mod(y, y, two, n);
                
				if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(n_mod_2, r, r_mod_2, s, temp_random, n_minus_3,
                            a, y, n_minus_1, j, s_minus_1, two, NULL);
                    return false;
                }
               
			   mpz_add_ui(j, j, 1);
            }
			
			if (mpz_cmp(y, n_minus_1) != 0) {
                mpz_clears(n_mod_2, r, r_mod_2, s, temp_random, n_minus_3,
                        a, y, n_minus_1, j, s_minus_1, two, NULL);
                return false;
            }
        }
    }

    // clear mpz objects
    mpz_clears(n_mod_2, r, r_mod_2, s, temp_random, n_minus_3, a, y,
            n_minus_1, j, s_minus_1, two, NULL);
    return true;
}


void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	mpz_t prime, two, temp_random;
    mpz_inits(prime, two, temp_random, NULL);

    // smallest number that takes "bits" bits is 2**(bits - 1)
    mpz_set_ui(two, 2);
    mpz_pow_ui(prime, two, bits - 1);

    // add some randomization (prime = smallest + rand(0, smallest)
    mpz_urandomm(temp_random, state, prime);
    mpz_set(prime, temp_random);

    // make sure prime is odd
    if (mpz_even_p(prime))
        mpz_sub_ui(prime, prime, 1);

    while (!is_prime(prime, iters)) {
        mpz_add_ui(prime, prime, 2);
    }

    mpz_set(p, prime);
    mpz_clears(prime, two, temp_random, NULL);
}
