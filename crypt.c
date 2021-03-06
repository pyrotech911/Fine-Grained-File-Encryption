#include "fge.h"


void fprint_string_as_hex(FILE * f, unsigned char *s, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		fprintf(f, "%02x", s[i]);
	}
}


void initialize_key(unsigned char *key)
{
	FILE *rng;
	int num = 0;
	rng = fopen("/dev/urandom", "r");
	while (num < EVP_MAX_KEY_LENGTH) {
		num += fread(&key[num],1,EVP_MAX_KEY_LENGTH - num,rng);
	}
	fclose(rng);
}


unsigned char * allocate_ciphertext(int mlen)
{
	/* Alloc space for any possible padding. */
	return (unsigned char *) malloc(mlen + BLOCKSIZE);
}


int encrypt(unsigned char *ptxt, unsigned char *key, unsigned char *ctxt){
	EVP_CIPHER_CTX *ctx; /* SSL context */
	const EVP_CIPHER *cipher = EVP_bf_cbc();/* Cipher */
	//initialize_key(key);  Get random key for file
	//unsigned char key[EVP_MAX_KEY_LENGTH];  Initilize key to size
	char ivec[EVP_MAX_IV_LENGTH] = {0};
	int mlen = strlen(ptxt) + 1;
	int ctlen;
	int extlen;
	ctx = (EVP_CIPHER_CTX *) malloc(sizeof(EVP_CIPHER_CTX));
	/* Blowfish CBC mode
	128-bit keys
	(default size)*/
	/* Print key in HEX */
	EVP_CIPHER_CTX_init(ctx); 
	/* Initialization context */
	EVP_EncryptInit_ex(ctx, cipher, NULL, key, ivec);
	/* Allocate space for the ciphertext */
	ctxt = allocate_ciphertext(mlen);
	EVP_EncryptUpdate(ctx, ctxt, &ctlen, ptxt, mlen);
	EVP_EncryptFinal_ex(ctx, &ctxt[ctlen], &extlen);
	free(ctx);
	ctlen += extlen;
	return ctlen;
	//initialize_key(key);  Get random key for file
	//unsigned char key[EVP_MAX_KEY_LENGTH];  Initilize key to size
	//fprint_string_as_hex(stdout,key,EVP_CIPHER_key_length(cipher));
}


int decrypt(unsigned char *ptxt, unsigned char *key, unsigned char *ctxt, int ctlen){
	int ptlen;
	int extlen;
	EVP_CIPHER_CTX *ctx; /* SSL context */

	const EVP_CIPHER *cipher = EVP_bf_cbc();/* Blowfish Cipher */
	ctx = (EVP_CIPHER_CTX *) malloc(sizeof(EVP_CIPHER_CTX));
	char ivec[EVP_MAX_IV_LENGTH] = {0};
	ptxt = (unsigned char *) malloc(sizeof(char)*(ctlen));


	EVP_CIPHER_CTX_init(ctx); 
	EVP_DecryptInit_ex(ctx, cipher, NULL, key, ivec);
	EVP_DecryptUpdate(ctx, ptxt, &ptlen, ctxt, ctlen);
	EVP_DecryptFinal_ex(ctx, &ptxt[ptlen], &extlen);

	ptlen += extlen;
	free(ctx);
	return ptlen;
}

void hash(unsigned char *keytext, unsigned char *sha){
	int i;
	size_t length = sizeof(keytext);
	if ((strchr(keytext, '\n')) != NULL || (strchr(keytext, '\0')) != NULL){
		unsigned char temp[length -1];
		for (i = 0; i < length - 1; i++){
			temp[i] = keytext[i];
		}
		keytext = temp;
		length = length - 1;
	}
	SHA1(keytext, length, sha);
}