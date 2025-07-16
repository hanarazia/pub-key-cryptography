#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <gmp.h>
#include "ss.h"

#define OPTIONS "i:o:n:vh"

int main(int argc, char ** argv) {
    int opt;
    FILE *input = stdin;
    FILE *output = stdout;
    char *pvfn = "ss.priv";
    int verbose = 0;

    while ((opt = getopt (argc, argv, OPTIONS)) != -1) {
        switch (opt) {
            case 'i': { 
                input = fopen(optarg, "r");
                break;
            }

			case 'o': {
                output = fopen(optarg, "w");
                break;
            }

            case 'n': {
                pvfn = optarg;
                break;
            }

            case 'v': {
                verbose = 1;
                break;
            }

            case 'h': {
                printf("-h              Display program help and usage.\n");
                printf("-v              Display verbose program output.\n");
                printf("-i infile       Input file of data to decrypt (default: stdin).\n");
                printf("-o outfile      Output file for decrypted data (default: stdout).\n");
                printf("-n pvfile       Private key file (default: ss.priv).\n");
				exit(1);
			}
        }
    }
	
	// open private key file
	FILE *pvfile = fopen(pvfn, "r");

    // print error and exit in event of failure
	if (!pvfile) {
        printf("Could not read %s file.\n", pvfn);
        exit(1);
    }

    if (!input) {
        printf("Could not read the input file.\n");
        exit(1);
    }

	mpz_t pq, d; 
    mpz_inits(pq, d, NULL);

    ss_read_priv(pq, d, pvfile);
	
	if (verbose) {
        gmp_printf("\tprivate exponent = %Zu\n", d);
		gmp_printf("\tprivate exponent bit size: %d\n", mpz_sizeinbase(d, 16)*4);
        gmp_printf("\tprivate modulus  = %Zu\n", pq);
		gmp_printf("\tprivate modulus bit size: %d\n", mpz_sizeinbase(pq, 16)*4);
    }

	ss_decrypt_file(input, output, d, pq);

	mpz_clears(pq, d, NULL);
	fclose(pvfile);
	return 0;

}
