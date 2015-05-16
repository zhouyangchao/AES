#include<string.h>
#include<assert.h>
#include "aes.h"
#include "conf.c"

#ifndef GET_UINT32
#define GET_UINT32(n,b,i)                               \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ]       )             \
        | ( (uint32_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 2] << 16 )             \
        | ( (uint32_t) (b)[(i) + 3] << 24 );            \
}
#endif

#define ROTL8(x) (((x) << 24) | ((x) >> 8))
#define ROTL16(x) (((x) << 16) | ((x) >> 16))
#define ROTL24(x) (((x) << 8) | ((x) >> 24))

#define SUB_WORD(x) (((uint32_t)S_BOX[(x)&0xFF]) \
	| ((uint32_t)S_BOX[((x) >>  8)&0xFF] << 8) \
	| ((uint32_t)S_BOX[((x) >> 16)&0xFF] << 16) \
	| ((uint32_t)S_BOX[((x) >> 24)&0xFF] << 24) \
	)

static void rotate(uint8_t state[16])
{
	assert(state != NULL);
	uint8_t _state[4][4];
	int r,c;
	for(r=0; r<4; ++r)
		for(c=0; c<4; ++c)
			_state[r][c] = state[4*c+r];
	memcpy(state, _state, sizeof(_state));
}

static void add_round_key(uint8_t state[16], const uint8_t key[16])
{
	assert(state != NULL);
	assert(key != NULL);
	int i;
	for (i=0; i<16; ++i)
		state[i] ^= key[i];
}

static void sub_bytes(uint8_t state[16])
{
	assert(state != NULL);
	int i;
	for (i=0; i<16; ++i)
		state[i] = S_BOX[state[i]];
}

static void inv_sub_bytes(uint8_t state[16])
{
	assert(state != NULL);
	int i;
	for (i=0; i<16; ++i)
		state[i] = INV_S_BOX[state[i]];
}

static void shift_rows(uint8_t state[16])
{
	assert(state != NULL);
	rotate(state);
	*(uint32_t *)(state+4) = ROTL8(*(uint32_t *)(state+4));
	*(uint32_t *)(state+8) = ROTL16(*(uint32_t *)(state+8));
	*(uint32_t *)(state+12) = ROTL24(*(uint32_t *)(state+12));
	rotate(state);
}

static void inv_shift_rows(uint8_t state[16])
{
	assert(state != NULL);
	rotate(state);
	*(uint32_t *)(state+4) = ROTL24(*(uint32_t *)(state+4));
	*(uint32_t *)(state+8) = ROTL16(*(uint32_t *)(state+8));
	*(uint32_t *)(state+12) = ROTL8(*(uint32_t *)(state+12));
	rotate(state);
}

static uint8_t GF_256(uint8_t a, uint8_t b)
{
	uint8_t t[8] = { a };
	uint8_t ret = 0x00;
	int i = 0;
	for (i = 1; i < 8; ++i) {
		t[i] = t[i-1] << 1;
		if (t[i-1]&0x80) t[i] ^= 0x1b;
	}
	for (i = 0; i < 8; ++i)
		ret ^= (((b >> i) & 0x01) * t[i]);
	return ret;
}

static void _mix_columns(uint8_t state[16], const uint8_t matrix[][4])
{
	assert(state != NULL);
	uint8_t _state[16] = {0};
	int r,c,i;
	for(r = 0; r < 4; ++r)
		for(c = 0; c < 4; ++c)
			for(i = 0; i < 4; ++i)
				_state[c*4+r] ^= GF_256(matrix[r][i], state[c*4+i]);
	memcpy(state, _state, sizeof(_state));
}

#define mix_columns(state) _mix_columns(state, MIX)
#define inv_mix_columns(state) _mix_columns(state, INV_MIX)

static void aes_round(uint8_t state[16], const uint8_t rk[16])
{
	sub_bytes(state);
	shift_rows(state);
	mix_columns(state);
	add_round_key(state, rk);
}
static void aes_inv_round(uint8_t state[16], const uint8_t inv_rk[16])
{
	inv_shift_rows(state);
	inv_sub_bytes(state);
	add_round_key(state, inv_rk);
	inv_mix_columns(state);
}

static void aes_final_round(uint8_t state[16], const uint8_t rk[16])
{
	sub_bytes(state);
	shift_rows(state);
	add_round_key(state, rk);
}
static void inv_final_round(uint8_t state[16], const uint8_t inv_rk[16])
{
	inv_shift_rows(state);
	inv_sub_bytes(state);
	add_round_key(state, inv_rk);
}

static void key_expansion(aes_context *ctx, const uint8_t *key)
{
	assert(ctx != NULL);
	assert(key != NULL);
	uint32_t Nk = ctx->nr - 6;
	uint32_t Nr = ctx->nr;
	uint32_t *RK = ctx->rk;
	
	uint32_t i = 0;
	do
	{
		GET_UINT32(RK[i], key, i<<2);
	} while(++i < Nk);
	do
	{
		uint32_t t = RK[i-1];
		if((i % Nk) == 0)
			t = SUB_WORD(ROTL8(t))^RCON[i/Nk -1];
		else if(Nk == 8 && (i % Nk) == 4)
			t = SUB_WORD(t);
		RK[i] = RK[i-Nk]^t;
	} while(++i < ((Nr+1)<<2));
}

int aes_set_key(aes_context *ctx, const uint8_t *key, uint32_t key_bit)
{
	if (ctx == NULL || key == NULL)
		return PARM_ERROR;
	memset(ctx, 0, sizeof(aes_context));
	switch (key_bit)
	{
		case 128: ctx->nr = 10; break;
		case 192: ctx->nr = 12; break;
		case 256: ctx->nr = 14; break;
		default: return PARM_ERROR;
	}
	ctx->rk = ctx->buf;
	key_expansion(ctx, key);
	return SUCCESS;
}

int aes_encrypt_block(aes_context *ctx
	, uint8_t cipher_text[16], const uint8_t text[16])
{
	if(ctx == NULL || cipher_text == NULL || text == NULL)
		return PARM_ERROR;
	if(ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *RK = ctx->rk;
	memcpy(cipher_text, text, 16);
	uint8_t *state = cipher_text;

	add_round_key(state, (const uint8_t *)RK);
	uint32_t i;
	for(i=1; i<Nr; ++i)
		aes_round(state, (const uint8_t *)(RK + i*4));
	aes_final_round(state, (const uint8_t *)(RK + Nr*4));
	
	return SUCCESS;
}

int aes_decrypt_block(aes_context *ctx
	, uint8_t text[16], const uint8_t cipher_text[16])
{
	if(ctx == NULL || text == NULL || cipher_text == NULL)
		return PARM_ERROR;
	if(ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *INV_RK = ctx->rk;
	uint8_t *state = text;
	memcpy(state, cipher_text, 16);
	
	add_round_key(state, (const uint8_t *)(INV_RK + Nr*4));
	uint32_t i;
	for(i=Nr-1; i>0; --i)
		aes_inv_round(state, (const uint8_t *)(INV_RK + i*4));
	inv_final_round(state, (const uint8_t *)INV_RK);
	
	return SUCCESS;
}
