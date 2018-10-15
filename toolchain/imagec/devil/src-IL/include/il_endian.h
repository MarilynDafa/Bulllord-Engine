//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/29/2009
//
// Filename: src-IL/include/il_endian.h
//
// Description: Handles Endian-ness
//
//-----------------------------------------------------------------------------

#ifndef IL_ENDIAN_H
#define IL_ENDIAN_H

#include "il_internal.h"

#ifdef WORDS_BIGENDIAN  // This is defined by ./configure.
	#ifndef __BIG_ENDIAN__
	#define __BIG_ENDIAN__ 1
	#endif
#endif

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __BIG_ENDIAN__) \
  || (defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__))
 	#undef __LITTLE_ENDIAN__
	#define Short(s) iSwapShort(s)
	#define UShort(s) iSwapUShort(s)
	#define Int(i) iSwapInt(i)
	#define UInt(i) iSwapUInt(i)
	#define Float(f) iSwapFloat(f)
	#define Double(d) iSwapDouble(d)
 
	#define BigShort(s)  
	#define BigUShort(s)  
	#define BigInt(i)  
	#define BigUInt(i)  
	#define BigFloat(f)  
	#define BigDouble(d)  
#else
	#undef __BIG_ENDIAN__
	#undef __LITTLE_ENDIAN__  // Not sure if it's defined by any compiler...
	#define __LITTLE_ENDIAN__
	#define Short(s)  
	#define UShort(s)  
	#define Int(i)  
	#define UInt(i)  
	#define Float(f)  
	#define Double(d)  

	#define BigShort(s) iSwapShort(s)
	#define BigUShort(s) iSwapUShort(s)
	#define BigInt(i) iSwapInt(i)
	#define BigUInt(i) iSwapUInt(i)
	#define BigFloat(f) iSwapFloat(f)
	#define BigDouble(d) iSwapDouble(d)
#endif



STATIC_INLINE void iSwapUShort(ILushort *s)  {
	#ifdef USE_WIN32_ASM
		__asm {
			mov ebx, s
			mov al, [ebx+1]
			mov ah, [ebx  ]
			mov [ebx], ax
		}
	#else
	#ifdef GCC_X86_ASM
		asm("ror $8,%0"
			: "=r" (*s)
			: "0" (*s));
	#else
		*s = ((*s)>>8) | ((*s)<<8);
	#endif //GCC_X86_ASM
	#endif //USE_WIN32_ASM
}

STATIC_INLINE void iSwapShort(ILshort *s) {
	iSwapUShort((ILushort*)s);
}

STATIC_INLINE void iSwapUInt(ILuint *i) {
	#ifdef USE_WIN32_ASM
		__asm {
			mov ebx, i
			mov eax, [ebx]
			bswap eax
			mov [ebx], eax
		}
	#else
	#ifdef GCC_X86_ASM
			asm("bswap %0;"
				: "+r" (*i));
	#else
		*i = ((*i)>>24) | (((*i)>>8) & 0xff00) | (((*i)<<8) & 0xff0000) | ((*i)<<24);
	#endif //GCC_X86_ASM
	#endif //USE_WIN32_ASM
}

STATIC_INLINE void iSwapInt(ILint *i) {
	iSwapUInt((ILuint*)i);
}

STATIC_INLINE void iSwapFloat(ILfloat *f) {
	iSwapUInt((ILuint*)f);
}

STATIC_INLINE void iSwapDouble(ILdouble *d) {
	#ifdef GCC_X86_ASM
	int *t = (int*)d;
	asm("bswap %2    \n"
		"bswap %3    \n"
		"movl  %2,%1 \n"
		"movl  %3,%0 \n"
		: "=g" (t[0]), "=g" (t[1])
		: "r"  (t[0]), "r"  (t[1]));
	#else
	ILubyte t,*b = (ILubyte*)d;
	#define dswap(x,y) t=b[x];b[x]=b[y];b[y]=b[x];
	dswap(0,7);
	dswap(1,6);
	dswap(2,5);
	dswap(3,4);
	#undef dswap
	#endif
}


STATIC_INLINE ILushort GetLittleUShort() {
	ILushort s;
	iread(&s, sizeof(ILushort), 1);
#ifdef __BIG_ENDIAN__
	iSwapUShort(&s);
#endif
	return s;
}

STATIC_INLINE ILshort GetLittleShort() {
	ILshort s;
	iread(&s, sizeof(ILshort), 1);
#ifdef __BIG_ENDIAN__
	iSwapShort(&s);
#endif
	return s;
}

STATIC_INLINE ILuint GetLittleUInt() {
	ILuint i;
	iread(&i, sizeof(ILuint), 1);
#ifdef __BIG_ENDIAN__
	iSwapUInt(&i);
#endif
	return i;
}

STATIC_INLINE ILint GetLittleInt() {
	ILint i;
	iread(&i, sizeof(ILint), 1);
#ifdef __BIG_ENDIAN__
	iSwapInt(&i);
#endif
	return i;
}

STATIC_INLINE ILfloat GetLittleFloat() {
	ILfloat f;
	iread(&f, sizeof(ILfloat), 1);
#ifdef __BIG_ENDIAN__
	iSwapFloat(&f);
#endif
	return f;
}

STATIC_INLINE ILdouble GetLittleDouble() {
	ILdouble d;
	iread(&d, sizeof(ILdouble), 1);
#ifdef __BIG_ENDIAN__
	iSwapDouble(&d);
#endif
	return d;
}


STATIC_INLINE ILushort GetBigUShort() {
	ILushort s;
	iread(&s, sizeof(ILushort), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapUShort(&s);
#endif
	return s;
}


STATIC_INLINE ILshort GetBigShort() {
	ILshort s;
	iread(&s, sizeof(ILshort), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapShort(&s);
#endif
	return s;
}


STATIC_INLINE ILuint GetBigUInt() {
	ILuint i;
	iread(&i, sizeof(ILuint), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapUInt(&i);
#endif
	return i;
}


STATIC_INLINE ILint GetBigInt() {
	ILint i;
	iread(&i, sizeof(ILint), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapInt(&i);
#endif
	return i;
}


STATIC_INLINE ILfloat GetBigFloat() {
	ILfloat f;
	iread(&f, sizeof(ILfloat), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapFloat(&f);
#endif
	return f;
}


STATIC_INLINE ILdouble GetBigDouble() {
	ILdouble d;
	iread(&d, sizeof(ILdouble), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapDouble(&d);
#endif
	return d;
}

STATIC_INLINE ILubyte SaveLittleUShort(ILushort s) {
#ifdef __BIG_ENDIAN__
	iSwapUShort(&s);
#endif
	return iwrite(&s, sizeof(ILushort), 1);
}

STATIC_INLINE ILubyte SaveLittleShort(ILshort s) {
#ifdef __BIG_ENDIAN__
	iSwapShort(&s);
#endif
	return iwrite(&s, sizeof(ILshort), 1);
}


STATIC_INLINE ILubyte SaveLittleUInt(ILuint i) {
#ifdef __BIG_ENDIAN__
	iSwapUInt(&i);
#endif
	return iwrite(&i, sizeof(ILuint), 1);
}


STATIC_INLINE ILubyte SaveLittleInt(ILint i) {
#ifdef __BIG_ENDIAN__
	iSwapInt(&i);
#endif
	return iwrite(&i, sizeof(ILint), 1);
}

STATIC_INLINE ILubyte SaveLittleFloat(ILfloat f) {
#ifdef __BIG_ENDIAN__
	iSwapFloat(&f);
#endif
	return iwrite(&f, sizeof(ILfloat), 1);
}


STATIC_INLINE ILubyte SaveLittleDouble(ILdouble d) {
#ifdef __BIG_ENDIAN__
	iSwapDouble(&d);
#endif
	return iwrite(&d, sizeof(ILdouble), 1);
}


STATIC_INLINE ILubyte SaveBigUShort(ILushort s) {
#ifdef __LITTLE_ENDIAN__
	iSwapUShort(&s);
#endif
	return iwrite(&s, sizeof(ILushort), 1);
}


STATIC_INLINE ILubyte SaveBigShort(ILshort s) {
#ifdef __LITTLE_ENDIAN__
	iSwapShort(&s);
#endif
	return iwrite(&s, sizeof(ILshort), 1);
}


STATIC_INLINE ILubyte SaveBigUInt(ILuint i) {
#ifdef __LITTLE_ENDIAN__
	iSwapUInt(&i);
#endif
	return iwrite(&i, sizeof(ILuint), 1);
}


STATIC_INLINE ILubyte SaveBigInt(ILint i) {
#ifdef __LITTLE_ENDIAN__
	iSwapInt(&i);
#endif
	return iwrite(&i, sizeof(ILint), 1);
}


STATIC_INLINE ILubyte SaveBigFloat(ILfloat f) {
#ifdef __LITTLE_ENDIAN__
	iSwapFloat(&f);
#endif
	return iwrite(&f, sizeof(ILfloat), 1);
}


STATIC_INLINE ILubyte SaveBigDouble(ILdouble d) {
#ifdef __LITTLE_ENDIAN__
	iSwapDouble(&d);
#endif
	return iwrite(&d, sizeof(ILdouble), 1);
}

void		EndianSwapData(void *_Image);

#endif//ENDIAN_H
