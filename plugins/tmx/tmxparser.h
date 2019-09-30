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
#ifndef __tmxparser_h_
#define __tmxparser_h_
#include "objectgroup.h"
#include "xml/ezxml.h"
#ifdef __cplusplus
extern "C" {
#endif
	/**
		This module is ported from cocos2dx 3.17.2
		CCTMXXMLParser
	*/
	typedef struct _TMXLayerInfo
	{
		BLDictionary*         _properties;
		char*         _name;
		Size                _layerSize;
		unsigned int            *_tiles;
		bool                _visible;
		unsigned char       _opacity;
		bool                _ownTiles;
		Vec2               _offset;
	}TMXLayerInfo;

	TMXLayerInfo* createLayerInfo();
	void destroyLayerInfo(TMXLayerInfo* obj);


	typedef struct _TMXTilesetInfo
	{
		char*     _name;
		int             _firstGid;
		Size            _tileSize;
		int             _spacing;
		int             _margin;
		Vec2            _tileOffset;
		char*     _sourceImage;
		Size            _imageSize;
		char*    _originSourceImage;
	}TMXTilesetInfo;

	TMXTilesetInfo* createTilesetInfo();
	void destroyTilesetInfo(TMXTilesetInfo* obj);
	Rect getRectForGID(TMXTilesetInfo* obj, unsigned int gid);

	typedef struct _TMXMapInfo
	{

		/// map orientation
		int    _orientation;
		///map staggerAxis
		int    _staggerAxis;
		///map staggerIndex
		int    _staggerIndex;
		///map hexsidelength
		int    _hexSideLength;
		/// map width & height
		Size _mapSize;
		/// tiles width & height
		Size _tileSize;
		/// Layers
		BLArray* _layers;
		/// tilesets
		BLArray* _tilesets;
		/// ObjectGroups
		BLArray* _objectGroups;
		/// parent element
		int _parentElement;
		/// parent GID
		int _parentGID;
		/// layer attribs
		int _layerAttribs;
		/// is storing characters?
		bool _storingCharacters;
		/// properties
		BLDictionary* _properties;
		//! xml format tile index
		int _xmlTileIndex;

		//! tmx filename
		char* _TMXFileName;
		// tmx resource path
		char* _resources;
		//! tile properties
		BLDictionary* _tileProperties;
		int _currentFirstGID;
		bool _recordFirstGID;
		char* _externalTilesetFilename;
		char* _externalTilesetFullPath;
		char _dir[260];
	}TMXMapInfo;

	TMXMapInfo* createMapInfo(const char* tmxFileName);
	void destroyMapInfo(TMXMapInfo* obj);
	bool parseXMLFile(TMXMapInfo* obj, const char* xmlFilename);
	void travelXML(TMXMapInfo* obj, ezxml_t node);
	void startElement(TMXMapInfo* obj, ezxml_t node);
	void endElement(TMXMapInfo* obj, ezxml_t node);

#ifdef __cplusplus
}
#endif
#endif/* __tmxparser_h_ */