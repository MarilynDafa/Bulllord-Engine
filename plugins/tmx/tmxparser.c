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
#include "tmxparser.h"
#include "util.h"
#include "streamio.h"

TMXLayerInfo* createLayerInfo()
{
	TMXLayerInfo* obj = (TMXLayerInfo*)malloc(sizeof(TMXLayerInfo));
	obj->_properties = blGenDict();
	obj->_name = NULL;
	obj->_layerSize.height = obj->_layerSize.width = 0.f;
	obj->_tiles = NULL;
	obj->_visible = TRUE;
	obj->_opacity = 255;
	obj->_ownTiles = TRUE;
	obj->_offset.x = obj->_offset.y = 0.f;
	return obj;
}

void destroyLayerInfo(TMXLayerInfo* obj)
{
	free(obj->_tiles);
	StrFree(obj->_name);
	freeValueMap(obj->_properties);
	free(obj);
}

TMXTilesetInfo* createTilesetInfo()
{
	TMXTilesetInfo* obj = (TMXTilesetInfo*)malloc(sizeof(TMXTilesetInfo));
	obj->_name = NULL;
	obj->_firstGid = 0;
	obj->_tileSize.height = obj->_tileSize.width = 0;
	obj->_spacing = 0;
	obj->_margin = 0;
	obj->_tileOffset.x = obj->_tileOffset.y = 0;
	obj->_sourceImage = NULL;
	obj->_imageSize.height = obj->_imageSize.width = 0;
	obj->_originSourceImage = NULL;
	return obj;
}

void destroyTilesetInfo(TMXTilesetInfo* obj)
{
	StrFree(obj->_name);
	StrFree(obj->_sourceImage);
	StrFree(obj->_originSourceImage);
	free(obj);
}

Rect getRectForGID(TMXTilesetInfo* obj, unsigned int gid)
{
	Rect rect;
	rect.size = obj->_tileSize;
	gid &= kTMXFlippedMask;
	gid = gid - obj->_firstGid;
	// max_x means the column count in tile map
	// in the origin:
	// max_x = (int)((_imageSize.width - _margin*2 + _spacing) / (_tileSize.width + _spacing));
	// but in editor "Tiled", _margin variable only effect the left side
	// for compatible with "Tiled", change the max_x calculation
	int max_x = (int)((obj->_imageSize.width - obj->_margin + obj->_spacing) / (obj->_tileSize.width + obj->_spacing));

	rect.origin.x = (gid % max_x) * (obj->_tileSize.width + obj->_spacing) + obj->_margin;
	rect.origin.y = (gid / max_x) * (obj->_tileSize.height + obj->_spacing) + obj->_margin;
	return rect;
}


TMXMapInfo* createMapInfo(const char* tmxFileName)
{
	TMXMapInfo* info = malloc(sizeof(TMXMapInfo));

	info->_TMXFileName = strdcopy(tmxFileName);
	// tmp vars
	info->_storingCharacters = FALSE;
	info->_layerAttribs = TMXLayerAttribNone;
	info->_parentElement = TMXPropertyNone;
	info->_currentFirstGID = -1;

	info->_orientation = TMXOrientationOrtho;
	info->_staggerAxis = TMXStaggerAxis_Y;
	info->_staggerIndex = TMXStaggerIndex_Even;
	info->_hexSideLength = 0;
	SizeInit(info->_mapSize);
	SizeInit(info->_tileSize);
	info->_parentElement = 0;
	info->_parentGID = 0;
	info->_layerAttribs = 0;
	info->_storingCharacters = FALSE;
	info->_xmlTileIndex = 0;
	info->_resources = NULL;
	info->_currentFirstGID = -1;
	info->_recordFirstGID = TRUE;
	info->_externalTilesetFilename = NULL;
	info->_externalTilesetFullPath = NULL;

	info->_layers = blGenArray();
	info->_tilesets = blGenArray();
	info->_objectGroups = blGenArray();
	info->_properties = blGenDict();
	info->_tileProperties = blGenDict();

	memset(info->_dir, 0, 260);
	BLS32 _filelen = (BLS32)strlen(tmxFileName);
	BLS32 _split = 0;
	for (_split = _filelen - 1; _split > 0; --_split)
	{
		if (tmxFileName[_split] == '/' || tmxFileName[_split] == '\\')
			break;
	}
	if (_split > 0)
	{
		strncpy(info->_dir, tmxFileName, _split);
		strcat(info->_dir, "/");
	}

	if (parseXMLFile(info, info->_TMXFileName))
	{
		//return info;
	}
	destroyMapInfo(info);
	return NULL;	
}

void destroyMapInfo(TMXMapInfo* obj)
{
	{
		FOREACH_ARRAY(TMXLayerInfo*, iter, obj->_layers)
		{
			destroyLayerInfo(iter);
		}
		blDeleteArray(obj->_layers);
	}
	{
		FOREACH_ARRAY(TMXTilesetInfo*, iter, obj->_tilesets)
		{
			destroyTilesetInfo(iter);
		}
		blDeleteArray(obj->_tilesets);
	}
	{
		FOREACH_ARRAY(TMXObjectGroup*, iter, obj->_objectGroups)
		{
			destroyObjectGroup(iter);
		}
		blDeleteArray(obj->_objectGroups);
	}

	freeValueMap(obj->_properties);
	freeValueMap(obj->_tileProperties);
	free(obj);
}
bool parseXMLFile(TMXMapInfo* obj, const char* xmlFilename)
{
	BLGuid _stream;
	_stream = blGenStream(xmlFilename);
	ezxml_t _doc = ezxml_parse_str((BLAnsi*)blStreamData(_stream), blStreamLength(_stream));
	travelXML(obj, _doc);
	ezxml_free(_doc);
	blDeleteStream(_stream);
	return TRUE;
}
void travelXML(TMXMapInfo* obj, ezxml_t node)
{
	startElement(obj, node);
	ezxml_t n = node->child;
	ezxml_t end = node->sibling;
	if (n)
	{
		for (; n != end;)
		{
			travelXML(obj, n);
			if (n->ordered)
				n = n->ordered;
			else
			{
				endElement(obj, node);
				break;
			}
		}
	}
	else
		endElement(obj, node);
}
void startElement(TMXMapInfo* tmxMapInfo, ezxml_t node)
{
	const char* elementName = ezxml_name(node);
	if (!strcmp(elementName, "map"))
	{
		const char* version = ezxml_attr(node, "version");
		
		const char* orientationStr = ezxml_attr(node, "orientation");
		if (orientationStr)
		{
			if (!strcmp(orientationStr, "orthogonal")) {
				tmxMapInfo->_orientation = (TMXOrientationOrtho);
			}
			else if (!strcmp(orientationStr, "isometric")) {
				tmxMapInfo->_orientation = (TMXOrientationIso);
			}
			else if (!strcmp(orientationStr, "hexagonal")) {
				tmxMapInfo->_orientation = (TMXOrientationHex);
			}
			else if (!strcmp(orientationStr, "staggered")) {
				tmxMapInfo->_orientation = (TMXOrientationStaggered);
			}
		}

		const char* staggerAxisStr = ezxml_attr(node, "staggeraxis");
		if (staggerAxisStr)
		{
			if (!strcmp(staggerAxisStr, "x")) {
				tmxMapInfo->_staggerAxis = (TMXStaggerAxis_X);
			}
			else if (!strcmp(staggerAxisStr, "y")) {
				tmxMapInfo->_staggerAxis = (TMXStaggerAxis_Y);
			}
		}

		const char* staggerIndex = ezxml_attr(node, "staggerindex");
		if (staggerIndex)
		{
			if (!strcmp(staggerIndex, "odd")) {
				tmxMapInfo->_staggerIndex = (TMXStaggerIndex_Odd);
			}
			else if (!strcmp(staggerIndex, "even")) {
				tmxMapInfo->_staggerIndex = (TMXStaggerIndex_Even);
			}
		}


		int hexSideLength = ezxml_attrd(node, "hexsidelength");
		tmxMapInfo->_hexSideLength = (hexSideLength);

		Size s;
		s.width = ezxml_attrf(node, "width");
		s.height = ezxml_attrf(node, "height");
		tmxMapInfo->_mapSize = (s);

		s.width = ezxml_attrf(node, "tilewidth");
		s.height = ezxml_attrf(node, "tileheight");
		tmxMapInfo->_tileSize = (s);



		// The parent element is now "map"
		tmxMapInfo->_parentElement = (TMXPropertyMap);
	}
	else if (!strcmp(elementName, "tileset"))
	{
		// If this is an external tileset then start parsing that
		const char* externalTilesetFilename = ezxml_attr(node, "source");
		if (externalTilesetFilename)
		{

			tmxMapInfo->_currentFirstGID = ezxml_attrd(node, "firstgid");
			if (tmxMapInfo->_currentFirstGID < 0)
			{
				tmxMapInfo->_currentFirstGID = 0;
			}
			tmxMapInfo->_recordFirstGID = FALSE;
			tmxMapInfo->_externalTilesetFilename = calloc(1, strlen(externalTilesetFilename) + 1 + strlen(tmxMapInfo->_dir));
			strcpy(tmxMapInfo->_externalTilesetFilename, tmxMapInfo->_dir);
			strcat(tmxMapInfo->_externalTilesetFilename, externalTilesetFilename);
			tmxMapInfo->_externalTilesetFullPath = strdcopy(tmxMapInfo->_externalTilesetFilename);
			parseXMLFile(tmxMapInfo, tmxMapInfo->_externalTilesetFullPath);
		}
		else
		{
			TMXTilesetInfo *tileset = createTilesetInfo();
			tileset->_name = strdcopy(ezxml_attr(node, "name"));

			if (tmxMapInfo->_recordFirstGID)
			{
				// unset before, so this is tmx file.
				tileset->_firstGid = ezxml_attrd(node, "firstgid");

				if (tileset->_firstGid < 0)
				{
					tileset->_firstGid = 0;
				}
			}
			else
			{
				tileset->_firstGid = tmxMapInfo->_currentFirstGID;
				tmxMapInfo->_currentFirstGID = 0;
			}

			tileset->_spacing = ezxml_attrd(node, "spacing");
			tileset->_margin = ezxml_attrd(node, "margin");
			Size s;
			s.width = ezxml_attrf(node, "tilewidth");
			s.height = ezxml_attrf(node, "tileheight");
			tileset->_tileSize = s;

			blArrayPushBack(tmxMapInfo->_tilesets, tileset);
		}
	}
	else if (!strcmp(elementName , "tile"))
	{
		if (tmxMapInfo->_parentElement == TMXPropertyLayer)
		{
			TMXLayerInfo* layer = blArrayBackElement(tmxMapInfo->_layers);
			Size layerSize = layer->_layerSize;
			unsigned int gid = ezxml_attru(node, "gid");
			int tilesAmount = (int)(layerSize.width*layerSize.height);

			if (tmxMapInfo->_xmlTileIndex < tilesAmount)
			{
				layer->_tiles[tmxMapInfo->_xmlTileIndex++] = gid;
			}
		}
		else
		{
			TMXTilesetInfo* info = blArrayBackElement(tmxMapInfo->_tilesets);
			tmxMapInfo->_parentGID = (info->_firstGid + ezxml_attrd(node, "id"));
			blDictInsert(tmxMapInfo->_tileProperties, tmxMapInfo->_parentGID, newVMap(NULL));
			tmxMapInfo->_parentElement = (TMXPropertyTile);
		}
	}
	else if (!strcmp(elementName , "layer"))
	{
		TMXLayerInfo *layer = createLayerInfo();
		layer->_name = strdcopy( ezxml_attr(node, "name"));

		Size s;
		s.width = ezxml_attrf(node, "width");
		s.height = ezxml_attrf(node, "height");
		layer->_layerSize = s;

		layer->_visible = ezxml_attrb(node, "visible", TRUE);

		const char* opacityValue = ezxml_attr(node, "opacity");
		if (opacityValue)
			layer->_opacity = (unsigned char)(255.0f * strtod(opacityValue, NULL));
		else
			layer->_opacity = 255;

		float x = ezxml_attrf(node, "x");
		float y = ezxml_attrf(node, "y");
		layer->_offset.x = x;
		layer->_offset.y = y;

		blArrayPushBack(tmxMapInfo->_layers, layer);

		// The parent element is now "layer"
		tmxMapInfo->_parentElement = (TMXPropertyLayer);
	}
	else if (!strcmp(elementName , "objectgroup"))
	{
		TMXObjectGroup *objectGroup = createObjectGroup();
		objectGroup->_groupName = strdcopy(ezxml_attr(node, "name"));
		Vec2 positionOffset;
		positionOffset.x = ezxml_attrf(node, "x") * tmxMapInfo->_tileSize.width;
		positionOffset.y = ezxml_attrf(node, "y") * tmxMapInfo->_tileSize.height;
		objectGroup->_positionOffset = (positionOffset);

		blArrayPushBack( tmxMapInfo->_objectGroups, objectGroup);

		// The parent element is now "objectgroup"
		tmxMapInfo->_parentElement = (TMXPropertyObjectGroup);
	}
	else if (!strcmp(elementName , "tileoffset"))
	{
		TMXTilesetInfo* tileset = blArrayBackElement(tmxMapInfo->_tilesets);

		float tileOffsetX = ezxml_attrf(node, "x");

		float tileOffsetY = ezxml_attrf(node, "y");

		tileset->_tileOffset.x = tileOffsetX;
		tileset->_tileOffset.y = tileOffsetY;

	}
	else if (!strcmp(elementName , "image"))
	{
		TMXTilesetInfo* tileset = blArrayBackElement(tmxMapInfo->_tilesets); 

		// build full path
		const char* imagename = ezxml_attr(node, "source");
		tileset->_originSourceImage = strdcopy(imagename);

		BLU32 _sourcelen = (BLU32)strlen(tileset->_originSourceImage);
		for (int _idx = (int)_sourcelen - 1; _idx >= 0; --_idx)
		{
			if (tileset->_originSourceImage[_idx] == '.')
				break;
			else
				tileset->_originSourceImage[_idx] = 0;
		}
		strcat(tileset->_originSourceImage, "bmg");
		tileset->_sourceImage = calloc(1, strlen(tileset->_originSourceImage) + strlen(tmxMapInfo->_dir) +1 );
		strcpy(tileset->_sourceImage, tmxMapInfo->_dir);
		strcat(tileset->_sourceImage, tileset->_originSourceImage);

	}
	else if (!strcmp(elementName , "data"))
	{
		const char* encoding = ezxml_attr(node, "encoding");
		const char* compression = ezxml_attr(node, "compression");

		if (!strcmp(encoding , ""))
		{
			tmxMapInfo->_layerAttribs = (tmxMapInfo->_layerAttribs | TMXLayerAttribNone);

			TMXLayerInfo* layer = blArrayBackElement(tmxMapInfo->_layers);
			Size layerSize = layer->_layerSize;
			int tilesAmount = (int)(layerSize.width*layerSize.height);

			BLU32 *tiles = (BLU32*)malloc(tilesAmount * sizeof(BLU32));
			// set all value to 0
			memset(tiles, 0, tilesAmount * sizeof(int));

			layer->_tiles = tiles;
		}
		else if (!strcmp(encoding , "base64"))
		{
			int layerAttribs = tmxMapInfo->_layerAttribs;
			tmxMapInfo->_layerAttribs = (layerAttribs | TMXLayerAttribBase64);
			tmxMapInfo->_storingCharacters = (TRUE);

			if (!strcmp(compression ,"gzip"))
			{
				layerAttribs = tmxMapInfo->_layerAttribs;
				tmxMapInfo->_layerAttribs = (layerAttribs | TMXLayerAttribGzip);
			}
			else
				if (!strcmp(compression , "zlib"))
				{
					layerAttribs = tmxMapInfo->_layerAttribs;
					tmxMapInfo->_layerAttribs = (layerAttribs | TMXLayerAttribZlib);
				}
		}
		else if (!strcmp(encoding , "csv"))
		{
			int layerAttribs = tmxMapInfo->_layerAttribs;
			tmxMapInfo->_layerAttribs = (layerAttribs | TMXLayerAttribCSV);
			tmxMapInfo->_storingCharacters = (TRUE);
		}
	}
	else if (!strcmp(elementName , "object"))
	{
		TMXObjectGroup* objectGroup = blArrayBackElement(tmxMapInfo->_objectGroups);

		// The value for "type" was blank or not a valid class name
		// Create an instance of TMXObjectInfo to store the object and its properties
		BLDictionary* dict = blGenDict();
		// Parse everything automatically
		const char* keys[] = { "name", "type", "width", "height", "gid", "id" };

		blDictInsert(dict, blHashString("name"), newStr(ezxml_attr(node, "name")));
		blDictInsert(dict, blHashString("type"), newStr(ezxml_attr(node, "type")));
		blDictInsert(dict, blHashString("width"), newFloat(ezxml_attrf(node, "width")));
		blDictInsert(dict, blHashString("height"), newFloat(ezxml_attrf(node, "height")));
		blDictInsert(dict, blHashString("gid"), newInt(ezxml_attrd(node, "gid")));
		blDictInsert(dict, blHashString("id"), newInt(ezxml_attrd(node, "id")));

		// But X and Y since they need special treatment
		// X
		int x = ezxml_attrd(node, "x");
		// Y
		int y = ezxml_attrd(node, "y");

		blDictInsert(dict, blHashString("x"), newFloat(x + objectGroup->_positionOffset.x));
		blDictInsert(dict, blHashString("y"), newFloat((tmxMapInfo->_mapSize.height * tmxMapInfo->_tileSize.height - y - objectGroup->_positionOffset.y - ezxml_attrd(node, "height"))));
	

		blDictInsert(dict, blHashString("rotation"), newFloat(ezxml_attrf(node, "rotation")));

		// Add the object to the objectGroup
		blArrayPushBack( objectGroup->_objects , newVMap(dict));

		// The parent element is now "object"
		tmxMapInfo->_parentElement = (TMXPropertyObject);
	}
	else if (!strcmp(elementName , "property"))
	{
		if (tmxMapInfo->_parentElement == TMXPropertyNone)
		{
			blDebugOutput("TMX tile map: Parent element is unsupported. Cannot add property named");
		}
		else if (tmxMapInfo->_parentElement == TMXPropertyMap)
		{
			// The parent element is the map
			const char* key = ezxml_attr(node, "name");
			blDictInsert( tmxMapInfo->_properties , blHashString(key), newStr(ezxml_attr(node, "value")));
		}
		else if (tmxMapInfo->_parentElement == TMXPropertyLayer)
		{
			// The parent element is the last layer
			TMXLayerInfo* layer = blArrayBackElement(tmxMapInfo->_layers);
			const char* key = ezxml_attr(node, "name");
			blDictInsert(layer->_properties, blHashString(key), newStr(ezxml_attr(node, "value")));
		}
		else if (tmxMapInfo->_parentElement== TMXPropertyObjectGroup)
		{
			// The parent element is the last object group
			TMXObjectGroup* objectGroup = blArrayBackElement(tmxMapInfo->_objectGroups);
			const char* key = ezxml_attr(node, "name");
			blDictInsert(objectGroup->_properties, blHashString(key), newStr(ezxml_attr(node, "value")));
		}
		else if (tmxMapInfo->_parentElement == TMXPropertyObject)
		{
			// The parent element is the last object
			TMXObjectGroup* objectGroup = blArrayBackElement(tmxMapInfo->_objectGroups);
			Value* dict = blArrayBackElement(objectGroup->_objects);
			blDictInsert(dict->_field.mapVal, blHashString(ezxml_attr(node, "name")), newStr(ezxml_attr(node, "value")));
		}
		else if (tmxMapInfo->_parentElement == TMXPropertyTile)
		{
			Value* dict =  blDictElement(tmxMapInfo->_tileProperties, tmxMapInfo->_parentGID);



			blDictInsert(dict->_field.mapVal, blHashString(ezxml_attr(node, "name")), newStr(ezxml_attr(node, "value")));
		}
	}
	else if (!strcmp(elementName , "polygon"))
	{
		// find parent object's dict and add polygon-points to it
		TMXObjectGroup* objectGroup = blArrayBackElement(tmxMapInfo->_objectGroups);
		Value* dict = blArrayBackElement(objectGroup->_objects);

		// get points value string
		const char* _points = ezxml_attr(node, "points");
		if (_points)
		{
			BLAnsi* _tmp = (char*)_points;
			BLAnsi* _params[256];
			BLU32 _idx = 0;
			_params[_idx] = strtok(_tmp, " ");
			while (_tmp)
			{
				_idx++;
				_tmp = strtok(NULL, " ");
				_params[_idx] = _tmp;
			}
			float* ptdata = malloc(_idx * 2 * sizeof(BLF32));
			for (unsigned int _jdx = 0; _jdx < _idx; ++_jdx)
			{
				ptdata[_jdx * 2 + 0] = (BLF32)strtod(strtok(_params[_jdx], ","), NULL);
				ptdata[_jdx * 2 + 1] = (BLF32)strtod(strtok(NULL, ","), NULL);
			}

			blDictInsert(dict->_field.mapVal, blHashString("points"), newArray(ptdata));
		}
	}
	else if (!strcmp(elementName , "polyline"))
	{
		TMXObjectGroup* objectGroup = blArrayBackElement(tmxMapInfo->_objectGroups);
		Value* dict = blArrayBackElement(objectGroup->_objects);

		// get points value string
		const char* _points = ezxml_attr(node, "points");
		if (_points)
		{
			BLAnsi* _tmp = (char*)_points;
			BLAnsi* _params[256];
			BLU32 _idx = 0;
			_params[_idx] = strtok(_tmp, " ");
			while (_tmp)
			{
				_idx++;
				_tmp = strtok(NULL, " ");
				_params[_idx] = _tmp;
			}
			float* ptdata = malloc(_idx * 2 * sizeof(BLF32));
			for (unsigned int _jdx = 0; _jdx < _idx; ++_jdx)
			{
				ptdata[_jdx * 2 + 0] = (BLF32)strtod(strtok(_params[_jdx], ","), NULL);
				ptdata[_jdx * 2 + 1] = (BLF32)strtod(strtok(NULL, ","), NULL);
			}

			blDictInsert(dict->_field.mapVal, blHashString("points"), newArray(ptdata));
		}
	}
}
void endElement(TMXMapInfo* tmxMapInfo, ezxml_t node)
{
	const char* elementName = ezxml_name(node);

	if (!strcmp(elementName, "data"))
	{
		if (tmxMapInfo->_layerAttribs & TMXLayerAttribBase64)
		{
			tmxMapInfo->_storingCharacters = (FALSE);

			TMXLayerInfo* layer = blArrayBackElement(tmxMapInfo->_layers);

			int _datalen = strlen(node->txt);

			int jdx = 0;
			BLAnsi* _trimdata = (BLAnsi*)calloc(1, _datalen);
			for (int _idx = 0, _jdx = 0; _idx < _datalen; ++_idx)
			{
				if (node->txt[_idx] != '\n' && node->txt[_idx] != '\t' && node->txt[_idx] != ' ')
				{
					_trimdata[_jdx] = node->txt[_idx];
					++_jdx;
				}
			}
			int len;
			BLU8* buffer = (BLU8*)blGenBase64Decoder(_trimdata, &len);
			if (!buffer)
			{
				blDebugOutput("TiledMap: decode data error");
				return;
			}

			if (tmxMapInfo->_layerAttribs & (TMXLayerAttribGzip | TMXLayerAttribZlib))
			{
				unsigned char *deflated = NULL;
				Size s = layer->_layerSize;
				// int sizeHint = s.width * s.height * sizeof(uint32_t);
				unsigned int sizeHint = (unsigned int)(s.width * s.height * sizeof(unsigned int));

				unsigned int inflatedLen = inflateMemoryWithHint(buffer, len, &deflated, sizeHint);
				assert(inflatedLen == sizeHint);


				if (!deflated)
				{
					blDebugOutput("TiledMap: inflate data error");
					return;
				}

				layer->_tiles = (unsigned int*)(deflated);
			}
			else
			{
				layer->_tiles = (unsigned int*)(buffer);
			}
			blDeleteBase64Decoder((BLU8*)buffer);

		}
		else if (tmxMapInfo->_layerAttribs & TMXLayerAttribCSV)
		{
			unsigned char *buffer;

			TMXLayerInfo* layer = blArrayBackElement(tmxMapInfo->_layers);

			tmxMapInfo->_storingCharacters = (FALSE);
			char* currentString = node->txt;

			char* _tmp = currentString;
			BLU32 _idx = 0;
			BLU32* gidTokens = malloc(100 * sizeof(BLU32));
			BLU32 sz = 100;
			strtok(_tmp, ",");
			while (_tmp)
			{
				BLU32 _val = strtoul(_tmp, NULL, 10);
				gidTokens[_idx++] = _val;
				_tmp = strtok(NULL, ",");
				if (_idx >= sz)
				{
					gidTokens = realloc(gidTokens, (100+ sz)*sizeof(BLU32));
					sz += 100;
				}
			}

			// 32-bits per gid
			buffer = (unsigned char*)malloc(_idx * 4);
			if (!buffer)
			{
				blDebugOutput("TiledMap: CSV buffer not allocated.");
				return;
			}

			unsigned int * bufferPtr = (unsigned int *)(buffer);
			memcpy(bufferPtr, gidTokens, _idx * 4);

			layer->_tiles = (unsigned int *)(buffer);

		}
		else if (tmxMapInfo->_layerAttribs & TMXLayerAttribNone)
		{
			tmxMapInfo->_xmlTileIndex = 0;
		}
	}
	else if (!strcmp(elementName , "map"))
	{
		// The map element has ended
		tmxMapInfo->_parentElement = (TMXPropertyNone);
	}
	else if (!strcmp(elementName , "layer"))
	{
		// The layer element has ended
		tmxMapInfo->_parentElement = (TMXPropertyNone);
	}
	else if (!strcmp(elementName , "objectgroup"))
	{
		// The objectgroup element has ended
		tmxMapInfo->_parentElement = (TMXPropertyNone);
	}
	else if (!strcmp(elementName , "object"))
	{
		// The object element has ended
		tmxMapInfo->_parentElement = (TMXPropertyNone);
	}
	else if (!strcmp(elementName , "tileset"))
	{
		tmxMapInfo->_recordFirstGID = TRUE;
	}
}