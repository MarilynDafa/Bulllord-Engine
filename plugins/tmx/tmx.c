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
#include "tmx.h"
#include "../../code/headers/system.h"
#include "../../code/headers/utils.h"
#include "../../code/headers/streamio.h"
#include "../../code/externals/xml/ezxml.h"
typedef struct _TMXTileset {
	BLAnsi aName[256];
	BLAnsi aSource[260];
	BLU32 nFirstGid;
	BLU32 nTileWidth;
	BLU32 nTileHeight;
	BLU32 nWidth;
	BLU32 nHeight;
}_BLTMXTilesetExt;
typedef struct _TMXLayer {
	BLAnsi aName[256];
	BLU32 nWidth;
	BLU32 nHeight;
}_BLTMXLayerExt;
typedef struct _TMXObject {
	BLAnsi aName[256];
	BLS32 nX;
	BLS32 nY;
	BLU32 nWidth;
	BLU32 nHeight;
	BLS32 nType;
	BLAnsi* pProperties;
}_BLTMXObjectExt;
typedef struct _TMXGroup {
	BLAnsi aName[260];
	BLU32 nWidth;
	BLU32 nHeight;
	_BLTMXObjectExt* pObjects;
	BLU32 nObjectNum;
}_BLTMXGroupExt;
typedef struct _TMXMember {
	BLAnsi aOrientation[32];
	BLU32 nWidth;
	BLU32 nHeight;
	BLU32 nTileWidth;
	BLU32 nTileHeight;
	_BLTMXTilesetExt* pTilesets;
	BLU32 nTilesetNum;
	_BLTMXLayerExt* pLayers;
	BLU32 nLayerNum;
	_BLTMXGroupExt* pGroups;
	BLU32 nGroupNum;
}_BLTMXMemberExt;
static _BLTMXMemberExt* _PrTmxMem = NULL;
BLVoid 
blTMXOpenEXT()
{
	_PrTmxMem = (_BLTMXMemberExt*)malloc(sizeof(_BLTMXMemberExt));
	_PrTmxMem->pTilesets = NULL;
	_PrTmxMem->nTilesetNum = 0;
	_PrTmxMem->pLayers = NULL;
	_PrTmxMem->nLayerNum = 0;
	_PrTmxMem->pGroups = NULL;
	_PrTmxMem->nGroupNum = 0;
}
BLVoid 
blTMXCloseEXT()
{
	if (_PrTmxMem->pTilesets)
		free(_PrTmxMem->pTilesets);
	if (_PrTmxMem->pLayers)
		free(_PrTmxMem->pLayers);
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nGroupNum; ++_idx)
	{
		if (_PrTmxMem->pGroups[_idx].pObjects)
		{
			for (BLU32 _jdx = 0; _jdx < _PrTmxMem->pGroups[_idx].nObjectNum; ++_jdx)
			{
				if (_PrTmxMem->pGroups[_idx].pObjects[_jdx].pProperties)
					free(_PrTmxMem->pGroups[_idx].pObjects[_jdx].pProperties);
			}
			free(_PrTmxMem->pGroups[_idx].pObjects);
		}
	}
	free(_PrTmxMem->pGroups);
	free(_PrTmxMem);
}
BLBool 
blTMXFileEXT(IN BLAnsi* _Filename, IN BLAnsi* _Archive)
{
	if (_PrTmxMem->pTilesets)
		free(_PrTmxMem->pTilesets);
	if (_PrTmxMem->pLayers)
		free(_PrTmxMem->pLayers);
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nGroupNum; ++_idx)
	{
		if (_PrTmxMem->pGroups[_idx].pObjects)
		{
			for (BLU32 _jdx = 0; _jdx < _PrTmxMem->pGroups[_idx].nObjectNum; ++_jdx)
			{
				if (_PrTmxMem->pGroups[_idx].pObjects[_jdx].pProperties)
					free(_PrTmxMem->pGroups[_idx].pObjects[_jdx].pProperties);
			}
			free(_PrTmxMem->pGroups[_idx].pObjects);
		}
	}
	if (_PrTmxMem->pGroups)
		free(_PrTmxMem->pGroups);
	_PrTmxMem->pTilesets = NULL;
	_PrTmxMem->nTilesetNum = 0;
	_PrTmxMem->pLayers = NULL;
	_PrTmxMem->nLayerNum = 0;
	_PrTmxMem->pGroups = NULL;
	_PrTmxMem->nGroupNum = 0;
	memset(_PrTmxMem->aOrientation, 0, sizeof(_PrTmxMem->aOrientation));
	BLGuid _stream;
	if (_Archive)
		_stream = blGenStream(_Filename, _Archive);
	else
	{
		BLAnsi _tmpname[260];
		strcpy(_tmpname, _Filename);
		for (BLU32 _i = 0; _i < strlen(_tmpname); ++_i)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			if (_tmpname[_i] == '/')
				_tmpname[_i] = '\\';
#else
			if (_tmpname[_i] == '\\')
				_tmpname[_i] = '/';
#endif
		}
		BLAnsi _path[260] = { 0 };
		strcpy(_path, blWorkingDir(TRUE));
		strcat(_path, _tmpname);
		_stream = blGenStream(_path, NULL);
		if (INVALID_GUID == _stream)
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			_stream = blGenStream(_path, NULL);
		}
		if (INVALID_GUID == _stream)
			return FALSE;
	}
	ezxml_t _doc = ezxml_parse_str(blStreamData(_stream), blStreamLength(_stream));
	ezxml_t _element = _doc;
	const BLAnsi* _orientation = ezxml_attr(_element, "orientation");
	const BLAnsi* _width = ezxml_attr(_element, "width");
	const BLAnsi* _height = ezxml_attr(_element, "height");
	const BLAnsi* _tilewidth = ezxml_attr(_element, "tilewidth");
	const BLAnsi* _tileheight = ezxml_attr(_element, "tileheight");
	strcpy(_PrTmxMem->aOrientation, _orientation);
	_PrTmxMem->nWidth = (BLU32)strtol(_width, NULL, 10);
	_PrTmxMem->nHeight = (BLU32)strtol(_height, NULL, 10);
	_PrTmxMem->nTileWidth = (BLU32)strtol(_tilewidth, NULL, 10);
	_PrTmxMem->nTileHeight = (BLU32)strtol(_tileheight, NULL, 10);
	_element = _doc->child;
	do {
		if (!strcmp(ezxml_name(_element), "tileset"))
		{
			const BLAnsi* _tsfirstgid = ezxml_attr(_element, "firstgid");
			const BLAnsi* _tsname = ezxml_attr(_element, "name");
			const BLAnsi* _tstilewidth = ezxml_attr(_element, "tilewidth");
			const BLAnsi* _tstileheight = ezxml_attr(_element, "tileheight");
			_PrTmxMem->nTilesetNum++;
			_PrTmxMem->pTilesets = (_BLTMXTilesetExt*)realloc(_PrTmxMem->pTilesets, _PrTmxMem->nTilesetNum * sizeof(_BLTMXTilesetExt));
			memset(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName, 0, sizeof(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName));
			strcpy(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName, _tsname);
			_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nFirstGid = (BLU32)strtol(_tsfirstgid, NULL, 10);
			_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nTileWidth = (BLU32)strtol(_tstilewidth, NULL, 10);
			_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nTileHeight = (BLU32)strtol(_tstileheight, NULL, 10);
			if (_element->child)
			{
				BLAnsi _source[260] = { 0 };
				strcpy(_source, ezxml_attr(_element->child, "source"));
				const BLAnsi* _tsw = ezxml_attr(_element->child, "width");
				const BLAnsi* _tsh = ezxml_attr(_element->child, "height");
				BLU32 _sourcelen = strlen(_source);
				for (BLS32 _idx = (BLS32)_sourcelen - 1; _idx >= 0; --_idx)
				{
					if (_source[_idx] == '.')
						break;
					else
						_source[_idx] = 0;
				}
				strcat(_source, "bmg");
				memset(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aSource, 0, sizeof(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aSource));
				strcpy(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aSource, _source);
				_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nWidth = (BLU32)strtol(_tsw, NULL, 10);
				_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nHeight = (BLU32)strtol(_tsh, NULL, 10);
			}
		}
		else if (!strcmp(ezxml_name(_element), "layer"))
		{
			const BLAnsi* _laname = ezxml_attr(_element, "name");
			const BLAnsi* _lawidth = ezxml_attr(_element, "width");
			const BLAnsi* _laheight = ezxml_attr(_element, "height");
			_PrTmxMem->nLayerNum++;
			_PrTmxMem->pLayers = (_BLTMXLayerExt*)realloc(_PrTmxMem->pLayers, _PrTmxMem->nLayerNum * sizeof(_BLTMXLayerExt));
			memset(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName, 0, sizeof(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName));
			strcpy(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName, _laname);
			_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].nWidth = (BLU32)strtol(_lawidth, NULL, 10);
			_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].nHeight = (BLU32)strtol(_laheight, NULL, 10);
			if (_element->child)
			{
				const BLAnsi* _encoding = ezxml_attr(_element->child, "encoding");
				const BLAnsi* _compression = ezxml_attr(_element->child, "compression");
				if (!strcmp(_encoding, "base64") && !strcmp(_compression, "zlib"))
				{
					const BLAnsi* _data = _element->child->txt;
				}
				else
				{
					blDebugOutput("Bulllord engine can only load base64 zlib encode file>%s", _Filename);
				}
			}
		}
		else if (!strcmp(ezxml_name(_element), "objectgroup"))
		{
			const BLAnsi* _ogname = ezxml_attr(_element, "name");
			const BLAnsi* _ogwidth = ezxml_attr(_element, "width");
			const BLAnsi* _ogheight = ezxml_attr(_element, "height");
			_PrTmxMem->nGroupNum++;
			_PrTmxMem->pGroups = (_BLTMXGroupExt*)realloc(_PrTmxMem->pGroups, _PrTmxMem->nGroupNum * sizeof(_BLTMXGroupExt));
			memset(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName, 0, sizeof(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName));
			strcpy(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName, _ogname);
			_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nWidth = (BLU32)strtol(_ogwidth, NULL, 10);
			_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nHeight = (BLU32)strtol(_ogheight, NULL, 10);
			_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum = 0;
			_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects = NULL;
			ezxml_t _obj = _element->child;
			if (_obj)
			{
				do {
					const BLAnsi* _oname = ezxml_attr(_obj, "name");
					const BLAnsi* _otype = ezxml_attr(_obj, "type");
					const BLAnsi* _ox = ezxml_attr(_obj, "x");
					const BLAnsi* _oy = ezxml_attr(_obj, "y");
					const BLAnsi* _owidth = ezxml_attr(_obj, "width");
					const BLAnsi* _oheight = ezxml_attr(_obj, "height");
					_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum++;
					_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects = (_BLTMXObjectExt*)realloc(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects, _PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum * sizeof(_BLTMXObjectExt));
					memset(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].aName, 0, sizeof(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].aName));
					if (_oname)
						strcpy(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].aName, _oname);
					if (_otype)
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].nType = (BLS32)strtol(_otype, NULL, 10);
					if (_ox)
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].nX = (BLU32)strtol(_ox, NULL, 10);
					if (_oy)
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].nY = (BLU32)strtol(_oy, NULL, 10);
					if (_owidth)
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].nWidth = (BLU32)strtol(_owidth, NULL, 10);
					if (_oheight)
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].nHeight = (BLU32)strtol(_oheight, NULL, 10);
					_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties = NULL;
					if (_obj->child)
					{
						BLU32 _pos = 0;
						ezxml_t _pro = _obj->child->child;
						do {
							const BLAnsi* _opname = ezxml_attr(_pro, "name");
							const BLAnsi* _opvalue = ezxml_attr(_pro, "value");
							BLAnsi _buf[512] = { 0 };
							sprintf(_buf, "%s:%s", _opname, _opvalue);
							_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties = (BLAnsi*)realloc(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties, strlen(_buf) + 1);
							strcpy(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties + _pos, _buf);
							strcat(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties, ",");
							_pos += strlen(_buf) + 1;
							_pro = _pro->next;
						} while (_pro);
						_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties[_pos - 1] = 0;
					}
					_obj = _obj->next;
				} while (_obj);
			}
		}
		if (_element->sibling)
			_element = _element->sibling;
		else
		{
			do {
				_element = _element->parent;
				if (!_element)
					break;
			} while (!_element->next);
			_element = _element ? _element->next : NULL;
		}
	} while (_element);
	ezxml_free(_doc);
	blDeleteStream(_stream);
	return TRUE;
}
BLVoid 
blTMXQueryEXT(OUT BLAnsi _Orientation[32], OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _TileWidth, OUT BLU32* _TileHeight)
{
	if (!_PrTmxMem)
		return;
	memset(_Orientation, 0, sizeof(_Orientation));
	strcpy(_Orientation, _PrTmxMem->aOrientation);
	*_Width = _PrTmxMem->nWidth;
	*_Height = _PrTmxMem->nHeight;
	*_TileWidth = _PrTmxMem->nTileWidth;
	*_TileHeight = _PrTmxMem->nTileHeight;
}
BLVoid 
blTMXTilesetQueryEXT(IN BLAnsi* _Name, OUT BLU32* _FirstGid, OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _TileWidth, OUT BLU32* _TileHeight)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nTilesetNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pTilesets[_idx].aName, _Name))
		{
			*_Width = _PrTmxMem->pTilesets[_idx].nWidth;
			*_Height = _PrTmxMem->pTilesets[_idx].nHeight;
			*_TileWidth = _PrTmxMem->pTilesets[_idx].nTileWidth;
			*_TileHeight = _PrTmxMem->pTilesets[_idx].nTileHeight;
			return;
		}
	}
}
BLVoid 
blTMXLayerQueryEXT(IN BLAnsi* _Name, OUT BLU32* _Width, OUT BLU32* _Height)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pLayers[_idx].aName, _Name))
		{
			*_Width = _PrTmxMem->pLayers[_idx].nWidth;
			*_Height = _PrTmxMem->pLayers[_idx].nHeight;
			return;
		}
	}
}
BLVoid 
blTMXObjectGroupQueryEXT(IN BLAnsi* _GroupName, OUT BLU32* _ObjectNum, OUT BLU32* _Width, OUT BLU32* _Height)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nGroupNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pGroups[_idx].aName, _GroupName))
		{
			*_ObjectNum = _PrTmxMem->pGroups[_idx].nObjectNum;
			*_Width = _PrTmxMem->pGroups[_idx].nWidth;
			*_Height = _PrTmxMem->pGroups[_idx].nHeight;
			return;
		}
	}
}
BLVoid 
blTMXObjectQueryEXT(IN BLAnsi* _GroupName, IN BLU32 _Index, OUT BLAnsi _Name[256], OUT BLS32* _XPos, OUT BLS32* _YPos, OUT BLU32* _Width, OUT BLU32* _Height, OUT BLAnsi** _Properties)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nGroupNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pGroups[_idx].aName, _GroupName))
		{
			if (_Index >= _PrTmxMem->pGroups[_idx].nObjectNum)
				return;
			memset(_Name, 0, sizeof(_Name));
			strcpy(_Name, _PrTmxMem->pGroups[_idx].pObjects[_Index].aName);
			*_XPos = _PrTmxMem->pGroups[_idx].pObjects[_Index].nX;
			*_YPos = _PrTmxMem->pGroups[_idx].pObjects[_Index].nY;
			*_Width = _PrTmxMem->pGroups[_idx].pObjects[_Index].nWidth;
			*_Height = _PrTmxMem->pGroups[_idx].pObjects[_Index].nHeight;
			*_Properties = _PrTmxMem->pGroups[_idx].pObjects[_Index].pProperties;
			return;
		}
	}
}