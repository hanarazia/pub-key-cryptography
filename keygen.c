#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <gmp.h>
#include "ss.h"
#include "randstate.h"

#define OPTIONS "b:i:n:d:s:vh"
#define NBITS 256

int main(int argc, char ** argv) {
	int opt;
    int nbits = NBITS;
    int iters = 50;
    char *pbfn = "ss.pub";
    char *pvfn = "ss.priv";
    int seed = time(NULL);
    int verbose = 0;

    while ((opt = getopt (argc, argv, OPTIONS)) != -1) {
        switch (opt) {
            case 'b': {
                nbits = atoi(optarg);
				break;
            }

            case 'i': {
                iters = atoi(optarg);
                break;
            }

            case 'n': {
                pbfn = optarg;
                break;
            }

            case 'd': {
                pvfn = optarg;
				break;
            }
			
			case 's': {
				seed = atoi(optarg);
				break;
			}

			case 'v': {
				verbose = 1;
				break;
			}

			case 'h': {
				printf("-h              Display program help and usage.\n");
				printf("-v              Display verbose program output.\n");
				printf("-b bits         Minimum bits needed for public key n (default: 256).\n");
				printf("-i iterations   Miller-Rabin iterations for testing primes (default: 50).\n");
				printf("-n pbfile       Public key file (default: ss.pub).\n");
				printf("-d pvfile       Private key file (default: ss.priv).\n");
				printf("-s seed         Random seed for testing.\n");
			}
		}
	}

	if (nbits < NBITS) {
        printf("Please use a higher value (> %d) for nbits (option -b).\n", NBITS);
        exit(1);
    }
	
	// initialize random state
	randstate_init(seed);

	// open public and private key files
	FILE *pbfile = fopen(pbfn, "w");
	FILE *pvfile = fopen(pvfn, "w");
	
	// print error and exit in event of failure
	if (!pbfile) {
        printf("Could not write %s file.\n", pbfn);
        exit(1);
    }

	if (!pvfile) {
        printf("Could not write %s file.\n", pvfn);
        exit(1);
    }

	// permissions for user
	int fn = fileno(pvfile);
    fchmod(fn, S_IRUSR|S_IWUSR);

	mpz_t p, q, n, d, pq;
    mpz_inits(p, q, n, d, pq, NULL);

	ss_make_pub(p, q, n, nbits, iters);
    ss_make_priv(d, pq, p, q);

	// get current user's name as a string
	char* user = getenv("USER");
	ss_write_pub(n, user, pbfile);
    ss_write_priv(pq, d, pvfile);

	if (verbose) {
        gmp_printf("\tusername = %s\n", user);
        gmp_printf("\tprime number p = %Zu\n", p);
		gmp_printf("\tprime number p bit size: %d\n", mpz_sizeinbase(p, 16)*4);
        gmp_printf("\tprime number q = %Zu\n", q);
		gmp_printf("\tprime number q bit size: %d\n", mpz_sizeinbase(q, 16)*4);
        gmp_printf("\tpublic key = %Zu\n", n);
		gmp_printf("\tpublic key bit size: %d\n", mpz_sizeinbase(n, 16)*4);
        gmp_printf("\tprivate exponent = %Zu\n", d);
		gmp_printf("\tprivate exponent bit size: %d\n", mpz_sizeinbase(d, 16)*4);
        gmp_printf("\tprivate modulus  = %Zu\n", pq);
		gmp_printf("\tprivate modulus bit size: %d\n", mpz_sizeinbase(pq, 16)*4);

    }

	// clear mpz objects
	mpz_clears(p, q, n, d, pq, NULL);
	fclose(pbfile);
	fclose(pvfile);
	randstate_clear();
	return 0;

}

