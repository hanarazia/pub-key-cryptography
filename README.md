# Public Key Cryptography (C)

The purpose of this project is to understand cryptography by generating SS public and private key pairs which can then be used to encrypt and decrypt files. The files will be encrypted using a public key and then decrypted using the corresponding private key. This assignment implements the GNU MP arithmetic library and Schmidt-Samoa algorithm.

## How to Run Locally (Requires: C Compiler, GMP Library)

1. Clone the repo  
   ```
   git clone https://github.com/yourusername/pub-key-cryptography.git
   cd pub-key-cryptography
   ```
2. Build the project
  ```
  make
  ```
3. Run the tools
  ```
    ./keygen -b		Sets minimum bits needed for public key n
    ./keygen -i		Sets Miller-Rabin iterations for testing primes
    ./keygen -n		Sets public key file
    ./keygen -d		Sets private key file
    ./keygen -s		Sets random seed for testing
    ./keygen -v		Prints verbose program output
    ./keygen -h		Prints out program help and usage

    ./encrypt -i    Sets input file of data to encrypt
    ./encrypt -o	Sets output file for encrypted data
    ./encrypt -n	Sets public key file
    ./encrypt -v    Prints verbose program output
    ./encrypt -h    Prints out program help and usage

    ./decrypt -i    Sets input file of data to decrypt
    ./decrypt -o    Sets output file for decrypted data
    ./decrypt -n    Sets private key file
    ./decrypt -v    Prints verbose program output
    ./decrypt -h    Prints out program help and usage
  ```
  Example:
  ```
    ./keygen -b 512 -v
    ./encrypt -i testfile.txt -o encrypted.file
    ./decrypt -i encrypted.file -o final.txt
    diff testfile.txt final.txt
  ```

---

## Files in Repo

| File           | Description                                                                 |
|----------------|-----------------------------------------------------------------------------|
| `decrypt.c`    | Contains implementation and main function for the decrypt program           |
| `encrypt.c`    | Contains implementation and main function for the encrypt program           |
| `keygen.c`     | Contains implementation and main function for the keygen program            |
| `numtheory.c`  | Contains implementation of number theory functions                          |
| `numtheory.h`  | Header file that specifies the interface to `numtheory.c`                   |
| `randstate.c`  | Contains implementation of random state interface for SS and num theory     |
| `randstate.h`  | Header file that specifies the interface to `randstate.c`                   |
| `ss.c`         | Contains implementation of the Schmidt-Samoa cryptosystem functions         |
| `ss.h`         | Header file that specifies the interface to `ss.c`                          |
| `testfile.txt` | Sample input text used for encryption and decryption demonstration          |
| `Makefile`     | Compiles the programs and applies `clang-format` to all source files        |
