#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>

#include "aes.c"

void test_transport()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t ret_state[16] = {
		0x01,0x05,0x09,0x0d,
		0x02,0x06,0x0a,0x0e,
		0x03,0x07,0x0b,0x0f,
		0x04,0x08,0x0c,0x10
	};
	transport(state);
	int ret = memcmp(ret_state, state, 16);
	CU_ASSERT_EQUAL(ret, 0);
}

void test_transport_uint32_t()
{
	uint32_t state[4] = {
		0x04030201,
		0x08070605,
		0x0c0b0a09,
		0x100f0e0d,
	};
	uint32_t ret_state[4] = {
		0x0d090501,
		0x0e0a0602,
		0x0f0b0703,
		0x100c0804,
	};
	transport((uint8_t *)state);
	int ret = memcmp(ret_state, state, 16);
	CU_ASSERT_EQUAL(ret, 0);
}

void test_add_round_key()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t key[16] = {
		0x05,0x05,0x05,0x05,
		0x05,0x05,0x05,0x05,
		0x05,0x05,0x05,0x05,
		0x05,0x05,0x05,0x05
	};
	uint8_t tmp_state[16];
	memcpy(tmp_state, state, sizeof(tmp_state));
	add_round_key(tmp_state, key);
	add_round_key(tmp_state, key);
	int ret = memcmp(tmp_state, state, sizeof(tmp_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_sub_bytes_back()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t tmp_state[16];
	memcpy(tmp_state, state, sizeof(tmp_state));
	sub_bytes(tmp_state);
	inv_sub_bytes(tmp_state);
	int ret = memcmp(tmp_state, state, sizeof(tmp_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_shift_rows()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t ret_state[16] = {
		0x01,0x06,0x0b,0x10,
		0x05,0x0a,0x0f,0x04,
		0x09,0x0e,0x03,0x08,
		0x0d,0x02,0x07,0x0c
	};
	shift_rows(state);
	int ret = memcmp(ret_state, state, sizeof(ret_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_inv_shift_rows()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t ret_state[16] = {
		0x01,0x0e,0x0b,0x08,
		0x05,0x02,0x0f,0x0c,
		0x09,0x06,0x03,0x10,
		0x0d,0x0a,0x07,0x04
	};
	inv_shift_rows(state);
	int ret = memcmp(ret_state, state, sizeof(ret_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_shift_rows_back()
{
	uint8_t state[16] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x09,0x0a,0x0b,0x0c,
		0x0d,0x0e,0x0f,0x10
	};
	uint8_t tmp_state[16];
	memcpy(tmp_state, state, sizeof(tmp_state));
	shift_rows(tmp_state);
	inv_shift_rows(tmp_state);
	int ret = memcmp(tmp_state, state, sizeof(tmp_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_GF_256_multiply()
{
	CU_ASSERT_EQUAL(GF_256_multiply(0x3a, 0x24), 0xe9);
	CU_ASSERT_EQUAL(GF_256_multiply(0x02, 0x87), 0x15);
	CU_ASSERT_EQUAL(GF_256_multiply(0x03, 0x6e), 0xb2);
	CU_ASSERT_EQUAL(GF_256_multiply(0x46, 0x01), 0x46);
	CU_ASSERT_EQUAL(GF_256_multiply(0xa6, 0x01), 0xa6);
}

void test_mix_columns()
{
	uint8_t state[16] = {
		0x87,0x6E,0x46,0xA6,
		0xF2,0x4C,0xE7,0x8C,
		0x4D,0x90,0x4A,0xD8,
		0x97,0xEC,0xC3,0x95
	};
	uint8_t ret_state[16] = {
		0x47,0x37,0x94,0xED,
		0x40,0xD4,0xE4,0xA5,
		0xA3,0x70,0x3A,0xA6,
		0x4C,0x9F,0x42,0xBC
	};
	mix_columns(state);
	int ret = memcmp(ret_state, state, sizeof(ret_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_inv_mix_columns()
{
	uint8_t state[16] = {
		0x47,0x37,0x94,0xED,
		0x40,0xD4,0xE4,0xA5,
		0xA3,0x70,0x3A,0xA6,
		0x4C,0x9F,0x42,0xBC
	};
	uint8_t ret_state[16] = {
		0x87,0x6E,0x46,0xA6,
		0xF2,0x4C,0xE7,0x8C,
		0x4D,0x90,0x4A,0xD8,
		0x97,0xEC,0xC3,0x95
	};
	inv_mix_columns(state);
	int ret = memcmp(ret_state, state, sizeof(ret_state));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_sub_word()
{
	CU_ASSERT_EQUAL(SUB_WORD(0x7f6798af), 0xd2854679);
}

void test_aes_set_key()
{
	uint8_t key[16] = {
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98
	};
	aes_context ctx;
	CU_ASSERT_EQUAL(aes_set_key(NULL, NULL, sizeof(key)*8), PARM_ERROR);
	CU_ASSERT_EQUAL(aes_set_key(NULL, NULL, 124), PARM_ERROR);
	CU_ASSERT_EQUAL(aes_set_key(&ctx, key, sizeof(key)*8), SUCCESS);
	CU_ASSERT_EQUAL(ctx.buf[20], 0xeb369d58);
	CU_ASSERT_EQUAL(ctx.buf[43], 0x76182686);
}

void test_aes_encrypt_block()
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
	uint8_t key[32] = {
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98,
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98
	};
	aes_context ctx;
	CU_ASSERT_EQUAL(aes_set_key(&ctx, key, 128), SUCCESS);
	CU_ASSERT_EQUAL(aes_encrypt_block(&ctx, cipher_text, text), SUCCESS);
	int ret = memcmp(ret_cipher_text, cipher_text, sizeof(ret_cipher_text));
	CU_ASSERT_EQUAL(ret, 0);
	
	CU_ASSERT_EQUAL(aes_set_key(&ctx, key, 192), SUCCESS);
	CU_ASSERT_EQUAL(aes_encrypt_block(&ctx, cipher_text, text), SUCCESS);
	CU_ASSERT_EQUAL(aes_decrypt_block(&ctx, ret_text, cipher_text), SUCCESS);
	ret = memcmp(ret_text, text, sizeof(cipher_text));
	CU_ASSERT_EQUAL(ret, 0);
	
	CU_ASSERT_EQUAL(aes_set_key(&ctx, key, 256), SUCCESS);
	CU_ASSERT_EQUAL(aes_encrypt_block(&ctx, cipher_text, text), SUCCESS);
	CU_ASSERT_EQUAL(aes_decrypt_block(&ctx, ret_text, cipher_text), SUCCESS);
	ret = memcmp(ret_text, text, sizeof(cipher_text));
	CU_ASSERT_EQUAL(ret, 0);
}

void test_aes_decrypt_block()
{
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10
	};
	uint8_t cipher_text[16] = {
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
	aes_context ctx;
	CU_ASSERT_EQUAL(aes_set_key(&ctx, key, sizeof(key)*8), SUCCESS);
	CU_ASSERT_EQUAL(aes_decrypt_block(&ctx, ret_text, cipher_text), SUCCESS);
	CU_ASSERT_EQUAL(memcmp(ret_text, text, 16), 0);
}

CU_TestInfo test_case[] = {
	{"test_transport:", test_transport},
	{"test_transport_uint32_t:", test_transport_uint32_t},
	{"test_add_round_key:", test_add_round_key},
	{"test_sub_bytes/test_inv_sub_bytes:", test_sub_bytes_back},
	{"test_shift_rows:", test_shift_rows},
	{"test_inv_shift_rows:", test_inv_shift_rows},
	{"test_shift_rows_back:", test_inv_shift_rows},
	{"test_GF_256_multiply:", test_GF_256_multiply},
	{"test_mix_columns:", test_mix_columns},
	{"test_inv_mix_columns:", test_inv_mix_columns},
	CU_TEST_INFO_NULL
};

CU_TestInfo test_case_macro[] = {
	{"test_sub_word:", test_sub_word},
	CU_TEST_INFO_NULL
};

CU_TestInfo test_case_main[] = {
	{"test_aes_set_key:", test_aes_set_key},
	{"test_aes_encrypt_block:", test_aes_encrypt_block},
	{"test_aes_decrypt_block:", test_aes_decrypt_block},
	CU_TEST_INFO_NULL
};

int suite_success_init(void){
	return 0;
}

int suite_success_clean(void){
	return 0;
}

CU_SuiteInfo suites[] = {
#if CUNIT_VER == 2
	{"testSuite1", suite_success_init, suite_success_clean, test_case},
	{"testSuite2", suite_success_init, suite_success_clean, test_case_macro},
	{"testSuite3", suite_success_init, suite_success_clean, test_case_main},
#elif CUNIT_VER == 1
	{"testSuite1", suite_success_init, suite_success_clean, NULL, NULL, test_case},
	{"testSuite2", suite_success_init, suite_success_clean, NULL, NULL, test_case_macro},
	{"testSuite3", suite_success_init, suite_success_clean, NULL, NULL, test_case_main},
#else
#error "unknown cunit version"
#endif
	CU_SUITE_INFO_NULL
};

void add_tests()
{
	assert(NULL != CU_get_registry());
	assert(!CU_is_test_running());
	
	if(CUE_SUCCESS != CU_register_suites(suites)) {
		exit(EXIT_FAILURE);
	}
}

int run_test()
{
	if(CU_initialize_registry()){
		fprintf(stderr, " Initialization of Test Registry failed. ");
		exit(EXIT_FAILURE);
	} else {
		add_tests();
		
		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();

//		CU_set_output_filename("TestMax");
//		CU_list_tests_to_file();
//		CU_automated_run_tests();

		CU_cleanup_registry();
		return CU_get_error();
	}
}

int main()
{
	return run_test();
}
