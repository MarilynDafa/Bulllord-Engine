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
#include "util.h"
#include "sprite.h"
#include "streamio.h"
#include "xml/ezxml.h"
#include "miniz/miniz.h"
#include "tmxparser.h"
typedef struct _TMXMember {
	TMXMapInfo* pMapInfo;
	BLS8* pNavigation;
	BLF32* pPathXRet;
	BLF32* pPathYRet;
}_BLTMXMemberExt;
static _BLTMXMemberExt* _PrTmxMem = NULL;
BLVoid 
blTMXOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrTmxMem = (_BLTMXMemberExt*)malloc(sizeof(_BLTMXMemberExt));
	_PrTmxMem->pMapInfo = NULL;
	_PrTmxMem->pNavigation = NULL;
	_PrTmxMem->pPathXRet = NULL;
	_PrTmxMem->pPathYRet = NULL;
}
BLVoid 
blTMXCloseEXT()
{
	if (_PrTmxMem->pMapInfo)
		destroyMapInfo(_PrTmxMem->pMapInfo);
	if (_PrTmxMem->pNavigation)
		free(_PrTmxMem->pNavigation);
	if (_PrTmxMem->pPathXRet)
		free(_PrTmxMem->pPathXRet);
	if (_PrTmxMem->pPathYRet)
		free(_PrTmxMem->pPathYRet);
	free(_PrTmxMem);
}
BLBool 
blTMXFileEXT(IN BLAnsi* _Filename)
{
	if (_PrTmxMem->pMapInfo)
	{
		destroyMapInfo(_PrTmxMem->pMapInfo);
	}
	_PrTmxMem->pMapInfo = createMapInfo(_Filename);
	return TRUE;
}
BLVoid 
blTMXMapQueryEXT(OUT BLAnsi _Orientation[32], OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _TileWidth, OUT BLU32* _TileHeight)
{
	if (!_PrTmxMem)
		return;
	if (!_PrTmxMem->pMapInfo)
		return;
	memset(_Orientation, 0, 32);
	switch (_PrTmxMem->pMapInfo->_orientation)
	{
	case TMXOrientationOrtho:
		strcpy(_Orientation, "orthogonal");
		break;
	case TMXOrientationHex:
		strcpy(_Orientation, "hexagonal");
		break;
	case TMXOrientationIso:
		strcpy(_Orientation, "isometric");
		break;
	case TMXOrientationStaggered:
		strcpy(_Orientation, "staggered");
		break;
	default:
		strcpy(_Orientation, "orthogonal");
		break;
	}
	*_Width = (BLU32)_PrTmxMem->pMapInfo->_mapSize.width;
	*_Height = (BLU32)_PrTmxMem->pMapInfo->_mapSize.height;
	*_TileWidth = (BLU32)_PrTmxMem->pMapInfo->_tileSize.width;
	*_TileHeight = (BLU32)_PrTmxMem->pMapInfo->_tileSize.height;
}
BLVoid 
blTMXGroupQueryEXT(IN BLAnsi* _GroupName, OUT BLU32* _ObjectNum)
{
	if (!_PrTmxMem)
		return;
	if (!_PrTmxMem->pMapInfo)
		return;
	FOREACH_ARRAY(TMXObjectGroup*, _iter, _PrTmxMem->pMapInfo->_objectGroups)
	{
		if (!strcmp(_GroupName, _iter->_groupName))
		{
			*_ObjectNum = _iter->_objects->nSize;
			return;
		}
	}	
	*_ObjectNum = 0;
}
BLVoid 
blTMXObjectQueryEXT(IN BLAnsi* _GroupName, IN BLU32 _Index, OUT BLAnsi _Name[256], OUT BLS32* _ID, OUT BLS32* _GID, OUT BLAnsi _Type[64], OUT BLS32* _XPos, OUT BLS32* _YPos, OUT BLU32* _Width, OUT BLU32* _Height, OUT BLF32* _Rotate, OUT BLF32** _Geometry)
{
	if (!_PrTmxMem)
		return;
	if (!_PrTmxMem->pMapInfo)
		return;
	FOREACH_ARRAY(TMXObjectGroup*, _iter, _PrTmxMem->pMapInfo->_objectGroups)
	{
		if (!strcmp(_GroupName, _iter->_groupName))
		{
			Value* _val = blArrayElement(_iter->_objects, _Index);
			memset(_Name, 0, 256);
			Value* _name = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"name"));
			strcpy(_Name, (const BLAnsi*)_name->_field.strVal);
			Value* _id = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"id"));
			*_ID = _id->_field.intVal;
			Value* _gid = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"gid"));
			*_GID = _gid->_field.intVal;
			memset(_Type, 0, 64);
			Value* _type = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"type"));
			strcpy(_Type, (const BLAnsi*)_type->_field.strVal);
			Value* _x = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"x"));
			*_XPos = (BLS32)_x->_field.floatVal;
			Value* _y = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"y"));
			*_YPos = (BLS32)_y->_field.floatVal;
			Value* _width = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"width"));
			*_Width = (BLS32)_width->_field.floatVal;
			Value* _height = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"height"));
			*_Height = (BLS32)_height->_field.floatVal;
			Value* _rotation = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"rotation"));
			*_Rotate = _rotation->_field.floatVal;
			Value* _points = blDictElement(_val->_field.mapVal, blHashString((const BLUtf8*)"points"));
			*_Geometry = _rotation->_field.arrayVal;
			return;
		}
	}
}
BLVoid 
blTMXPropertyQueryEXT(IN BLAnsi* _GroupName, IN BLAnsi* _ObjectName, IN BLAnsi* _Name, OUT BLAnsi _Value[64])
{
	if (!_PrTmxMem)
		return;
	if (!_PrTmxMem->pMapInfo)
		return;
	if (!_Name)
		return;
	if (!_GroupName && !_ObjectName)
	{
		Value* _val = blDictElement(_PrTmxMem->pMapInfo->_properties, blHashString((const BLUtf8*)_Name));
		if (_val)
		{
			memset(_Value, 0, 64);
			strcpy(_Value, _val->_field.strVal);
		}
	}
	else if (_GroupName && !_ObjectName)
	{
		FOREACH_ARRAY(TMXObjectGroup*, _iter, _PrTmxMem->pMapInfo->_objectGroups)
		{
			if (!strcmp(_GroupName, _iter->_groupName))
			{
				Value* _val = blDictElement(_iter->_properties, blHashString((const BLUtf8*)_Name));
				if (_val)
				{
					memset(_Value, 0, 64);
					strcpy(_Value, _val->_field.strVal);
				}
				return;
			}
		}
	}
	else
	{
		FOREACH_ARRAY(TMXObjectGroup*, _iter, _PrTmxMem->pMapInfo->_objectGroups)
		{
			if (!strcmp(_GroupName, _iter->_groupName))
			{
				FOREACH_ARRAY(Value*, _iter2, _iter->_objects)
				{
					Value* _val = blDictElement(_iter2->_field.mapVal, blHashString((const BLUtf8*)_Name)); 
					if (_val)
					{
						memset(_Value, 0, 64);
						strcpy(_Value, _val->_field.strVal);
					}
					return;
				}
			}
		}
	}
}
BLVoid 
blTMXLayerVisibilityEXT(IN BLAnsi* _Layer, IN BLBool _Show)
{
}
BLBool 
blTMXLayerTileAtEXT(IN BLAnsi* _Layer, IN BLS32 _XPos, IN BLS32 _YPos)
{
	return FALSE;
}
BLBool 
blTMXLayerAsNavigationEXT(IN BLAnsi* _Layer, IN BLBool _Barrier)
{
	return TRUE;
}
BLBool 
blTMXPathFindEXT(IN BLS32 _StartX, IN BLS32 _StartY, IN BLS32 _EndX, IN BLS32 _EndY, IN BLBool _Strict, OUT BLF32** _XPath, OUT BLF32** _YPath, OUT BLU32* _PathNum)
{
	return TRUE;
}
