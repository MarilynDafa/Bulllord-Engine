#pragma once
#include "BMGHeader.h"

//!param header
//!需要填写numberOfArrayElements , numberOfMipmapLevels
//!1D和Cube纹理需要填写pixelWidth；2D纹理还需要填写pixelHeight；3D纹理还需要填写pixelDepth

//!param data
//!需要填写CompressData

//!param tt
//!指定纹理类型

//!param at
//!如果该纹理图片没有alpha通道，或者alpha通道为0或255 at填写AT_SAMPLE_ALPHA ， 否则填写AT_COMPLEX_ALPHA

//!param filename
//!文件存储名

//!用完之后要自己释放Data_block中的CompressData
bool CompressKTX(BMG_header_TT& header, Data_block& data , std::vector<SpriteSheet> ssv, Texture_target tt , Alpha_type at , const char* filename,CompressMethord cm);

//!param filename
//!ktx纹理路径

//!prama data;
//!data为返回值
//!header为返回值
//!tt为返回值
bool UnCompressKTX(const char* filename , Data_block& data , std::vector<SpriteSheet>& ssv, BMG_header_TT& header, Texture_target& tt);

void encode_s3tc(const BMG_header_TT& header , Data_block& data , Texture_target tt, int at);
void decode_s3tc(BMG_header_TT& header,Data_block& data,Texture_target& tt, int at);

void encode_etc2(const BMG_header_TT& header , Data_block& data , Texture_target tt, int at);
void decode_etc2(BMG_header_TT& header,Data_block& data,Texture_target& tt, int at);

void encode_astc( const BMG_header_TT& header , Data_block& data , Texture_target tt , int at );
void decode_astc( BMG_header_TT& header,Data_block& data,Texture_target& tt, int at);


void encode_bmg(const BMG_header_TT& header, Data_block& data, Texture_target tt, int at);
void decode_bmg(BMG_header_TT& header, Data_block& data, Texture_target& tt, int at);

void startup();




