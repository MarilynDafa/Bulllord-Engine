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
#ifndef __streamio_h_
#define __streamio_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif

BL_API BLGuid blGenStream(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Archive);

BL_API BLBool blDeleteStream(
	IN BLGuid _ID);

BL_API BLVoid* blStreamData(
	IN BLGuid _ID);

BL_API BLU32 blStreamTell(
	IN BLGuid _ID);

BL_API BLU32 blStreamLength(
	IN BLGuid _ID);

BL_API BLBool blStreamEos(
	IN BLGuid _ID);

BL_API BLU32 blStreamRead(
	IN BLGuid _ID, 
	IN BLU32 _Count,
	OUT BLVoid* _Buf);

BL_API BLU32 blStreamWrite(
	IN BLGuid _ID, 
	IN BLU32 _Count,
	IN BLVoid* _Buf);

BL_API BLVoid blStreamSeek(
	IN BLGuid _ID,
	IN BLU32 _Pos);

BL_API BLBool blExistFile(
	IN BLAnsi* _Filename);
    
BL_API BLBool blWriteFile(
	IN BLAnsi* _Filename,
	IN BLU32 _Count,
	IN BLU8* _Data);

BL_API BLBool blDeleteFile(
	IN BLAnsi* _Filename);
    
BL_API BLBool blRegisterArchive(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Archive);
    
BL_API BLBool blPatchArchive(
	IN BLAnsi* _Filename, 
	IN BLAnsi* _Archive);
    
BL_API BLU32 blQueryArchiveVer(
	IN BLAnsi* _Archive);
    
BL_API BLBool blGenSqlConnection(
	IN BLAnsi* _Dbname,
	IN BLAnsi* _Dbpwd,
	IN BLBool _Inmem);
    
BL_API BLVoid blDeleteSqlConnection();
    
BL_API BLBool blSqlCmdBegin(
	IN BLAnsi* _Cmd);
    
BL_API BLVoid blSqlCmdEnd();
    
BL_API BLS64 blSqlValueAsInteger(
	IN BLAnsi* _Name);
    
BL_API BLF64 blSqlValueAsReal(
	IN BLAnsi* _Name);
    
BL_API const BLUtf8* blSqlValueAsString(
	IN BLAnsi* _Name);

BL_API const BLVoid* blSqlValueAsBinary(
	IN BLAnsi* _Name);
    
BL_API BLBool blSqlRetrieveRow();

#ifdef __cplusplus
}
#endif

#endif/* __streamio_h_ */
