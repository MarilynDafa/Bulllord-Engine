/*
 * Copyright 2015 The Etc2Comp Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS (1)
#endif

#include "EtcConfig.h"


#include "EtcFile.h"

#include "EtcFileHeader.h"
#include "EtcColor.h"
#include "Etc.h"
#include "EtcBlock4x4EncodingBits.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

using namespace Etc;

// ----------------------------------------------------------------------------------------------------
//
File::File(const char *a_pstrFilename, Format a_fileformat, Image::Format a_imageformat,
			unsigned char *a_paucEncodingBits, unsigned int a_uiEncodingBitsBytes,
			unsigned int a_uiSourceWidth, unsigned int a_uiSourceHeight,
			unsigned int a_uiExtendedWidth, unsigned int a_uiExtendedHeight)
{
	if (a_pstrFilename == nullptr)
	{
		m_pstrFilename = const_cast<char *>("");
	}
	else
	{
		m_pstrFilename = new char[strlen(a_pstrFilename) + 1];
		strcpy(m_pstrFilename, a_pstrFilename);
	}

	m_fileformat = a_fileformat;
	if (m_fileformat == Format::INFER_FROM_FILE_EXTENSION)
	{
		// ***** TODO: add this later *****
		m_fileformat = Format::KTX;
	}

	m_imageformat = a_imageformat;

	m_paucEncodingBits = a_paucEncodingBits;
	m_uiEncodingBitsBytes = a_uiEncodingBitsBytes;
	m_uiSourceWidth = a_uiSourceWidth;
	m_uiSourceHeight = a_uiSourceHeight;
	m_uiExtendedWidth = a_uiExtendedWidth;
	m_uiExtendedHeight = a_uiExtendedHeight;

	switch (m_fileformat)
	{
	case Format::PKM:
		m_pheader = new FileHeader_Pkm(this);
		break;

	case Format::KTX:
		m_pheader = new FileHeader_Ktx(this);
		break;

	default:
		assert(0);
		break;
	}

}


// ----------------------------------------------------------------------------------------------------
//
File::File(const char *a_pstrFilename, Format a_fileformat)
{
	if (a_pstrFilename == nullptr)
	{
		return;
	}
	else
	{
		m_pstrFilename = new char[strlen(a_pstrFilename) + 1];
		strcpy(m_pstrFilename, a_pstrFilename);
	}

	m_fileformat = a_fileformat;
	if (m_fileformat == Format::INFER_FROM_FILE_EXTENSION)
	{
		// ***** TODO: add this later *****
		m_fileformat = Format::KTX;
	}

	FILE *pfile = fopen(m_pstrFilename, "rb");
	if (pfile == nullptr)
	{
		printf("ERROR: Couldn't open %s", m_pstrFilename);
		exit(1);
	}
	fseek(pfile, 0, SEEK_END);
	unsigned int fileSize = ftell(pfile);
	fseek(pfile, 0, SEEK_SET);
	size_t szResult;

	m_pheader = new FileHeader_Ktx(this);
	szResult = fread( ((FileHeader_Ktx*)m_pheader)->GetData(), 1, sizeof(FileHeader_Ktx::Data), pfile);
	assert(szResult > 0);

	if (((FileHeader_Ktx*)m_pheader)->GetData()->m_u32BytesOfKeyValueData > 0)
		fseek(pfile, ((FileHeader_Ktx*)m_pheader)->GetData()->m_u32BytesOfKeyValueData, SEEK_CUR);
	szResult = fread(&m_uiEncodingBitsBytes, 1, sizeof(unsigned int), pfile);
	assert(szResult > 0);

	m_paucEncodingBits = new unsigned char[m_uiEncodingBitsBytes];
	assert(ftell(pfile) + m_uiEncodingBitsBytes <= fileSize);
	szResult = fread(m_paucEncodingBits, 1, m_uiEncodingBitsBytes, pfile);
	assert(szResult == m_uiEncodingBitsBytes);

	uint32_t uiInternalFormat = ((FileHeader_Ktx*)m_pheader)->GetData()->m_u32GlInternalFormat;
	uint32_t uiBaseInternalFormat = ((FileHeader_Ktx*)m_pheader)->GetData()->m_u32GlBaseInternalFormat;
	
	if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC1_RGB8 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC1_RGB8)
	{
		m_imageformat = Image::Format::ETC1;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_RGB8 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_RGB8)
	{
		m_imageformat = Image::Format::RGB8;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_RGB8A1 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_RGB8A1)
	{
		m_imageformat = Image::Format::RGB8A1;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_RGBA8 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_RGBA8)
	{
		m_imageformat = Image::Format::RGBA8;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_R11 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_R11)
	{
		m_imageformat = Image::Format::R11;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_SIGNED_R11 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_R11)
	{
		m_imageformat = Image::Format::SIGNED_R11;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_RG11 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_RG11)
	{
		m_imageformat = Image::Format::RG11;
	}
	else if (uiInternalFormat == (uint32_t)FileHeader_Ktx::InternalFormat::ETC2_SIGNED_RG11 && uiBaseInternalFormat == (uint32_t)FileHeader_Ktx::BaseInternalFormat::ETC2_RG11)
	{
		m_imageformat = Image::Format::SIGNED_RG11;
	}
	else
	{
		m_imageformat = Image::Format::UNKNOWN;
	}

	m_uiSourceWidth = ((FileHeader_Ktx*)m_pheader)->GetData()->m_u32PixelWidth;
	m_uiSourceHeight = ((FileHeader_Ktx*)m_pheader)->GetData()->m_u32PixelHeight;
	m_uiExtendedWidth = Image::CalcExtendedDimension((unsigned short)m_uiSourceWidth);
	m_uiExtendedHeight = Image::CalcExtendedDimension((unsigned short)m_uiSourceHeight);

	unsigned int uiBlocks = m_uiExtendedWidth * m_uiExtendedHeight / 16;
	Block4x4EncodingBits::Format encodingbitsformat = Image::DetermineEncodingBitsFormat(m_imageformat);
	unsigned int expectedbytes = uiBlocks * Block4x4EncodingBits::GetBytesPerBlock(encodingbitsformat);
	assert(expectedbytes == m_uiEncodingBitsBytes);

	fclose(pfile);
}

File::~File()
{

	if (m_paucEncodingBits != nullptr)
	{
		delete[] m_paucEncodingBits;
		m_paucEncodingBits = nullptr;
	}
	if(m_pstrFilename != nullptr)
	{
		delete[] m_pstrFilename;
		m_pstrFilename = nullptr;
	}

	if (m_pheader != nullptr)
	{
		delete m_pheader;
		m_pheader = nullptr;
	}
}

void File::UseSingleBlock(int a_iPixelX, int a_iPixelY)
{
	if (a_iPixelX <= -1 || a_iPixelY <= -1)
		return;
	if (a_iPixelX >(int) m_uiSourceWidth)
	{
		//if we are using a ktx thats the size of a single block or less
		//then make sure we use the 4x4 image as the single block
		if (m_uiSourceWidth <= 4)
		{
			a_iPixelX = 0;
		}
		else
		{
			printf("blockAtHV: H coordinate out of range, capped to image width\n");
			a_iPixelX = m_uiSourceWidth - 1;
		}
	}
	if (a_iPixelY >(int) m_uiSourceHeight)
	{
		//if we are using a ktx thats the size of a single block or less
		//then make sure we use the 4x4 image as the single block
		if (m_uiSourceHeight <= 4)
		{
			a_iPixelY= 0;
		}
		else
		{
			printf("blockAtHV: V coordinate out of range, capped to image height\n");
			a_iPixelY = m_uiSourceHeight - 1;
		}
	}

	unsigned int origWidth = m_uiSourceWidth;
	unsigned int origHeight = m_uiSourceHeight;

	m_uiSourceWidth = 4;
	m_uiSourceHeight = 4;
	m_uiExtendedWidth = Image::CalcExtendedDimension((unsigned short)m_uiSourceWidth);
	m_uiExtendedHeight = Image::CalcExtendedDimension((unsigned short)m_uiSourceHeight);

	//block position in pixels
	// remove the bottom 2 bits to get the block coordinates 
	unsigned int iBlockPosX = (a_iPixelX & 0xFFFFFFFC);
	unsigned int iBlockPosY = (a_iPixelY & 0xFFFFFFFC);

	int numXBlocks = (origWidth / 4);
	int numYBlocks = (origHeight / 4);
	
	Block4x4EncodingBits::Format encodingbitsformat = Image::DetermineEncodingBitsFormat(m_imageformat);
	unsigned int uiEncodingBitsBytesPerBlock = Block4x4EncodingBits::GetBytesPerBlock(encodingbitsformat);

	// block location 
	//int iBlockX = (a_iPixelX % 4) == 0 ? a_iPixelX / 4.0f : (a_iPixelX / 4) + 1;
	//int iBlockY = (a_iPixelY % 4) == 0 ? a_iPixelY / 4.0f : (a_iPixelY / 4) + 1;
	//m_paucEncodingBits += ((iBlockY * numXBlocks) + iBlockX) * uiEncodingBitsBytesPerBlock;

	
	unsigned int num = numXBlocks*numYBlocks;
	unsigned int uiH = 0, uiV = 0;
	for (unsigned int uiBlock = 0; uiBlock < num; uiBlock++)
	{
		if (uiH == iBlockPosX && uiV == iBlockPosY)
		{
			break;
		}
		m_paucEncodingBits += uiEncodingBitsBytesPerBlock;
		uiH += 4;

		if (uiH >= origWidth)
		{
			uiH = 0;
			uiV += 4;
		}
	}
}
// ----------------------------------------------------------------------------------------------------
//
void File::Write()
{

	FILE *pfile = fopen(m_pstrFilename, "wb");
	if (pfile == nullptr)
	{
		printf("Error: couldn't open Etc file (%s)\n", m_pstrFilename);
		exit(1);
	}

	m_pheader->Write(pfile);
	unsigned int iResult = (int)fwrite(m_paucEncodingBits, 1, m_uiEncodingBitsBytes, pfile);
	if (iResult != m_uiEncodingBitsBytes)
	{
		printf("Error: couldn't write Etc file (%s)\n", m_pstrFilename);
		exit(1);
	}

	fclose(pfile);

}

// ----------------------------------------------------------------------------------------------------
//

