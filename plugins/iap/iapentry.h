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
#ifndef __iapentry_h_
#define __iapentry_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
	/* _PURCHASE_ERRCODE */
#define BL_PE_SUCCEEDED				0
#define BL_PE_ALREADYPURCHASED		1
#define BL_PE_NOTFULFILLED			2
#define BL_PE_NOTPURCHASED	 		3
#define BL_PE_UNKNOW				4
	
	/* _PAYMENT_CHANNEL */
#define BL_PC_SELF					0xFFFF
#define BL_PC_DEFAULT				0
#define BL_PC_GOOGLE				1
#define BL_PC_APPSTORE				2
#define BL_PC_MSSTORE				3

BL_API BLVoid blIAPOpenEXT();

BL_API BLVoid blIAPCloseEXT();
    
BL_API BLVoid blRegistProductsEXT(
	IN BLEnum _Channel,
	IN BLAnsi* _Products,
	IN BLAnsi* _GoogleLicense);

BL_API BLBool blPurchaseEXT(
	IN BLAnsi* _ProductID,
	IN BLVoid(*_Subscriber)(BLEnum, BLAnsi*, BLAnsi*));

BL_API BLVoid blValidationEXT(
	IN BLVoid(*_Subscriber)(BLBool));

BL_API BLVoid blCheckUnfulfilledEXT(
	IN BLVoid(*_Subscriber)(BLAnsi*, BLAnsi*, BLAnsi*));
#ifdef __cplusplus
}
#endif
#endif/* __tmx_h_ */
