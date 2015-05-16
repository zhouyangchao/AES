#include <stdio.h>
#include "aes.h"

void print(const uint8_t *msg, const uint8_t *buf)
{
	printf("%s:", msg);
	int i;
	for(i=0; i<16; ++i)
		printf("%02x ", buf[i]);
	printf("\n");
}

int main()
{
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10
	};
	uint8_t cipher_text[16] = {0};
	uint8_t ret_cipher_text[16] = {
		0xff,0x0b,0x84,0x4a,
		0x08,0x53,0xbf,0x7c,
		0x69,0x34,0xab,0x43,
		0x64,0x14,0x8f,0xb9
	};
	uint8_t key[16] = {
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98
	};
	
	print("input  ", text);
	aes_context ctx;
	if (aes_set_key(&ctx, key, sizeof(key)*8) != SUCCESS)
	{
		perror("aes_set_key error.\n");
		return -1;
	}
	if(aes_encrypt_block(&ctx, cipher_text, text) != SUCCESS)
	{
		perror("aes_encrypt_block error.\n");
		return -1;
	}
	print("encrypt", cipher_text);
	if(aes_decrypt_block(&ctx, ret_text, cipher_text) != SUCCESS)
	{
		perror("aes_decrypt_block error.\n");
		return -1;
	}
	print("decrypt", ret_text);
}