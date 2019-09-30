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
#ifndef __objectgroup_h_
#define __objectgroup_h_
#include "misc.h"
#include "array.h"
#include "dictionary.h"
#ifdef __cplusplus
extern "C" {
#endif
	/**
		This module is ported from cocos2dx 3.17.2
		CCTMXObjectGroup
	*/
	typedef struct _ObjectGroup {
		/** name of the group */
		char* _groupName;
		/** offset position of child objects */
		Vec2 _positionOffset;
		/** list of properties stored in a dictionary */
		BLDictionary* _properties;
		/** array of the objects */
		BLArray* _objects;
	} TMXObjectGroup;

	TMXObjectGroup* createObjectGroup();

	void destroyObjectGroup(TMXObjectGroup* obj);

	void setGroupName(TMXObjectGroup* obj, const char* groupName);

	Value* getProperty(TMXObjectGroup* obj, const char* propertyName);

	BLDictionary* getObject(TMXObjectGroup* obj, const char* objectName);
#ifdef __cplusplus
}
#endif
#endif/* __objectgroup_h_ */
