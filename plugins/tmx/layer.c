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
#include "layer.h"
#include "streamio.h"
#include "system.h"
#include "sprite.h"
#include <math.h>
TMXLayer* createLayer(TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo, int l)
{
	TMXLayer* layer = malloc(sizeof(TMXLayer));
	layer->_layerName = NULL;
	layer->_opacity = 0;
	layer->_vertexZvalue = 0;
	layer->_useAutomaticVertexZ = FALSE;
	layer->_contentScaleFactor = 1;
	layer->_layerSize.height = layer->_layerSize.width = 0;
	layer->_mapTileSize.height = layer->_mapTileSize.width = 0;
	layer->_tiles = NULL;
	layer->_tileSet = NULL;
	layer->_layerOrientation = TMXOrientationOrtho;
	layer->_staggerAxis = TMXStaggerAxis_Y;
	layer->_staggerIndex = TMXStaggerIndex_Even;
	layer->_hexSideLength = 0;
	layer->index = l;
	layer->_properties = blGenDict();
	layer->_vis = layerInfo->_visible;
	if (initWithTilesetInfo(layer, tilesetInfo, layerInfo, mapInfo))
		return layer;
	else
		return NULL;
}
void destroyLayer(TMXLayer* layer)
{
	StrFree(layer->_layerName);
	free(layer);
}
bool initWithTilesetInfo(TMXLayer* layer, TMXTilesetInfo *tilesetInfo, TMXLayerInfo *layerInfo, TMXMapInfo *mapInfo)
{
	Size size = layerInfo->_layerSize;
	float totalNumberOfTiles = size.width * size.height;
	float capacity = totalNumberOfTiles * 0.35f + 1; // 35 percent is occupied ?
	char workdir[260] = {0};
	char userdir[260] = {0};
	strcpy(workdir, blWorkingDir());
	strcat(workdir, tilesetInfo->_sourceImage);
	strcpy(userdir, blUserFolderDir());
	strcat(userdir, tilesetInfo->_sourceImage);
	/*
	Texture2D *texture = nullptr;
	if (tilesetInfo)
	{
		texture = Director::getInstance()->getTextureCache()->addImage(tilesetInfo->_sourceImage);
	}

	if (nullptr == texture)
		return false;
		*/

		//if (SpriteBatchNode::initWithTexture(texture, static_cast<ssize_t>(capacity)))
	if (blFileExist(workdir) || blFileExist(userdir))
	{
		// layerInfo
		layer->_layerName = strdcopy( layerInfo->_name);
		layer->_layerSize = size;
		layer->_tiles = layerInfo->_tiles;
		layer->_opacity = layerInfo->_opacity;
		layer->_properties = layerInfo->_properties;
		layer->_contentScaleFactor = 1.f;

		// tilesetInfo
		layer->_tileSet = tilesetInfo;

		// mapInfo
		layer->_mapTileSize = mapInfo->_tileSize;
		layer->_layerOrientation = mapInfo->_orientation;
		layer->_staggerAxis = mapInfo->_staggerAxis;
		layer->_staggerIndex = mapInfo->_staggerIndex;
		layer->_hexSideLength = mapInfo->_hexSideLength;

		// offset (after layer orientation is set);
		Vec2 offset = calculateLayerOffset(layer, layerInfo->_offset);
		layer->_position = offset;

		//layer->_atlasIndexArray = ccCArrayNew(totalNumberOfTiles);

		float width = 0;
		float height = 0;
		if (layer->_layerOrientation == TMXOrientationHex || layer->_layerOrientation == TMXOrientationStaggered) {
			if (layer->_staggerAxis == TMXStaggerAxis_X) {
				height = layer->_mapTileSize.height * (layer->_layerSize.height + 0.5f);
				width = (layer->_mapTileSize.width + layer->_hexSideLength) * ((int)(layer->_layerSize.width / 2)) + layer->_mapTileSize.width * ((int)layer->_layerSize.width % 2);
			}
			else {
				width = layer->_mapTileSize.width * (layer->_layerSize.width + 0.5f);
				height = (layer->_mapTileSize.height + layer->_hexSideLength) * ((int)(layer->_layerSize.height / 2)) + layer->_mapTileSize.height * ((int)layer->_layerSize.height % 2);
			}
		}
		else {
			width = layer->_layerSize.width * layer->_mapTileSize.width;
			height = layer->_layerSize.height * layer->_mapTileSize.height;
		}
		layer->_contentSize.width = width;
		layer->_contentSize.height = height;

		layer->_useAutomaticVertexZ = FALSE;
		layer->_vertexZvalue = 0;

		return TRUE;
	}
	return FALSE;
}
Vec2 calculateLayerOffset(TMXLayer* layer, Vec2 pos)
{
	float dx = layer->_mapTileSize.width * 0.5f;
	float dy = layer->_mapTileSize.height * 0.5f;
	Vec2 ret;
	switch (layer->_layerOrientation)
	{
	case TMXOrientationOrtho:
		ret.x = pos.x * layer->_mapTileSize.width + dx;
		ret.y = pos.y *layer->_mapTileSize.height + dy;
		break;
	case TMXOrientationIso:
		ret.x = (layer->_mapTileSize.width / 2) * (pos.x - pos.y) + dx;
		ret.y = (layer->_mapTileSize.height / 2) * (-pos.x - pos.y) + dy;
		break;
	case TMXOrientationHex:
	{
		if (layer->_staggerAxis == TMXStaggerAxis_Y)
		{
			int diffX = (int)((layer->_staggerIndex == TMXStaggerIndex_Even) ? layer->_mapTileSize.width / 2 : 0);
			ret.x = pos.x * layer->_mapTileSize.width + diffX + dx;
			ret.y = pos.y * (layer->_mapTileSize.height - (layer->_mapTileSize.width - layer->_hexSideLength) / 2) + dy;
		}
		else if (layer->_staggerAxis == TMXStaggerAxis_X)
		{
			int diffY = (int)((layer->_staggerIndex == TMXStaggerIndex_Odd) ? layer->_mapTileSize.height / 2 : 0);
			ret.x = pos.x * (layer->_mapTileSize.width - (layer->_mapTileSize.width - layer->_hexSideLength) / 2) + dx;
			ret.y = pos.y * layer->_mapTileSize.height - diffY + dy;
		}
		break;
	}
	case TMXOrientationStaggered:
	{
		if (layer->_staggerAxis == TMXStaggerAxis_Y)
		{
			int diffX = (int)((layer->_staggerIndex == TMXStaggerIndex_Even) ? layer->_mapTileSize.width / 2 : 0);
			ret.x = pos.x * layer->_mapTileSize.width + diffX + dx;
			ret.y = (pos.y) * layer->_mapTileSize.height / 2 + dy;

		}
		else if (layer->_staggerAxis == TMXStaggerAxis_X)
		{
			int diffY = (int)((layer->_staggerIndex == TMXStaggerIndex_Even) ? layer->_mapTileSize.height / 2 : 0);
			ret.x = pos.x * layer->_mapTileSize.width/2 + dx;
			ret.y = (pos.y) * layer->_mapTileSize.height +diffY + dy;//eve
		}

	}
	break;
	}
	return ret;
}
bool getTileAt(TMXLayer* layer, Vec2 pos)
{
	int gid = getTileGIDAt(layer, pos, NULL);

	if (gid)
		return TRUE;
	else
		return FALSE;
}
unsigned int getTileGIDAt(TMXLayer* layer, Vec2 pos, TMXTileFlags* flags)
{
	int idx = (int)(((int)pos.x + (int)pos.y * layer->_layerSize.width));
	// Bits on the far end of the 32-bit global tile ID are used for tile flags
	BLU32 tile = layer->_tiles[idx];

	// issue1264, flipped tiles can be changed dynamically
	if (flags)
	{
		*flags = (TMXTileFlags)(tile & kTMXFlipedAll);
	}

	return (tile & kTMXFlippedMask);
}
void setTileGID(TMXLayer* layer, BLU32 gid, Vec2 pos, TMXTileFlags flags)
{
	TMXTileFlags currentFlags;
	BLU32 currentGID = getTileGIDAt(layer, pos, &currentFlags);

	if (currentGID != gid || currentFlags != flags)
	{
		BLU32 gidAndFlags = gid | flags;

		// setting gid=0 is equal to remove the tile
		if (gid == 0)
		{
			removeTileAt(layer, pos);
		}
		// empty tile. create a new one
		else if (currentGID == 0)
		{
			insertTileForGID(layer, gidAndFlags, pos);
		}
		// modifying an existing tile with a non-empty tile
		else
		{
			int z = (int)pos.x + (int)(pos.y * layer->_layerSize.width);
			/*
			Sprite *sprite = static_cast<Sprite*>(getChildByTag(z));
			if (sprite)
			{
				Rect rect = _tileSet->getRectForGID(gid);
				rect = CC_RECT_PIXELS_TO_POINTS(rect);

				sprite->setTextureRect(rect, false, rect.size);
				if (flags)
				{
					setupTileSprite(sprite, sprite->getPosition(), gidAndFlags);
				}
				_tiles[z] = gidAndFlags;
			}
			else
			{
				updateTileForGID(gidAndFlags, pos);
			}*/
		}
	}
}
void removeTileAt(TMXLayer* layer, Vec2 pos)
{
	int gid = getTileGIDAt(layer, pos, NULL);

	if (gid)
	{
		int z = (int)(pos.x + pos.y * layer->_layerSize.width);
		// remove tile from GID map
		layer->_tiles[z] = 0;

		/*
		BLU32 atlasIndex = atlasIndexForExistantZ(z);


		// remove tile from atlas position array
		ccCArrayRemoveValueAtIndex(_atlasIndexArray, atlasIndex);

		// remove it from sprites and/or texture atlas
		Sprite *sprite = (Sprite*)getChildByTag(z);
		if (sprite)
		{
			SpriteBatchNode::removeChild(sprite, true);
		}
		else
		{
			_textureAtlas->removeQuadAtIndex(atlasIndex);

			// update possible children
			for (const auto &obj : _children) {
				Sprite* child = static_cast<Sprite*>(obj);
				ssize_t ai = child->getAtlasIndex();
				if (ai >= atlasIndex)
				{
					child->setAtlasIndex(ai - 1);
				}
			}
		}*/
	}
}
void setupTiles(TMXLayer* layer)
{
	layer->_tileSet->_imageSize;
	//CFByteOrder o = CFByteOrderGetCurrent();

	// Parse cocos2d properties

	for (int y = 0; y < layer->_layerSize.height; y++)
	{
		for (int x = 0; x < layer->_layerSize.width; x++)
		{
			int newX = x;
			// fix correct render ordering in Hexagonal maps when stagger axis == x
			if (layer->_staggerAxis == TMXStaggerAxis_X && ((layer->_layerOrientation == TMXOrientationHex) || (layer->_layerOrientation == TMXOrientationStaggered)))
			{
				if (layer->_staggerIndex == TMXStaggerIndex_Odd)
				{
					if (x >= layer->_layerSize.width / 2)
						newX = (int)((x - ceil(layer->_layerSize.width / 2)) * 2 + 1);
					else
						newX = x * 2;
				}
				else {
					// TMXStaggerIndex_Even
					if (x >= (int)(layer->_layerSize.width / 2))
						newX = (x - (int)(layer->_layerSize.width / 2)) * 2;
					else
						newX = x * 2 + 1;
				}
			}

			int pos = (int)(newX + layer->_layerSize.width * y);
			int gid = layer->_tiles[pos];

			// gid are stored in little endian.
			// if host is big endian, then swap
			//if( o == CFByteOrderBigEndian )
			//    gid = CFSwapInt32( gid );
			/* We support little endian.*/

			// FIXME:: gid == 0 --> empty tile
			if (gid != 0)
			{
				Vec2 xx;
				xx.x = (float)newX;
				xx.y = (float)y;
				appendTileForGID(layer, gid, xx);
			}
		}
	}
}
void appendTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos)
{
	if (gid != 0 && ((int)((gid & kTMXFlippedMask)) - layer->_tileSet->_firstGid) >= 0)
	{
		Rect rect = getRectForGID(layer->_tileSet, gid);

		// Z could be just an integer that gets incremented each time it is called.
		// but that wouldn't work on layers with empty tiles.
		// and it is IMPORTANT that Z returns an unique and bigger number than the previous one.
		// since _atlasIndexArray must be ordered because `bsearch` is used to find the GID for
		// a given Z. (github issue #16512)
		int z = getZForPos(layer, pos);

		//Sprite *tile = reusedTileWithRect(rect);

		setupTileSprite(layer, pos, rect, gid);

		// optimization:
		// The difference between appendTileForGID and insertTileforGID is that append is faster, since
		// it appends the tile at the end of the texture atlas
		//ssize_t indexForZ = _atlasIndexArray->num;

		// don't add it using the "standard" way.
		//insertQuadFromSprite(tile, indexForZ);

		// append should be after addQuadFromSprite since it modifies the quantity values
		//ccCArrayInsertValueAtIndex(_atlasIndexArray, (void*)z, indexForZ);

	
	}
}
void insertTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos)
{
}
void updateTileForGID(TMXLayer* layer, BLU32 gid, Vec2 pos)
{
}
int getZForPos(TMXLayer* layer, Vec2 pos)
{
	int z = -1;
	// fix correct render ordering in Hexagonal maps when stagger axis == x
	if (layer->_staggerAxis == TMXStaggerAxis_X && ((layer->_layerOrientation == TMXOrientationHex) || (layer->_layerOrientation == TMXOrientationStaggered)))
	{
		if (layer->_staggerIndex == TMXStaggerIndex_Odd)
		{
			if (((int)pos.x % 2) == 0)
				z = (int)(pos.x / 2 + pos.y * layer->_layerSize.width);
			else
				z = (int)(pos.x / 2 + ceil(layer->_layerSize.width / 2) + pos.y * layer->_layerSize.width);
		}
		else {
			// TMXStaggerIndex_Even
			if (((int)pos.x % 2) == 1)
				z = (int)(pos.x / 2 + pos.y * layer->_layerSize.width);
			else
				z = (int)(pos.x / 2 + floor(layer->_layerSize.width / 2) + pos.y * layer->_layerSize.width);
		}
	}
	else
	{
		z = (int)(pos.x + pos.y * layer->_layerSize.width);
	}
	return z;
}
void setupTileSprite(TMXLayer* layer, Vec2 pos, Rect tr, BLU32 gid)
{
	int _rot = 0;
	bool flipx = FALSE;
	bool flipy = FALSE;
	if (gid & kTMXTileDiagonalFlag)
	{
		BLU32 flag = gid & (kTMXTileHorizontalFlag | kTMXTileVerticalFlag);

		// handle the 4 diagonally flipped states.
		if (flag == kTMXTileHorizontalFlag)
		{
			_rot = 90;
		}
		else if (flag == kTMXTileVerticalFlag)
		{
			_rot = 270;
		}
		else if (flag == (kTMXTileVerticalFlag | kTMXTileHorizontalFlag))
		{
			_rot = 90;
			flipx = 1;
		}
		else
		{
			_rot = 270;
			flipx = 1;
		}
	}
	else
	{
		if (gid & kTMXTileHorizontalFlag)
		{
			flipx = 1;
		}

		if (gid & kTMXTileVerticalFlag)
		{
			flipy = 1;
		}
	}
	BLAnsi _buf[32] = { 0 };
	memset(_buf, 0, 32);
	sprintf(_buf, "@#tilespritelayer_%d_%d#@", layer->index, gid);
	BLAnsi _buflayer[32] = { 0 };
	sprintf(_buflayer, "%d", layer->index);
	BLGuid _id = blGenSprite(_buf, _buflayer, tr.size.width, tr.size.height, 0, _rot, TRUE);
	Vec2 p = getPositionAt(layer, pos);
	BLF32 ltx = !flipx ? (tr.origin.x / layer->_tileSet->_imageSize.width) : ((tr.size.width + tr.origin.x) / layer->_tileSet->_imageSize.width);
	BLF32 lty = !flipy ? (tr.origin.y / layer->_tileSet->_imageSize.height) : ((tr.size.height + tr.origin.y) / layer->_tileSet->_imageSize.height);
	BLF32 rbx = flipx ? (tr.origin.x / layer->_tileSet->_imageSize.width) : ((tr.size.width + tr.origin.x) / layer->_tileSet->_imageSize.width);
	BLF32 rby = flipy ? (tr.origin.y / layer->_tileSet->_imageSize.height) : ((tr.size.height + tr.origin.y) / layer->_tileSet->_imageSize.height);
	blSpriteTile(_id, layer->_tileSet->_sourceImage, ltx, lty, rbx, rby, 
		p.x+layer->_position.x, 
		p.y + layer->_position.y, 
		layer->_opacity / 255.f, layer->_vis);
}
Vec2 getPositionAt(TMXLayer* layer, Vec2 pos)
{
	Vec2 ret;
	switch (layer->_layerOrientation)
	{
	case TMXOrientationOrtho:
		ret = getPositionForOrthoAt(layer, pos);
		break;
	case TMXOrientationIso:
		ret = getPositionForIsoAt(layer, pos);
		break;
	case TMXOrientationHex:
		ret = getPositionForHexAt(layer, pos);
		break;
	case TMXOrientationStaggered:
		ret = getPositionForStaggeredAt(layer, pos);
		break;
	}
	return ret;
}
Vec2 getPositionForIsoAt(TMXLayer* layer, Vec2 pos)
{
	Vec2 ret;
	ret.x = layer->_mapTileSize.width / 2 * (layer->_layerSize.width + pos.x - pos.y - 1);
	ret.y = layer->_mapTileSize.height / 2 * ((layer->_layerSize.height * 2 - pos.x - pos.y) - 2);
	return ret;
}
Vec2 getPositionForOrthoAt(TMXLayer* layer, Vec2 pos)
{
	Vec2 ret;
	ret.x = pos.x * layer->_mapTileSize.width;
	ret.y = pos.y * layer->_mapTileSize.height;
	return ret;
}
Vec2 getPositionForHexAt(TMXLayer* layer, Vec2 pos)
{
	Vec2 xy;
	Vec2 offset = layer->_tileSet->_tileOffset;

	int odd_even = (layer->_staggerIndex == TMXStaggerIndex_Odd) ? 1 : -1;
	switch (layer->_staggerAxis)
	{
	case TMXStaggerAxis_Y:
	{
		float diffX = 0;
		if ((int)pos.y % 2 == 1)
		{
			diffX = layer->_mapTileSize.width / 2 * odd_even;
		}
		xy.x = pos.x * layer->_mapTileSize.width + diffX + offset.x;
		xy.y = pos.y* (layer->_mapTileSize.height - (layer->_mapTileSize.height - layer->_hexSideLength) / 2) + offset.y;
		break;
	}

	case TMXStaggerAxis_X:
	{
		float diffY = 0;
		if ((int)pos.x % 2 == 1)
		{
			diffY = layer->_mapTileSize.height / 2 * -odd_even;
		}

		xy.x = pos.x * (layer->_mapTileSize.width - (layer->_mapTileSize.width - layer->_hexSideLength) / 2) + offset.x;
		xy.y = pos.y * layer->_mapTileSize.height - diffY + offset.y;
		break;
	}
	}
	return xy;
}
Vec2 getPositionForStaggeredAt(TMXLayer* layer, Vec2 pos)
{
	Vec2 xy;
	int odd_even = (layer->_staggerIndex == TMXStaggerIndex_Odd) ? 1 : -1;
	switch (layer->_staggerAxis)
	{
	case TMXStaggerAxis_Y:
	{
		float diffX = 0;
		if ((int)pos.y % 2 == 1)
		{
			diffX = layer->_mapTileSize.width / 2 * odd_even;
		}
		xy.x = pos.x * layer->_mapTileSize.width + diffX;
		xy.y = pos.y * layer->_mapTileSize.height / 2;
	}
	break;

	case TMXStaggerAxis_X:
	{
		float diffY = 0;
		if ((int)pos.x % 2 == 1)
		{
			diffY = layer->_mapTileSize.height / 2 * -odd_even;
		}

		xy.x = pos.x * layer->_mapTileSize.width / 2;
		xy.y = pos.y * layer->_mapTileSize.height - diffY;
	}
	break;
	}
	return xy;
}
