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
#include "tmxentry.h"
#include "AStar.h"
#include "../../code/headers/utils.h"
#include "../../code/headers/sprite.h"
#include "../../code/headers/streamio.h"
#include "../../code/externals/xml/ezxml.h"
#include "../../code/externals/zlib/zlib.h"
typedef struct _TMXTileset {
	BLAnsi aName[256];
	BLAnsi aSource[260];
	BLU32 nFirstGid;
	BLU32 nWidth;
	BLU32 nHeight;
}_BLTMXTilesetExt;
typedef struct _TMXLayer {
	BLAnsi aName[256];
	BLGuid* pTiles;
	BLU8* pData;
}_BLTMXLayerExt;
typedef struct _TMXObject {
	BLAnsi aName[256];
	BLS32 nX;
	BLS32 nY;
	BLU32 nWidth;
	BLU32 nHeight;
	BLS32 nType;
	BLAnsi* pProperties;
    BLBool bVisible;
}_BLTMXObjectExt;
typedef struct _TMXGroup {
	BLAnsi aName[260];
    BLBool bVisible;
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
	BLS8* pNavigation;
	BLF32* pPathXRet;
	BLF32* pPathYRet;
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
	_PrTmxMem->pNavigation = NULL;
	_PrTmxMem->pPathXRet = NULL;
	_PrTmxMem->pPathYRet = NULL;
}
BLVoid 
blTMXCloseEXT()
{
	if (_PrTmxMem->pTilesets)
		free(_PrTmxMem->pTilesets);
	if (_PrTmxMem->pNavigation)
		free(_PrTmxMem->pNavigation);
	if (_PrTmxMem->pPathXRet)
		free(_PrTmxMem->pPathXRet);
	if (_PrTmxMem->pPathYRet)
		free(_PrTmxMem->pPathYRet);
	if (_PrTmxMem->pLayers)
	{
		for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
		{
			if (_PrTmxMem->pLayers[_idx].pTiles)
				free(_PrTmxMem->pLayers[_idx].pTiles);
			if (_PrTmxMem->pLayers[_idx].pData)
				free(_PrTmxMem->pLayers[_idx].pData);
		}
		free(_PrTmxMem->pLayers);
	}
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
blTMXFileEXT(IN BLAnsi* _Filename)
{
	if (_PrTmxMem->pTilesets)
		free(_PrTmxMem->pTilesets);
	if (_PrTmxMem->pLayers)
	{
		for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
		{
			if (_PrTmxMem->pLayers[_idx].pTiles)
				free(_PrTmxMem->pLayers[_idx].pTiles);
			if (_PrTmxMem->pLayers[_idx].pData)
				free(_PrTmxMem->pLayers[_idx].pData);
		}
		free(_PrTmxMem->pLayers);
	}
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
	blSpriteClear(TRUE, FALSE);
	memset(_PrTmxMem->aOrientation, 0, sizeof(_PrTmxMem->aOrientation));
	BLGuid _stream;
    BLAnsi _dir[260] = { 0 };
    BLS32 _filelen = (BLS32)strlen(_Filename);
    BLS32 _split = 0;
    for (_split = _filelen - 1; _split > 0; --_split)
    {
        if (_Filename[_split] == '/' || _Filename[_split] == '\\')
            break;
    }
    if (_split > 0)
    {
        strncpy(_dir, _Filename, _split);
        strcat(_dir, "/");
    }
	_stream = blGenStream(_Filename);
	ezxml_t _doc = ezxml_parse_str((BLAnsi*)blStreamData(_stream), blStreamLength(_stream));
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
	BLU32 _layer = 0;
	do {
		if (!strcmp(ezxml_name(_element), "tileset"))
		{
			const BLAnsi* _tsfirstgid = ezxml_attr(_element, "firstgid");
			const BLAnsi* _tsname = ezxml_attr(_element, "name");
			_PrTmxMem->nTilesetNum++;
			_PrTmxMem->pTilesets = (_BLTMXTilesetExt*)realloc(_PrTmxMem->pTilesets, _PrTmxMem->nTilesetNum * sizeof(_BLTMXTilesetExt));
			memset(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName, 0, sizeof(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName));
			strcpy(_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].aName, _tsname);
			_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nFirstGid = (BLU32)strtol(_tsfirstgid, NULL, 10);
			if (_element->child)
			{
				BLAnsi _source[260] = { 0 };
				strcpy(_source, ezxml_attr(_element->child, "source"));
				const BLAnsi* _srcwidth = ezxml_attr(_element->child, "width");
				const BLAnsi* _srcheight = ezxml_attr(_element->child, "height");
				BLU32 _sourcelen = (BLU32)strlen(_source);
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
				_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nWidth = (BLU32)strtol(_srcwidth, NULL, 10);
				_PrTmxMem->pTilesets[_PrTmxMem->nTilesetNum - 1].nHeight = (BLU32)strtol(_srcheight, NULL, 10);
			}
		}
		else if (!strcmp(ezxml_name(_element), "layer"))
		{
			const BLAnsi* _laname = ezxml_attr(_element, "name");
			const BLAnsi* _opacity = ezxml_attr(_element, "opacity");
			BLF32 _fopacity = 1.f;
			if (_opacity)
				_fopacity = strtof(_opacity, NULL);
			_PrTmxMem->nLayerNum++;
			_PrTmxMem->pLayers = (_BLTMXLayerExt*)realloc(_PrTmxMem->pLayers, _PrTmxMem->nLayerNum * sizeof(_BLTMXLayerExt));
			memset(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName, 0, sizeof(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName));
			strcpy(_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].aName, _laname);
			if (_element->child)
			{
				const BLAnsi* _encoding = ezxml_attr(_element->child, "encoding");
				const BLAnsi* _compression = ezxml_attr(_element->child, "compression");
				if (!strcmp(_encoding, "base64") && _compression && !strcmp(_compression, "zlib"))
				{
                    BLU32 _sz;
                    BLU32 _datalen = (BLU32)strlen(_element->child->txt);
                    BLAnsi* _trimdata = (BLAnsi*)alloca(_datalen);
                    memset(_trimdata, 0, _datalen);
                    for (BLU32 _idx = 0, _jdx = 0; _idx < _datalen; ++_idx)
                    {
                        if (_element->child->txt[_idx] != '\n' && _element->child->txt[_idx] != '\t' && _element->child->txt[_idx] != ' ')
                        {
                            _trimdata[_jdx] = _element->child->txt[_idx];
                            ++_jdx;
                        }
                    }
                    const BLU8* _mem = blGenBase64Decoder(_trimdata, &_sz);
                    if(_mem)
                    {
                        uLongf _orisz = 4 * _PrTmxMem->nHeight * _PrTmxMem->nWidth;
                        Bytef* _orimem = (Bytef*)malloc(_orisz);
                        BLS32 _zipret = uncompress(_orimem, &_orisz, _mem, _sz);
                        if (_zipret == Z_OK)
                        {
							BLU32 _tidx = 0;
                            BLAnsi _buf[32] = { 0 };
							_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].pData = _orimem;
							_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].pTiles = (BLGuid*)malloc(_PrTmxMem->nHeight * _PrTmxMem->nWidth * sizeof(BLGuid));
                            for (BLU32 _y = 0; _y < _PrTmxMem->nHeight; ++_y)
                            {
                                for (BLU32 _x = 0; _x < _PrTmxMem->nWidth; ++_x)
                                {
                                    BLU32 _gid = _orimem[_tidx] | _orimem[_tidx + 1] << 8 | _orimem[_tidx + 2] << 16 | _orimem[_tidx + 3] << 24;
                                    _tidx += 4;
                                    _gid &= ~(0x80000000 | 0x40000000 | 0x20000000);
                                    memset(_buf, 0, 32);
                                    sprintf(_buf, "@#tilespritelayer_%d_%d#@", _layer, _gid);
									BLAnsi _source[260] = { 0 };
									BLF32 _w = 1.f, _h = 1.f, _first = 0.f;
									for (BLU32 _idx = 0; _idx < _PrTmxMem->nTilesetNum; ++_idx)
									{
										if (_gid >= _PrTmxMem->pTilesets[_idx].nFirstGid)
										{
											memset(_source, 0, sizeof(_source));
											if (_dir[0] == 0)
												strcpy(_source, _PrTmxMem->pTilesets[_idx].aSource);
											else
											{
												strcpy(_source, _dir);
												strcat(_source, _PrTmxMem->pTilesets[_idx].aSource);
											}
											_w = (BLF32)_PrTmxMem->pTilesets[_idx].nWidth;
											_h = (BLF32)_PrTmxMem->pTilesets[_idx].nHeight;
											_first = (BLF32)_PrTmxMem->pTilesets[_idx].nFirstGid;
										}
									}
									BLF32 _deltax = (BLF32)_PrTmxMem->nTileWidth / _w;
									BLF32 _deltay = (BLF32)_PrTmxMem->nTileHeight / _h;
									_w = (_w < 2.f) ? 1.f : _w / _PrTmxMem->nTileWidth;
									_h = (_h < 2.f) ? 1.f : _h / _PrTmxMem->nTileHeight;
									BLAnsi _buflayer[32] = { 0 };
									sprintf(_buflayer, "%d", _layer);
                                    BLGuid _id = blGenSprite(_buf, _buflayer, (BLF32)_PrTmxMem->nTileWidth, (BLF32)_PrTmxMem->nTileHeight, 0, 1, TRUE);
                                    BLF32 _ltx = (BLF32)((_gid - (BLU32)_first) % (BLU32)_w) / _w;;
                                    BLF32 _lty = (BLF32)((_gid - (BLU32)_first) / _w) / _h;
									BLF32 _rbx = _ltx + _deltax;
                                    BLF32 _rby = _lty + _deltay;
                                    blSpriteTile(_id, _source, _ltx, _lty, _rbx, _rby, (BLF32)_x * _PrTmxMem->nTileWidth, (BLF32)_y * _PrTmxMem->nTileHeight, _fopacity, TRUE);
									_PrTmxMem->pLayers[_PrTmxMem->nLayerNum - 1].pTiles[_y * _PrTmxMem->nWidth + _x] = _id;
                                }
                            }
                        }
						blDeleteBase64Decoder((BLU8*)_mem);
                    }
                    else
                        blDebugOutput("%s decode error", _Filename);
					_layer++;
				}
                else
                    blDebugOutput("%s use unsupport encoding format", _Filename);
			}
		}
		else if (!strcmp(ezxml_name(_element), "objectgroup"))
		{
			const BLAnsi* _ogname = ezxml_attr(_element, "name");
			const BLAnsi* _ogvisible = ezxml_attr(_element, "visible");
			_PrTmxMem->nGroupNum++;
			_PrTmxMem->pGroups = (_BLTMXGroupExt*)realloc(_PrTmxMem->pGroups, _PrTmxMem->nGroupNum * sizeof(_BLTMXGroupExt));
			memset(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName, 0, sizeof(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName));
			strcpy(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].aName, _ogname);
            if (_ogvisible)
                _PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].bVisible = (BLU32)strtol(_ogvisible, NULL, 10);
            else
                _PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].bVisible = FALSE;
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
                    const BLAnsi* _ovisible = ezxml_attr(_obj, "visible");
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
                    if (_ovisible)
                        _PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].bVisible = (BLU32)strtol(_ovisible, NULL, 10);
                    else
                        _PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].bVisible = TRUE;
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
							_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties = (BLAnsi*)realloc(_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].pObjects[_PrTmxMem->pGroups[_PrTmxMem->nGroupNum - 1].nObjectNum - 1].pProperties, _pos + strlen(_buf) + 1);
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
        _element = _element->ordered;
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
	memset(_Orientation, 0, 32 * sizeof(BLAnsi));
	strcpy(_Orientation, _PrTmxMem->aOrientation);
	*_Width = _PrTmxMem->nWidth;
	*_Height = _PrTmxMem->nHeight;
	*_TileWidth = _PrTmxMem->nTileWidth;
	*_TileHeight = _PrTmxMem->nTileHeight;
}
BLVoid 
blTMXObjectGroupQueryEXT(IN BLAnsi* _GroupName, OUT BLU32* _ObjectNum, OUT BLBool* _Visible)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nGroupNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pGroups[_idx].aName, _GroupName))
		{
			*_ObjectNum = _PrTmxMem->pGroups[_idx].nObjectNum;
			*_Visible = _PrTmxMem->pGroups[_idx].bVisible;
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
			memset(_Name, 0, 256 * sizeof(BLAnsi));
			strcpy(_Name, _PrTmxMem->pGroups[_idx].pObjects[_Index].aName);
			*_XPos = _PrTmxMem->pGroups[_idx].pObjects[_Index].nX * _PrTmxMem->nTileWidth;
			*_YPos = _PrTmxMem->pGroups[_idx].pObjects[_Index].nY * _PrTmxMem->nTileHeight;
			*_Width = _PrTmxMem->pGroups[_idx].pObjects[_Index].nWidth;
			*_Height = _PrTmxMem->pGroups[_idx].pObjects[_Index].nHeight;
			*_Properties = _PrTmxMem->pGroups[_idx].pObjects[_Index].pProperties;
			return;
		}
	}
}
BLVoid 
blTMXLayerVisibilityEXT(IN BLAnsi* _Layer, IN BLBool _Show)
{
	if (!_PrTmxMem)
		return;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pLayers[_idx].aName, _Layer))
		{
			for (BLU32 _jdx = 0; _jdx < _PrTmxMem->nHeight * _PrTmxMem->nWidth; ++_jdx)
				blSpriteTile(_PrTmxMem->pLayers[_idx].pTiles[_jdx], NULL, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, _Show);
		}
	}
}
BLBool 
blTMXLayerTileAtEXT(IN BLAnsi* _Layer, IN BLS32 _XPos, IN BLS32 _YPos)
{
	if (!_PrTmxMem)
		return FALSE;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pLayers[_idx].aName, _Layer))
		{
			BLU32* _gids = (BLU32*)_PrTmxMem->pLayers[_idx].pData;
			return _gids[_YPos / _PrTmxMem->nTileHeight * _PrTmxMem->nWidth + _XPos / _PrTmxMem->nTileWidth];
		}
	}
	return FALSE;
}
BLBool 
blTMXLayerAsNavigationEXT(IN BLAnsi* _Layer, IN BLBool _Barrier)
{
	if (!_PrTmxMem)
		return FALSE;
	_BLTMXLayerExt* _layer = NULL;
	for (BLU32 _idx = 0; _idx < _PrTmxMem->nLayerNum; ++_idx)
	{
		if (!strcmp(_PrTmxMem->pLayers[_idx].aName, _Layer))
		{
			_layer = &_PrTmxMem->pLayers[_idx];
			break;
		}
	}
	if (!_layer)
		return FALSE;
	if (_PrTmxMem->pNavigation)
		free(_PrTmxMem->pNavigation);
	_PrTmxMem->pNavigation = (BLS8*)malloc(_PrTmxMem->nWidth * _PrTmxMem->nHeight * sizeof(BLS8));
	BLU32* _gids = (BLU32*)_layer->pData;
	for (BLU32 _jdx = 0; _jdx < _PrTmxMem->nHeight; ++_jdx)
	{
		for (BLU32 _idx = 0; _idx < _PrTmxMem->nWidth; ++_idx)
		{
			if (_Barrier)
				_PrTmxMem->pNavigation[_idx + _jdx * _PrTmxMem->nWidth] = !_gids[_jdx * _PrTmxMem->nWidth + _idx];
			else
				_PrTmxMem->pNavigation[_idx + _jdx * _PrTmxMem->nWidth] = _gids[_jdx * _PrTmxMem->nWidth + _idx];
		}
	}
	return TRUE;
}
BLBool 
blTMXPathFindEXT(IN BLS32 _StartX, IN BLS32 _StartY, IN BLS32 _EndX, IN BLS32 _EndY, IN BLBool _Strict, OUT BLF32** _XPath, OUT BLF32** _YPath, OUT BLU32* _PathNum)
{
	if (!_PrTmxMem)
		return FALSE;
	if (!_PrTmxMem->pNavigation)
		return FALSE;
	BLS32 _begin = astar_getIndexByWidth(_PrTmxMem->nWidth, _StartX / _PrTmxMem->nTileWidth, _StartY / _PrTmxMem->nTileHeight);
	BLS32 _end = astar_getIndexByWidth(_PrTmxMem->nWidth, _EndX / _PrTmxMem->nTileWidth, _EndY / _PrTmxMem->nTileHeight);
	if (!_Strict && !_PrTmxMem->pNavigation[_EndX / _PrTmxMem->nTileWidth + _EndY / _PrTmxMem->nTileHeight * _PrTmxMem->nWidth])
	{
		BLS32 _numx = (BLS32)abs(_EndX - _StartX) / _PrTmxMem->nTileWidth;
		BLS32 _numy = (BLS32)abs(_EndY - _StartY) / _PrTmxMem->nTileHeight;
		BLS32 _num = (BLS32)((_numx) > (_numy) ? (_numx) : (_numy));
		if (_num)
		{
			BLF32 _deltax = (BLF32)(_EndX - _StartX) / _num;
			BLF32 _deltay = (BLF32)(_EndY - _StartY) / _num;
			for (BLS32 _idx = 0; _idx < _num - 1; ++_idx)
			{
				BLS32 _tmpx = (BLS32)(_EndX - _idx * _deltax);
				BLS32 _tmpy = (BLS32)(_EndY - _idx * _deltay);
				if (_PrTmxMem->pNavigation[_tmpx / _PrTmxMem->nTileWidth + _tmpy / _PrTmxMem->nTileHeight * _PrTmxMem->nWidth])
				{
					_end = astar_getIndexByWidth(_PrTmxMem->nWidth, _tmpx / _PrTmxMem->nTileWidth, _tmpy / _PrTmxMem->nTileHeight);
					break;
				}
			}
		}
	}
	BLS32 _retlen = 0;
	BLS32* _ret = astar_compute((const BLAnsi*)_PrTmxMem->pNavigation, &_retlen, _PrTmxMem->nWidth, _PrTmxMem->nHeight, _begin, _end);
	if (!_ret || _retlen == 0)
		return FALSE;
	_PrTmxMem->pPathXRet = (BLF32*)realloc(_PrTmxMem->pPathXRet, (_retlen + 1) * sizeof(BLF32));
	_PrTmxMem->pPathYRet = (BLF32*)realloc(_PrTmxMem->pPathYRet, (_retlen + 1) * sizeof(BLF32));
	for (BLS32 _idx = _retlen - 1, _jdx = 0; _idx >= 0; --_idx, ++_jdx)
	{
		BLS32 _x, _y;
		astar_getCoordByWidth(_PrTmxMem->nWidth, _ret[_idx], &_x, &_y);
		_PrTmxMem->pPathXRet[_jdx] = (BLF32)_x * _PrTmxMem->nTileWidth;
		_PrTmxMem->pPathYRet[_jdx] = (BLF32)_y * _PrTmxMem->nTileHeight;
	}
	_PrTmxMem->pPathXRet[_retlen] = (BLF32)_EndX;
	_PrTmxMem->pPathYRet[_retlen] = (BLF32)_EndY;
	*_XPath = _PrTmxMem->pPathXRet;
	*_YPath = _PrTmxMem->pPathYRet;
	*_PathNum = _retlen;
	return TRUE;
}
