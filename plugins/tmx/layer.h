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
#ifndef __layer_h_
#define __layer_h_
#include "tmxparser.h"
#ifdef __cplusplus
extern "C" {
#endif
	/**
		This module is ported from cocos2dx 3.17.2
		CCTMXLayer
	*/
	typedef struct _TMXLayer {
		//! name of the layer
		char* _layerName;
		//! TMX Layer supports opacity
		unsigned char _opacity;

		//! Only used when vertexZ is used
		int _vertexZvalue;
		bool _useAutomaticVertexZ;

		// used for retina display
		float _contentScaleFactor;

		/** size of the layer in tiles */
		Size _layerSize;
		/** size of the map's tile (could be different from the tile's size) */
		Size _mapTileSize;
		/** pointer to the map of tiles */
		unsigned int* _tiles;
		/** Tileset information for the layer */
		TMXTilesetInfo* _tileSet;
		/** Layer orientation, which is the same as the map orientation */
		int _layerOrientation;
		/** Stagger Axis */
		int _staggerAxis;
		/** Stagger Index */
		int _staggerIndex;
		/** Hex side length*/
		int _hexSideLength;
		/** properties from the layer. They can be added using Tiled */
		BLDictionary* _properties;
		
		Vec2 _position;
		Size _contentSize;
		bool _vis;

		int index;
	} TMXLayer;

	TMXLayer* createLayer(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo, int layer);
	void destroyLayer(TMXLayer* layer);
	bool initWithTilesetInfo(TMXLayer* layer, TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo);
	Vec2 calculateLayerOffset(TMXLayer* layer, Vec2 pos);
	bool getTileAt(TMXLayer* layer, Vec2 pos);
	unsigned int getTileGIDAt(TMXLayer* layer, Vec2 tileCoordinate, TMXTileFlags* flags);
	void setTileGID(TMXLayer* layer, BLU32 gid, Vec2 tileCoordinate, TMXTileFlags flags);
	void removeTileAt(TMXLayer* layer, Vec2 tileCoordinate);
	void setupTiles(TMXLayer* layer);
	void appendTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos);
	void insertTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos);
	void updateTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos);
	int getZForPos(TMXLayer* layer, Vec2 pos);
	void setupTileSprite(TMXLayer* layer, Vec2 pos, Rect sz, BLU32 gid);
	Vec2 getPositionAt(TMXLayer* layer, Vec2 pos);
	Vec2 getPositionForIsoAt(TMXLayer* layer, Vec2 pos);
	Vec2 getPositionForOrthoAt(TMXLayer* layer, Vec2 pos);
	Vec2 getPositionForHexAt(TMXLayer* layer, Vec2 pos);
	Vec2 getPositionForStaggeredAt(TMXLayer* layer, Vec2 pos);

#ifdef __cplusplus
}
#endif
#endif/* __layer_h_ */