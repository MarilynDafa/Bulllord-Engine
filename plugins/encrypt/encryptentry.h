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
#ifndef __encryptentry_h_
#define __encryptentry_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blEncryptOpenEXT(
	IN BLAnsi* _Version,
	...);

BL_API BLVoid blEncryptCloseEXT();

BL_API BLBool blEncryptRSASignEXT(
	IN BLAnsi* _Input,
	IN BLAnsi* _PrivateKey,
	IN BLU32 _Version,
	OUT BLAnsi _Output[1025]);

BL_API BLBool blEncryptRSAVerifyEXT(
	IN BLAnsi* _Input,
	IN BLAnsi* _PublicKey,
	IN BLU32 _Version);

BL_API BLBool blEncryptRSAEXT(
	IN BLAnsi* _Input,
	IN BLAnsi* _PublicKey,
	OUT BLAnsi _Output[1025]);

BL_API BLBool blDecryptRSAEXT(
	IN BLAnsi* _Input,
	IN BLAnsi* _PrivateKey,
	OUT BLAnsi _Output[1025]);
#ifdef __cplusplus
}
#endif
#endif/* __encrypt_h_ */
