/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "FileMgr.h"
#include "Algorithm.h"
#include "ResMgr.h"
#include "Font.h"
#include "Color.h"
#include FT_LCD_FILTER_H
namespace o2d{
	namespace internal{
		static s32 _ft_usage_count = 0;
		static FT_Library _ft_lib;
		void _rasrer_callback(s32 y_ , s32 count_ , const FT_Span* const spans_ , void* const user_)
		{
			c_vector<c_font::s_span>* sptr = (c_vector<c_font::s_span>*)user_;
			for(s32 i = 0 ; i < count_ ; ++i)
			{
				c_font::s_span sp;
				sp.x = spans_[i].x;
				sp.y = y_;
				sp.width = spans_[i].len;
				sp.coverage = spans_[i].coverage;
				sptr->push_back(sp);
			}
		}
	}
	//--------------------------------------------------------
	c_font::c_font(const utf16* filename_)
		:c_resource(filename_ , RT_UI) , 
		m_outline_clr(0.f , 0.f , 0.f , 1.f) , 
		m_font_face(0) , 
		m_flags(0) , 
		m_id(0) ,
		m_active_height(-1) , 
		m_antialiased(true) 
	{
		if (!internal::_ft_usage_count++)
			FT_Init_FreeType(&internal::_ft_lib);
		m_code_range.push_back(MAKE_U32(32 , 32));
	}
	//--------------------------------------------------------
	c_font::~c_font()
	{
		if(!--internal::_ft_usage_count)
			FT_Done_FreeType(internal::_ft_lib);
	}
	//--------------------------------------------------------
	c_font::s_glyph_info* c_font::get_glyph(u32 c_)
	{
		AUTO(TYPEOF(c_map<u32 , s_fix_font*>)::iterator , iter , m_all_fix.find(m_active_height));
		if(iter == m_all_fix.end())
			return nullptr;
		s_fix_font* ff = iter->second();
		AUTO(TYPEOF(c_map<u32 , s_glyph_info*>)::iterator , iter2 , ff->glyphs.find(c_));
		if (iter2 == ff->glyphs.end())
		{
			s_glyph_info* gi = new s_glyph_info();
			memset(gi, 0, sizeof(*gi));
			gi->adv = 0;// m_font_face->glyph->metrics.horiAdvance * FT_POS_COEF;
			gi->valid = false;
			ff->glyphs.insert(c_, gi);
			_add_glyph(c_, gi, ff);
			gi->valid = true;
			return gi;
		}
		else
		{
			s_glyph_info* gi = iter2->second();
			if(gi->valid)
				return gi;
		}
	}
	//--------------------------------------------------------
	u32 c_font::get_text_extent(const c_string& str_, u16 flag)
	{
		u32 ret = 0;
		for(u32 i = 0; i < str_.get_length(); ++i ) 
		{
			utf16 c = str_[i];
			if(c == L'\r' || c == L'\n')
				continue;
			s_glyph_info* gi = get_glyph(MAKEU32(c, flag));
			if(!gi)
				continue;
			ret += gi->adv;
		}
		return ret;
	}
	//--------------------------------------------------------
	s32 c_font::get_char_index(const c_string& str_, s32 pixelx_)
	{
		s32 ret = 0;
		for(u32 i = 0; i < str_.get_length(); ++i) 
		{
			if(ret >= pixelx_)
				return (i == 0)?0:(i-1);
			utf16 c = str_[i];
			if(c == L'\r' || c == L'\n')
				continue;
			s_glyph_info* gi = get_glyph(MAKEU32(c, 0));
			if(!gi)
				continue;          
			ret += gi->adv;
		}
		if(ret >= pixelx_)
			return str_.get_length() - 1;
		else
			return -1;
	}
	//--------------------------------------------------------
	void c_font::text_break_word(c_string& text_, u32 width_, u16 flag)
	{
		u32 lineleftw = width_;
		for(u32 i = 0 ; i < text_.get_length() ; ++i)
		{
			utf16 ch = text_[i];
			if(ch == L'\n')
				lineleftw = width_;
			u32 adv;
			if(ch == L'\r' || ch == L'\n')
				adv = 0;
			else
				adv = get_glyph(MAKEU32(ch, flag))->adv;
			if(lineleftw < adv)
			{
				if(width_ < adv)
					break;
				lineleftw = width_;
				text_.insert(i , 1 , L'\n');
			}
			else
				lineleftw -= adv;
		}
	}
	//--------------------------------------------------------
	void c_font::measure_text(const c_string& text_, f32& outx_, f32& outy_, u16 flag6)
	{
		if(text_.is_empty())
		{
			outx_ = 0.f;
			outy_ = 0.f;
			return;
		}
		u16 flag = LOWU16(text_[0]);
		bool outline = LOWU16(flag) & 0x000F;
		bool bold = LOWU16(flag) & 0x00F0;
		bool shadow = LOWU16(flag) & 0x0F00;
		bool intly = LOWU16(flag) & 0xF000;
		outx_ = outy_ = 0;
		s32 linespace = 0;
		s32 lineheight = m_active_height;
		c_string str = text_;
		u32 start = 0, end = 0;
		c_string curstr;
		while(end < str.get_length())
		{
			if(start != 0) 
				outy_ += linespace;
			if((end = str.find_first_of(L'\n', start)) == NPOS)
				end = str.get_length();
			curstr = str.sub_str(start, end - start);
			start = end + 1;
			u32 adv = get_text_extent(curstr, flag);
			if(adv >= outx_)
				outx_ = adv;
			outy_ += lineheight ;
		}
		if (outline)
		{
			outx_ += 2;
			outy_ += 2;
		}
		else if (bold)
		{
			outx_ += 1;
			outy_ += 1;
		}
		else if (shadow)
		{
			outy_ += 1;
		}
	}
	//--------------------------------------------------------
	bool c_font::active_height(u32 height_)
	{
		bool find = false;
		FOREACH(TYPEOF(c_vector<u32>)::iterator, iter, m_all_size)
		{
			if (*iter == height_)
			{
				find = true;
				break;
			}
		}
		if (!find)
			serialize_height(height_);
		m_active_height = height_;
		FT_Set_Char_Size(m_font_face, height_ * 64, 0, 72 * 64, 72);
		return true;
	}
	//--------------------------------------------------------
	bool c_font::serialize_height(u32 height_)
	{
		s_fix_font* ff;
		bool find = false;
		FOREACH(TYPEOF(c_vector<u32>)::iterator , iter , m_all_size)
		{
			if (*iter == height_)
			{
				find = true;
				break;
			}
		}
		if (!find)
		{
			m_all_size.push_back(height_);
			ff = new s_fix_font();
			memset(ff, 0, sizeof(*ff));
			m_all_fix.insert(height_, ff);

		}
		FT_Set_Char_Size(m_font_face, height_ * 64, 0, 72 * 64, 72);
		/*
		{
			f32 sz72 = (height_ * 72.0f) / 96.0;
			f32 bestdelta = 99999.0;
			f32 bestsz = 0;
			for(s32 i = 0; i < m_font_face->num_fixed_sizes; i++)
			{
				f32 size = m_font_face->available_sizes [i].size * f32(FT_POS_COEF);
				f32 delta = fabs(size - sz72);
				if(delta < bestdelta)
				{
					bestdelta = delta;
					bestsz = size;
				}
			}
			if((bestsz <= 0) || FT_Set_Char_Size(m_font_face, 0, FT_F26Dot6(bestsz * 64), 0, 0))
			{
				FT_Done_Face(m_font_face);
				return false;
			}
		}*/
		
		return true;
	}
	//--------------------------------------------------------
	bool c_font::_load_impl()
	{
		c_stream out;
		if(!c_file_manager::get_singleton_ptr()->open_file(m_filename , out))
			return false;
		m_source.reset(out.get_size());
		out.read_buf(m_source.get_pointer() , m_source.get_size());
		if(FT_New_Memory_Face(internal::_ft_lib, (FT_Bytes)m_source.get_pointer(), m_source.get_size(), 0,&m_font_face))
			return false;
		FT_Select_Charmap(m_font_face, FT_ENCODING_UNICODE);
/*
		FT_Library_SetLcdFilter(internal::_ft_lib, FT_LCD_FILTER_LIGHT);
		unsigned char lcd_weights[5];
		lcd_weights[0] = 0x10;
		lcd_weights[1] = 0x40;
		lcd_weights[2] = 0x70;
		lcd_weights[3] = 0x40;
		lcd_weights[4] = 0x10;
		FT_Library_SetLcdFilterWeights(internal::_ft_lib, lcd_weights);*/
		if(!m_font_face->charmap)
		{
			FT_Done_Face(m_font_face);
			m_font_face = 0;
			return false;
		} 
		m_flags = FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT;
		c_string tmp = m_filename;
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmp.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		m_id = hash(tmp3.front().c_str());
		return true;
	}
	//--------------------------------------------------------
	void c_font::_unload_impl()
	{
		FOREACH(TYPEOF(c_map<u32 , s_fix_font*>)::iterator , iter , m_all_fix)
		{
			FOREACH(TYPEOF(c_vector<c_tex_loader*>)::iterator , iter2 , iter->second()->loaders)
			{
				c_resource_manager::get_singleton_ptr()->discard_resource(*iter2);
				delete *iter2;
			}
			FOREACH(TYPEOF(c_map<u32, s_glyph_info*>)::iterator , iter3 , iter->second()->glyphs)
				delete iter3->second();
			delete iter->second();
		}
		FT_Done_Face(m_font_face);
	}
	//--------------------------------------------------------
	bool c_font::_add_glyph(u32 c_, s_glyph_info* gi_ , s_fix_font* ff_)
	{
		
		if(!ff_->cur_tex)
		{
			c_tex_loader* loader = new c_tex_loader(false , FONT_TEXTURE_SIZE , FONT_TEXTURE_SIZE);
			ff_->cur_tex = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(loader);
			ff_->cur_tex->set_filter(FO_POINT, FO_POINT);
			ff_->texs.push_back(ff_->cur_tex);
			ff_->loaders.push_back(loader);
			ff_->record_x = INTER_GLYPH_PAD_SPACE;
			ff_->record_y = INTER_GLYPH_PAD_SPACE;
			ff_->yb = INTER_GLYPH_PAD_SPACE;
		}
		bool outline = LOWU16(c_) & 0x000F;
		bool bold = LOWU16(c_) & 0x00F0;
		bool shadow = LOWU16(c_) & 0x0F00;
		bool intly = LOWU16(c_) & 0xF000;
		utf16 cc = HIGU16(c_);

		if (intly)
		{
			float lean = 0.15f;
			FT_Matrix matrix;
			matrix.xx = 1.0 / 64.0 * 0x10000L;
			matrix.xy = lean * 0x10000L;
			matrix.yx = 0;
			matrix.yy = 0x10000L;
			FT_Set_Transform(m_font_face, &matrix, 0);
		}
		else
		{
			FT_Matrix matrix;
			matrix.xx = 1.0/64.0*0x10000L;
			matrix.xy = 0;
			matrix.yx = 0;
			matrix.yy = 0x10000L;
			FT_Set_Transform(m_font_face, &matrix, 0);
		}
		if (FT_Load_Char(m_font_face, cc,  m_flags))
			return false;
		gi_->adv = m_font_face->glyph->advance.x * FT_POS_COEF;
#if 0
		c_rect rect;
		c_vector<s_span> spans;
		c_vector<s_span> outlinespans;
		if(outline)
		{
			FT_Raster_Params params;
			memset(&params , 0 , sizeof(params));
			params.flags = FT_RASTER_FLAG_AA|FT_RASTER_FLAG_DIRECT;
			params.gray_spans = internal::_rasrer_callback;
			params.user = &spans;
			FT_Outline_Render(internal::_ft_lib , &m_font_face->glyph->outline , &params);
			FT_Stroker stroker;
			FT_Stroker_New(internal::_ft_lib , &stroker);
			FT_Stroker_Set(stroker , 0.7*64 , FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
			FT_Glyph glyph;
			if(FT_Get_Glyph(m_font_face->glyph , &glyph) == 0)
			{
				FT_Glyph_StrokeBorder(&glyph , stroker , 0 , 1);
				if(glyph->format == FT_GLYPH_FORMAT_OUTLINE)
				{
					FT_Outline* o = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;				
					memset(&params , 0 , sizeof(params));
					params.flags = FT_RASTER_FLAG_AA|FT_RASTER_FLAG_DIRECT;
					params.gray_spans = internal::_rasrer_callback;
					params.user = &outlinespans;
					FT_Outline_Render(internal::_ft_lib , o , &params);
				}
			}
			FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
			gi_->offset.x() = ((FT_BitmapGlyph)glyph)->left;
			gi_->offset.y() = ((FT_BitmapGlyph)glyph)->top;
			FT_Stroker_Done(stroker);
			FT_Done_Glyph(glyph);
			if(!spans.empty())
			{
				rect = c_rect(spans.front().x , spans.front().y , spans.front().x , spans.front().y);
				for(c_vector<s_span>::iterator s = spans.begin() ; s!=spans.end() ; ++s)
				{
					rect_extend(rect , c_float2(s->x , s->y));
					rect_extend(rect , c_float2(s->x + s->width - 1, s->y));
				}
				for(c_vector<s_span>::iterator s = outlinespans.begin() ; s!=outlinespans.end() ; ++s)
				{
					rect_extend(rect , c_float2(s->x , s->y));
					rect_extend(rect , c_float2(s->x + s->width - 1, s->y));
				}
			}
		}
#endif
		u32 glyphw = m_font_face->glyph->bitmap.width;
		u32 glyphh = m_font_face->glyph->bitmap.rows;
		glyphw+=INTER_GLYPH_PAD_SPACE;
		glyphh+=INTER_GLYPH_PAD_SPACE;
		u32 xnext = ff_->record_x + glyphw;
		if(xnext > FONT_TEXTURE_SIZE)
		{
			ff_->record_x = INTER_GLYPH_PAD_SPACE;
			xnext = ff_->record_x + glyphw;
			ff_->record_y = ff_->yb;
		}
		u32 ybot = ff_->record_y + glyphh + (shadow ? 1 : 0);
		while(ybot > FONT_TEXTURE_SIZE)
		{
			c_tex_loader* loader = new c_tex_loader(false , FONT_TEXTURE_SIZE , FONT_TEXTURE_SIZE);
			ff_->cur_tex = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(loader);
			ff_->cur_tex->set_filter(FO_POINT, FO_POINT);
			ff_->texs.push_back(ff_->cur_tex);
			ff_->loaders.push_back(loader);
			ff_->record_x = INTER_GLYPH_PAD_SPACE;
			ff_->record_y = INTER_GLYPH_PAD_SPACE;
			ff_->yb = INTER_GLYPH_PAD_SPACE;
			xnext = ff_->record_x + glyphw;
			if(xnext > FONT_TEXTURE_SIZE)
			{
				ff_->record_x = INTER_GLYPH_PAD_SPACE;
				xnext = ff_->record_x + glyphw;
				ff_->record_y = ff_->yb;
			}
			ybot = ff_->record_y + glyphh;
		}
		gi_->rect.lt_pt().x() = ff_->record_x;
		gi_->rect.lt_pt().y() = ff_->record_y;
		gi_->offset.x() = m_font_face->glyph->bitmap_left;
		gi_->offset.y() = -m_font_face->glyph->metrics.horiBearingY * FT_POS_COEF + m_active_height * 72.0 / 96.0 + 1;
		gi_->rect.rb_pt().x() = ff_->record_x + glyphw - INTER_GLYPH_PAD_SPACE + 0;
		gi_->rect.rb_pt().y() = ff_->record_y + glyphh - INTER_GLYPH_PAD_SPACE + 0;
		gi_->tex = ff_->cur_tex;        
		u32* buf;
		u32 gw , gh;
#if 0
		if(outline)
		{
			buf = (u32*)malloc((rect.width()+1) * (rect.height()+1)*sizeof(u32));
			memset(buf , 0 , (rect.width()+1) * (rect.height()+1)*sizeof(u32));
			_draw_glyph_2_buffer(buf, rect, spans , outlinespans);  
			gw = rect.width()+1;
			gh = rect.height()+1;
		}
		else
#endif
		{
			int imgw, imgh;
			imgw = m_font_face->glyph->bitmap.width;
			imgh = m_font_face->glyph->bitmap.rows;
			if (outline)
			{
				imgw += 2;
				imgh += 2;
				gi_->rect.rb_pt().x() +=  2;
				gi_->rect.rb_pt().y() +=  2;
				gi_->offset.x() -= 1;
				gi_->offset.y() -= 1;
			}
			else if (bold)
			{
				imgw++; imgh++;
				gi_->rect.rb_pt().x() += 1;
				gi_->rect.rb_pt().y() += 1;
			}
			else if (shadow)
			{
				imgh++;
				gi_->rect.rb_pt().y() += 1;
			}
			buf = (u32*)malloc(imgw * imgh*sizeof(u32));
			memset(buf , 0 , imgw * imgh *sizeof(u32));
			_draw_glyph_2_buffer(buf, imgw, shadow, bold, outline);
			gw = imgw;
			gh = imgh;
		}
		ff_->cur_tex->upload(ff_->record_x , ff_->record_y , 0 , gw , gh , buf , gw*gh*sizeof(u32));
		free(buf);
		ff_->record_x = xnext;
		if(ybot > ff_->yb)
			ff_->yb = ybot;
		return true;
	}
	//--------------------------------------------------------
	void c_font::_draw_glyph_2_buffer(u32* buffer_, const c_rect& rc_, const c_vector<s_span>& sp_, const c_vector<s_span>& osp_) const
	{
		s32 height = rc_.height() + 1;
		s32 width = rc_.width() + 1;
		for(c_vector<s_span>::const_iterator s = osp_.begin() ; s != osp_.end() ; ++s)
		{
			for(s32 w = 0 ; w < s->width ; ++w)
			{
				u32 idx = (u32)((height - 1 - (s->y - rc_.lt_pt().y())) * width + s->x -  rc_.lt_pt().x() + w);
				buffer_[idx] = c_color(m_outline_clr.r() , m_outline_clr.g() , m_outline_clr.b() , f32(s->coverage)/255.f).rgba();
			}
		}
 		for(c_vector<s_span>::const_iterator s = sp_.begin() ; s!=sp_.end() ;++s)
 		{
 			for(s32 w = 0 ; w < s->width ; ++w)
 			{
 				u32& dst = buffer_[s32((height - 1 - (s->y - rc_.lt_pt().y()))*width + s->x - rc_.lt_pt().x() + w)];
 				c_color src = c_color(1.f , 1.f , 1.f , f32(s->coverage)/255.f).rgba();
 				c_color newclr(dst);
 				newclr.r() = newclr.r() + (src.r() - newclr.r())*src.a();
 				newclr.g() = newclr.g() + (src.r() - newclr.g())*src.a();
 				newclr.b() = newclr.b() + (src.r() - newclr.b())*src.a();
 				newclr.a() = MIN_VALUE(1.f , newclr.a() + src.a());
 				dst = newclr.rgba();
 			}
 		}
	}
	//--------------------------------------------------------
	void c_font::_draw_glyph_2_buffer(u32* buffer_, u32 bufsz_, bool shadow, bool bold, bool outline) const
	{
		FT_Bitmap* bitmap = &m_font_face->glyph->bitmap;
		if (outline)
		{
			u32 dsttmp;
			u32 srctmp;
			u32* tmpbufLT = buffer_;
			u32* tmpbufT = buffer_ +1;
			u32* tmpbufRT = buffer_ + 2;
			u32* tmpbufL = buffer_ + bufsz_;
			u32* tmpbuf = buffer_+ bufsz_ + 1;
			u32* tmpbufR = buffer_ + bufsz_ + 2;
			u32* tmpbufLB = buffer_ + 2 * bufsz_ + 0;
			u32* tmpbufB = buffer_ + 2 * bufsz_ + 1;
			u32* tmpbufRB = buffer_ + 2 * bufsz_ + 2;
			for (s32 i = 0; i < bitmap->rows; ++i)
			{
				u8* src = bitmap->buffer + ((i)* bitmap->pitch);
				{
					u8* dst = (u8*)(tmpbuf);
					u8* dstLT = (u8*)(tmpbufLT);
					u8* dstT = (u8*)(tmpbufT);
					u8* dstRT = (u8*)(tmpbufRT);
					u8* dstL = (u8*)(tmpbufL);
					u8* dstR = (u8*)(tmpbufR);
					u8* dstLB = (u8*)(tmpbufLB);
					u8* dstB = (u8*)(tmpbufB);
					u8* dstRB = (u8*)(tmpbufRB);
					for (s32 j = 0; j < bitmap->width; ++j)
					{
						*dstLT++ += 0x00;
						*dstT++ += 0x00;
						*dstRT++ += 0x00;
						*dstL++ += 0x00;
						*dstR++ += 0x00;
						*dstLB++ += 0x00;
						*dstB++ += 0x00;
						*dstRB++ += 0x00;
						srctmp = *src;
						*dst++ = MIN_VALUE(255, *dst + (0xFF)  * (srctmp / 255.0));
						*dstLT++ += 0x00;
						*dstT++ += 0x00;
						*dstRT++ += 0x00;
						*dstL++ += 0x00;
						*dstR++ += 0x00;
						*dstLB++ += 0x00;
						*dstB++ += 0x00;
						*dstRB++ += 0x00;
						*dst++ = MIN_VALUE(255, *dst + (0xFF) * (srctmp / 255.0));
						*dstLT++ += 0x00;
						*dstT++ += 0x00;
						*dstRT++ += 0x00;
						*dstL++ += 0x00;
						*dstR++ += 0x00;
						*dstLB++ += 0x00;
						*dstB++ += 0x00;
						*dstRB++ += 0x00;
						*dst++ = MIN_VALUE(255, *dst + (0xFF) * (srctmp / 255.0));
						*dstLT++ = MIN_VALUE(*dstLT + (*src)*0.5, 255);
						*dstT++ = MIN_VALUE(*dstT + (*src)*0.5, 255);
						*dstRT++ = MIN_VALUE(*dstRT + (*src)*0.5, 255);
						*dstL++ = MIN_VALUE(*dstL + (*src)*0.5, 255);
						*dstR++ = MIN_VALUE(*dstR + (*src)*0.5, 255);
						*dstLB++ = MIN_VALUE(*dstLB + (*src)*0.5, 255);
						*dstB++ = MIN_VALUE(*dstB + (*src)*0.5, 255);
						*dstRB++ = MIN_VALUE(*dstRB + (*src)*0.5, 255);
						dsttmp = *(dst);
						*dst++ = MIN_VALUE(255, dsttmp + srctmp);
						src++;
					}
				}
				tmpbufLT += bufsz_;
				tmpbufT += bufsz_;
				tmpbufL += bufsz_;
				tmpbuf += bufsz_;
				tmpbufR += bufsz_;
				tmpbufRT += bufsz_;
				tmpbufLB += bufsz_;
				tmpbufB += bufsz_;
				tmpbufRB += bufsz_;
			}
		}
		else if (bold)
		{
			u32* tmpbuf = buffer_;
			u32 dsttmp;
			u32 srctmp;
			u32* tmpbuf2 = buffer_ + 1;
			u32* tmpbuf3 = buffer_ + bufsz_;
			u32* tmpbuf4 = buffer_ + bufsz_ + 1;
			for (s32 i = 0; i < bitmap->rows; ++i)
			{
				u8* src = bitmap->buffer + ((i)* bitmap->pitch);
				{
					u8* dst = (u8*)(tmpbuf);
					u8* dst2 = (u8*)(tmpbuf2);
					u8* dst3 = (u8*)(tmpbuf3);
					u8* dst4 = (u8*)(tmpbuf4);
					for (s32 j = 0; j < bitmap->width; ++j)
					{
						*dst2++ = 0xFF;
						*dst3++ = 0xFF;
						*dst4++ = 0xFF;
						srctmp = *src;
						*dst++ = MIN_VALUE(255, *dst  + (0xFF)  * (srctmp / 255.0));
						*dst2++ = 0xFF;
						*dst3++ = 0xFF;
						*dst4++ = 0xFF;
						*dst++ = MIN_VALUE(255, *dst + (0xFF) * (srctmp / 255.0));
						*dst2++ = 0xFF;
						*dst3++ = 0xFF;
						*dst4++ = 0xFF;
						*dst++ = MIN_VALUE(255, *dst + (0xFF) * (srctmp / 255.0));
						*dst2++ = MIN_VALUE((*src)*0.35, 255);
						*dst3++ = MIN_VALUE((*src)*0.35, 255);
						*dst4++ = MIN_VALUE((*src)*0.35, 255);
						dsttmp = *(dst);
						*dst++ = MIN_VALUE(255, dsttmp + srctmp);
						src++;
					}
				}
				tmpbuf += bufsz_;
				tmpbuf2 += bufsz_;
				tmpbuf3 += bufsz_;
				tmpbuf4 += bufsz_;
			}
		}
		else if (shadow)
		{
			u32* tmpbuf = buffer_;
			u32 dsttmp;
			u32 srctmp;
			u32* tmpbuf2 = buffer_  + bitmap->pitch;
			u8 shadowclr = 0x40;
			for (s32 i = 0; i < bitmap->rows; ++i)
			{
				u8* src = bitmap->buffer + ((i) * bitmap->pitch);
				{
					u8* dst = (u8*)(tmpbuf);
					u8* dst2 = (u8*)(tmpbuf2);
					for (s32 j = 0; j < bitmap->width; ++j)
					{
						*dst2++ = shadowclr;
						srctmp = *src;
						*dst++ = MIN_VALUE(255, shadowclr + (0xFF) * (srctmp / 255.0));
						*dst2++ = shadowclr;
						srctmp = *src;
						*dst++ = MIN_VALUE(255, shadowclr + (0xFF) * (srctmp / 255.0));
						*dst2++ = shadowclr;
						srctmp = *src;
						*dst++ = MIN_VALUE(255, shadowclr + (0xFF) * (srctmp / 255.0));
						*dst2++ = 0.5 * (*src++);
						dsttmp = *dst;
						*dst++ = MIN_VALUE(255, dsttmp + srctmp);
					}
				}
				tmpbuf += bufsz_;
				tmpbuf2 += bufsz_;
			}			
		}
		else
		{
			for (s32 i = 0; i < bitmap->rows; ++i)
			{
				u8* src = bitmap->buffer + (i * bitmap->pitch);
				{
					u8* dst = (u8*)(buffer_);
					for (s32 j = 0; j < bitmap->width; ++j)
					{
						*dst++ = 0xFF;
						*dst++ = 0xFF;
						*dst++ = 0xFF;
						*dst++ = *src++;
					}
				}
				buffer_ += bufsz_;
			}
		}
	}
}