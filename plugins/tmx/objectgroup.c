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
#include "objectgroup.h"
#include "util.h"

TMXObjectGroup* createObjectGroup()
{
	TMXObjectGroup* ret = (TMXObjectGroup*)malloc(sizeof(TMXObjectGroup));
	ret->_groupName = NULL;
	ret->_positionOffset.x = 0.f;
	ret->_positionOffset.y = 0.f;
	ret->_properties = blGenDict();
	ret->_objects = blGenArray();
	return ret;
}

void destroyObjectGroup(TMXObjectGroup* obj)
{
	freeValueMap(obj->_properties);
	FOREACH_ARRAY(BLDictionary*, _iter, obj->_objects)
	{
		freeValueMap(_iter);
	}
	blDeleteArray(obj->_objects);
	StrFree(obj->_groupName);
	free(obj);
}

void setGroupName(TMXObjectGroup* obj, const char* groupName)
{
	StrFree(obj->_groupName);
	obj->_groupName = strdcopy(groupName);
}

Value* getProperty(TMXObjectGroup* obj, const char* propertyName)
{
	if (!propertyName)
		return NULL;
	BLU32 hash = blHashString((const BLUtf8*)propertyName);
	return blDictElement(obj->_properties, hash);
}

BLDictionary* getObject(TMXObjectGroup* obj, const char* objectName)
{
	if (!objectName)
		return NULL;
	BLU32 namehash = blHashString((const BLUtf8*)"name");
	if (obj->_objects->nSize)
	{
		FOREACH_ARRAY(BLDictionary*, _iter, obj->_objects)
		{
			Value* _val = blDictElement(_iter, namehash);
			if (_val && _val->_type == TypeSTRING)
			{
				if (!strcmp(_val->_field.strVal, objectName))
				{
					return _iter;
				}
			}
		}
	}
	return NULL;
}