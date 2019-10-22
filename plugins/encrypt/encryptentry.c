/*
Bulllord Game Engine
Copyright (C) 2010-2017 Trix

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "encryptentry.h"
#include "util.h"
#include "mbedtls/rsa.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
BLVoid
blEncryptOpenEXT(IN BLAnsi* _Version, ...)
{
}
BLVoid
blIAPCloseEXT()
{
}
BLBool
blRSASignEXT(IN BLAnsi* _Input, IN BLAnsi* _PrivateKey, IN BLU32 _Version, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_key(&_ctx, (const BLU8*)_PrivateKey, strlen(_PrivateKey) + 1, 0, 0))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_sign";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _hash[32] = { 0 };
	mbedtls_md_type_t _type = _Version ? MBEDTLS_MD_SHA256 : MBEDTLS_MD_SHA1;
	if (mbedtls_md(mbedtls_md_info_from_type(_type), (const BLU8*)_Input, strlen(_Input), _hash))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _buf[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_sign(&_ctx, _type, _hash, 0, _buf, &_olen, mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	memset(_Output, 0, 1025 * sizeof(BLAnsi));
	const BLAnsi* _base64 = blGenBase64Encoder(_buf, (BLU32)_olen);
	strcpy(_Output, _base64);
	blDeleteBase64Encoder((BLAnsi*)_base64);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool
blRSAVerifyEXT(IN BLAnsi* _Input, IN BLAnsi* _PublicKey, IN BLU32 _Version)
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_public_key(&_ctx, (const BLU8*)_PublicKey, strlen(_PublicKey) + 1))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_verify";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _hash[32] = { 0 };
	mbedtls_md_type_t _type = _Version ? MBEDTLS_MD_SHA256 : MBEDTLS_MD_SHA1;
	if (mbedtls_md(mbedtls_md_info_from_type(_type), (const BLU8*)_Input, strlen(_Input), _hash))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU32 _bufsz;
	const BLU8* _buf = blGenBase64Decoder(_Input, &_bufsz);
	if (mbedtls_pk_verify(&_ctx, _type, _hash, 0, _buf, _bufsz))
	{
		blDeleteBase64Decoder((BLU8*)_buf);
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	blDeleteBase64Decoder((BLU8*)_buf);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool
blRSAEncryptEXT(IN BLAnsi* _Input, IN BLAnsi* _PublicKey, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_public_key(&_ctx, (const BLU8*)_PublicKey, strlen(_PublicKey) + 1))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_encrypt";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _buf[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_encrypt(&_ctx, (const BLU8*)_Input, strlen(_Input), _buf, &_olen, sizeof(_buf), mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	memset(_Output, 0, sizeof(BLAnsi) * 1025);
	const BLAnsi* _base64 = blGenBase64Encoder(_buf, (BLU32)_olen);
	strcpy(_Output, _base64);
	blDeleteBase64Encoder((BLAnsi*)_base64);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool
blRSADecryptEXT(IN BLAnsi* _Input, IN BLAnsi* _PrivateKey, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_key(&_ctx, (const BLU8*)_PrivateKey, strlen(_PrivateKey) + 1, 0, 0))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_decrypt";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU32 _bufsz;
	const BLU8* _buf = blGenBase64Decoder(_Input, &_bufsz);
	BLAnsi _result[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_decrypt(&_ctx, _buf, _bufsz, (BLU8*)_result, &_olen, sizeof(_result), mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		blDeleteBase64Decoder((BLU8*)_buf);
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	blDeleteBase64Decoder((BLU8*)_buf);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	memset(_Output, 0, 1025 * sizeof(BLAnsi));
	memcpy(_Output, _result, _olen);
	return TRUE;
}