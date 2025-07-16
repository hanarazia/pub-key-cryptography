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
	char *pbfn = "ss.pub";
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
                pbfn = optarg;
                break;
            }

			case 'v': {
                verbose = 1;
                break;
            }

			case 'h': {
                printf("-h              Display program help and usage.\n");
                printf("-v              Display verbose program output.\n");
                printf("-i infile       Input file of data to encrypt (default: stdin).\n");
            	printf("-o outfile      Output file for encrypted data (default: stdout).\n");
				printf("-n pbfile       Public key file (default: ss.pub).\n");
        		exit(1);
			}
        }
    }
	
	// open public key file 
	FILE *pbfile = fopen(pbfn, "r");
	
	// print error and exit in event of failure
	if (!pbfile) {
		printf("Could not read %s file.\n", pbfn);
		exit(1);
	}

	if (!input) {
        printf("Could not read the input file.\n");
        exit(1);
    }

	mpz_t n;
	mpz_init(n);

	char user[256];
	ss_read_pub(n, user, pbfile);

	if (verbose) {
        gmp_printf("\tusername = %s\n", user);
		gmp_printf("\tpublic key = %Zu\n", n);
		gmp_printf("\tpublic key bit size: %d\n", mpz_sizeinbase(n, 16)*4);
	}

	ss_encrypt_file(input, output, n);

	mpz_clear(n);
	fclose(pbfile);
	return 0;

}
