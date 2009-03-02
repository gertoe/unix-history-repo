/*
 * Test program for SHA1 and MD5
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "sha1.h"
#include "md5.h"
#include "crypto.h"


static int test_eap_fast(void)
{
	/* RFC 4851, Appendix B.1 */
	const u8 pac_key[] = {
		0x0B, 0x97, 0x39, 0x0F, 0x37, 0x51, 0x78, 0x09,
		0x81, 0x1E, 0xFD, 0x9C, 0x6E, 0x65, 0x94, 0x2B,
		0x63, 0x2C, 0xE9, 0x53, 0x89, 0x38, 0x08, 0xBA,
		0x36, 0x0B, 0x03, 0x7C, 0xD1, 0x85, 0xE4, 0x14
	};
	const u8 seed[] = {
		0x3F, 0xFB, 0x11, 0xC4, 0x6C, 0xBF, 0xA5, 0x7A,
		0x54, 0x40, 0xDA, 0xE8, 0x22, 0xD3, 0x11, 0xD3,
		0xF7, 0x6D, 0xE4, 0x1D, 0xD9, 0x33, 0xE5, 0x93,
		0x70, 0x97, 0xEB, 0xA9, 0xB3, 0x66, 0xF4, 0x2A,
		0x00, 0x00, 0x00, 0x02, 0x6A, 0x66, 0x43, 0x2A,
		0x8D, 0x14, 0x43, 0x2C, 0xEC, 0x58, 0x2D, 0x2F,
		0xC7, 0x9C, 0x33, 0x64, 0xBA, 0x04, 0xAD, 0x3A,
		0x52, 0x54, 0xD6, 0xA5, 0x79, 0xAD, 0x1E, 0x00
	};
	const u8 master_secret[] = {
		0x4A, 0x1A, 0x51, 0x2C, 0x01, 0x60, 0xBC, 0x02,
		0x3C, 0xCF, 0xBC, 0x83, 0x3F, 0x03, 0xBC, 0x64,
		0x88, 0xC1, 0x31, 0x2F, 0x0B, 0xA9, 0xA2, 0x77,
		0x16, 0xA8, 0xD8, 0xE8, 0xBD, 0xC9, 0xD2, 0x29,
		0x38, 0x4B, 0x7A, 0x85, 0xBE, 0x16, 0x4D, 0x27,
		0x33, 0xD5, 0x24, 0x79, 0x87, 0xB1, 0xC5, 0xA2  
	};
	const u8 key_block[] = {
		0x59, 0x59, 0xBE, 0x8E, 0x41, 0x3A, 0x77, 0x74,
		0x8B, 0xB2, 0xE5, 0xD3, 0x60, 0xAC, 0x4D, 0x35,
		0xDF, 0xFB, 0xC8, 0x1E, 0x9C, 0x24, 0x9C, 0x8B,
		0x0E, 0xC3, 0x1D, 0x72, 0xC8, 0x84, 0x9D, 0x57,
		0x48, 0x51, 0x2E, 0x45, 0x97, 0x6C, 0x88, 0x70,
		0xBE, 0x5F, 0x01, 0xD3, 0x64, 0xE7, 0x4C, 0xBB,
		0x11, 0x24, 0xE3, 0x49, 0xE2, 0x3B, 0xCD, 0xEF,
		0x7A, 0xB3, 0x05, 0x39, 0x5D, 0x64, 0x8A, 0x44,
		0x11, 0xB6, 0x69, 0x88, 0x34, 0x2E, 0x8E, 0x29,
		0xD6, 0x4B, 0x7D, 0x72, 0x17, 0x59, 0x28, 0x05,
		0xAF, 0xF9, 0xB7, 0xFF, 0x66, 0x6D, 0xA1, 0x96,
		0x8F, 0x0B, 0x5E, 0x06, 0x46, 0x7A, 0x44, 0x84,
		0x64, 0xC1, 0xC8, 0x0C, 0x96, 0x44, 0x09, 0x98,
		0xFF, 0x92, 0xA8, 0xB4, 0xC6, 0x42, 0x28, 0x71
	};
	const u8 sks[] = {
		0xD6, 0x4B, 0x7D, 0x72, 0x17, 0x59, 0x28, 0x05,
		0xAF, 0xF9, 0xB7, 0xFF, 0x66, 0x6D, 0xA1, 0x96,
		0x8F, 0x0B, 0x5E, 0x06, 0x46, 0x7A, 0x44, 0x84,
		0x64, 0xC1, 0xC8, 0x0C, 0x96, 0x44, 0x09, 0x98,
		0xFF, 0x92, 0xA8, 0xB4, 0xC6, 0x42, 0x28, 0x71
	};
	const u8 isk[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	const u8 imck[] = {
		0x16, 0x15, 0x3C, 0x3F, 0x21, 0x55, 0xEF, 0xD9,
		0x7F, 0x34, 0xAE, 0xC8, 0x1A, 0x4E, 0x66, 0x80,
		0x4C, 0xC3, 0x76, 0xF2, 0x8A, 0xA9, 0x6F, 0x96,
		0xC2, 0x54, 0x5F, 0x8C, 0xAB, 0x65, 0x02, 0xE1,
		0x18, 0x40, 0x7B, 0x56, 0xBE, 0xEA, 0xA7, 0xC5,
		0x76, 0x5D, 0x8F, 0x0B, 0xC5, 0x07, 0xC6, 0xB9,
		0x04, 0xD0, 0x69, 0x56, 0x72, 0x8B, 0x6B, 0xB8,
		0x15, 0xEC, 0x57, 0x7B
	};
	const u8 msk[] = {
		0x4D, 0x83, 0xA9, 0xBE, 0x6F, 0x8A, 0x74, 0xED,
		0x6A, 0x02, 0x66, 0x0A, 0x63, 0x4D, 0x2C, 0x33,
		0xC2, 0xDA, 0x60, 0x15, 0xC6, 0x37, 0x04, 0x51,
		0x90, 0x38, 0x63, 0xDA, 0x54, 0x3E, 0x14, 0xB9,
		0x27, 0x99, 0x18, 0x1E, 0x07, 0xBF, 0x0F, 0x5A,
		0x5E, 0x3C, 0x32, 0x93, 0x80, 0x8C, 0x6C, 0x49,
		0x67, 0xED, 0x24, 0xFE, 0x45, 0x40, 0xA0, 0x59,
		0x5E, 0x37, 0xC2, 0xE9, 0xD0, 0x5D, 0x0A, 0xE3
	};
	const u8 emsk[] = {
		0x3A, 0xD4, 0xAB, 0xDB, 0x76, 0xB2, 0x7F, 0x3B,
		0xEA, 0x32, 0x2C, 0x2B, 0x74, 0xF4, 0x28, 0x55,
		0xEF, 0x2D, 0xBA, 0x78, 0xC9, 0x57, 0x2F, 0x0D,
		0x06, 0xCD, 0x51, 0x7C, 0x20, 0x93, 0x98, 0xA9,
		0x76, 0xEA, 0x70, 0x21, 0xD7, 0x0E, 0x25, 0x54,
		0x97, 0xED, 0xB2, 0x8A, 0xF6, 0xED, 0xFD, 0x0A,
		0x2A, 0xE7, 0xA1, 0x58, 0x90, 0x10, 0x50, 0x44,
		0xB3, 0x82, 0x85, 0xDB, 0x06, 0x14, 0xD2, 0xF9
	};
	/* RFC 4851, Appendix B.2 */
	u8 tlv[] = {
		0x80, 0x0C, 0x00, 0x38, 0x00, 0x01, 0x01, 0x00,
		0xD8, 0x6A, 0x8C, 0x68, 0x3C, 0x32, 0x31, 0xA8,
		0x56, 0x63, 0xB6, 0x40, 0x21, 0xFE, 0x21, 0x14,
		0x4E, 0xE7, 0x54, 0x20, 0x79, 0x2D, 0x42, 0x62,
		0xC9, 0xBF, 0x53, 0x7F, 0x54, 0xFD, 0xAC, 0x58,
		0x43, 0x24, 0x6E, 0x30, 0x92, 0x17, 0x6D, 0xCF,
		0xE6, 0xE0, 0x69, 0xEB, 0x33, 0x61, 0x6A, 0xCC,
		0x05, 0xC5, 0x5B, 0xB7
	};
	const u8 compound_mac[] = {
		0x43, 0x24, 0x6E, 0x30, 0x92, 0x17, 0x6D, 0xCF,
		0xE6, 0xE0, 0x69, 0xEB, 0x33, 0x61, 0x6A, 0xCC,
		0x05, 0xC5, 0x5B, 0xB7
	};
	u8 buf[512];
	const u8 *simck, *cmk;
	int errors = 0;

	printf("EAP-FAST test cases\n");

	printf("- T-PRF (SHA1) test case / master_secret\n");
	sha1_t_prf(pac_key, sizeof(pac_key), "PAC to master secret label hash",
		   seed, sizeof(seed), buf, sizeof(master_secret));
	if (memcmp(master_secret, buf, sizeof(master_secret)) != 0) {
		printf("T-PRF test - FAILED!\n");
		errors++;
	}

	printf("- PRF (TLS, SHA1/MD5) test case / key_block\n");
	tls_prf(master_secret, sizeof(master_secret), "key expansion",
		seed, sizeof(seed), buf, sizeof(key_block));
	if (memcmp(key_block, buf, sizeof(key_block)) != 0) {
		printf("PRF test - FAILED!\n");
		errors++;
	}

	printf("- T-PRF (SHA1) test case / IMCK\n");
	sha1_t_prf(sks, sizeof(sks), "Inner Methods Compound Keys",
		   isk, sizeof(isk), buf, sizeof(imck));
	if (memcmp(imck, buf, sizeof(imck)) != 0) {
		printf("T-PRF test - FAILED!\n");
		errors++;
	}

	simck = imck;
	cmk = imck + 40;

	printf("- T-PRF (SHA1) test case / MSK\n");
	sha1_t_prf(simck, 40, "Session Key Generating Function",
		   (u8 *) "", 0, buf, sizeof(msk));
	if (memcmp(msk, buf, sizeof(msk)) != 0) {
		printf("T-PRF test - FAILED!\n");
		errors++;
	}

	printf("- T-PRF (SHA1) test case / EMSK\n");
	sha1_t_prf(simck, 40, "Extended Session Key Generating Function",
		   (u8 *) "", 0, buf, sizeof(msk));
	if (memcmp(emsk, buf, sizeof(emsk)) != 0) {
		printf("T-PRF test - FAILED!\n");
		errors++;
	}

	printf("- Compound MAC test case\n");
	memset(tlv + sizeof(tlv) - 20, 0, 20);
	hmac_sha1(cmk, 20, tlv, sizeof(tlv), tlv + sizeof(tlv) - 20);
	if (memcmp(tlv + sizeof(tlv) - 20, compound_mac, sizeof(compound_mac))
	    != 0) {
		printf("Compound MAC test - FAILED!\n");
		errors++;
	}

	return errors;
}


static u8 key0[] =
{
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0b, 0x0b, 0x0b, 0x0b
};
static u8 data0[] = "Hi There";
static u8 prf0[] =
{
	0xbc, 0xd4, 0xc6, 0x50, 0xb3, 0x0b, 0x96, 0x84,
	0x95, 0x18, 0x29, 0xe0, 0xd7, 0x5f, 0x9d, 0x54,
	0xb8, 0x62, 0x17, 0x5e, 0xd9, 0xf0, 0x06, 0x06,
	0xe1, 0x7d, 0x8d, 0xa3, 0x54, 0x02, 0xff, 0xee,
	0x75, 0xdf, 0x78, 0xc3, 0xd3, 0x1e, 0x0f, 0x88,
	0x9f, 0x01, 0x21, 0x20, 0xc0, 0x86, 0x2b, 0xeb,
	0x67, 0x75, 0x3e, 0x74, 0x39, 0xae, 0x24, 0x2e,
	0xdb, 0x83, 0x73, 0x69, 0x83, 0x56, 0xcf, 0x5a
};

static u8 key1[] = "Jefe";
static u8 data1[] = "what do ya want for nothing?";
static u8 prf1[] =
{
	0x51, 0xf4, 0xde, 0x5b, 0x33, 0xf2, 0x49, 0xad,
	0xf8, 0x1a, 0xeb, 0x71, 0x3a, 0x3c, 0x20, 0xf4,
	0xfe, 0x63, 0x14, 0x46, 0xfa, 0xbd, 0xfa, 0x58,
	0x24, 0x47, 0x59, 0xae, 0x58, 0xef, 0x90, 0x09,
	0xa9, 0x9a, 0xbf, 0x4e, 0xac, 0x2c, 0xa5, 0xfa,
	0x87, 0xe6, 0x92, 0xc4, 0x40, 0xeb, 0x40, 0x02,
	0x3e, 0x7b, 0xab, 0xb2, 0x06, 0xd6, 0x1d, 0xe7,
	0xb9, 0x2f, 0x41, 0x52, 0x90, 0x92, 0xb8, 0xfc
};


static u8 key2[] =
{
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa
};
static u8 data2[] =
{
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
	0xdd, 0xdd
};
static u8 prf2[] =
{
	0xe1, 0xac, 0x54, 0x6e, 0xc4, 0xcb, 0x63, 0x6f,
	0x99, 0x76, 0x48, 0x7b, 0xe5, 0xc8, 0x6b, 0xe1,
	0x7a, 0x02, 0x52, 0xca, 0x5d, 0x8d, 0x8d, 0xf1,
	0x2c, 0xfb, 0x04, 0x73, 0x52, 0x52, 0x49, 0xce,
	0x9d, 0xd8, 0xd1, 0x77, 0xea, 0xd7, 0x10, 0xbc,
	0x9b, 0x59, 0x05, 0x47, 0x23, 0x91, 0x07, 0xae,
	0xf7, 0xb4, 0xab, 0xd4, 0x3d, 0x87, 0xf0, 0xa6,
	0x8f, 0x1c, 0xbd, 0x9e, 0x2b, 0x6f, 0x76, 0x07
};


struct passphrase_test {
	char *passphrase;
	char *ssid;
	char psk[32];
};

static struct passphrase_test passphrase_tests[] =
{
	{
		"password",
		"IEEE",
		{
			0xf4, 0x2c, 0x6f, 0xc5, 0x2d, 0xf0, 0xeb, 0xef,
			0x9e, 0xbb, 0x4b, 0x90, 0xb3, 0x8a, 0x5f, 0x90,
			0x2e, 0x83, 0xfe, 0x1b, 0x13, 0x5a, 0x70, 0xe2,
			0x3a, 0xed, 0x76, 0x2e, 0x97, 0x10, 0xa1, 0x2e
		}
	},
	{
		"ThisIsAPassword",
		"ThisIsASSID",
		{
			0x0d, 0xc0, 0xd6, 0xeb, 0x90, 0x55, 0x5e, 0xd6,
			0x41, 0x97, 0x56, 0xb9, 0xa1, 0x5e, 0xc3, 0xe3,
			0x20, 0x9b, 0x63, 0xdf, 0x70, 0x7d, 0xd5, 0x08,
			0xd1, 0x45, 0x81, 0xf8, 0x98, 0x27, 0x21, 0xaf
		}
	},
	{
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
		{
			0xbe, 0xcb, 0x93, 0x86, 0x6b, 0xb8, 0xc3, 0x83,
			0x2c, 0xb7, 0x77, 0xc2, 0xf5, 0x59, 0x80, 0x7c,
			0x8c, 0x59, 0xaf, 0xcb, 0x6e, 0xae, 0x73, 0x48,
			0x85, 0x00, 0x13, 0x00, 0xa9, 0x81, 0xcc, 0x62
		}
	},
};

#define NUM_PASSPHRASE_TESTS \
(sizeof(passphrase_tests) / sizeof(passphrase_tests[0]))


int main(int argc, char *argv[])
{
	u8 res[512];
	int ret = 0;
	unsigned int i;

	printf("PRF-SHA1 test cases:\n");

	sha1_prf(key0, sizeof(key0), "prefix", data0, sizeof(data0) - 1,
		 res, sizeof(prf0));
	if (memcmp(res, prf0, sizeof(prf0)) == 0)
		printf("Test case 0 - OK\n");
	else {
		printf("Test case 0 - FAILED!\n");
		ret++;
	}

	sha1_prf(key1, sizeof(key1) - 1, "prefix", data1, sizeof(data1) - 1,
		 res, sizeof(prf1));
	if (memcmp(res, prf1, sizeof(prf1)) == 0)
		printf("Test case 1 - OK\n");
	else {
		printf("Test case 1 - FAILED!\n");
		ret++;
	}

	sha1_prf(key2, sizeof(key2), "prefix", data2, sizeof(data2),
		 res, sizeof(prf2));
	if (memcmp(res, prf2, sizeof(prf2)) == 0)
		printf("Test case 2 - OK\n");
	else {
		printf("Test case 2 - FAILED!\n");
		ret++;
	}

	ret += test_eap_fast();

	printf("PBKDF2-SHA1 Passphrase test cases:\n");
	for (i = 0; i < NUM_PASSPHRASE_TESTS; i++) {
		u8 psk[32];
		struct passphrase_test *test = &passphrase_tests[i];
		pbkdf2_sha1(test->passphrase,
			    test->ssid, strlen(test->ssid),
			    4096, psk, 32);
		if (memcmp(psk, test->psk, 32) == 0)
			printf("Test case %d - OK\n", i);
		else {
			printf("Test case %d - FAILED!\n", i);
			ret++;
		}
	}

	return ret;
}
