/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "UIMgr.h"
#include "Skin.h"
#include "ResMgr.h"
#include "CartoonBox.h"
namespace o2d{
	c_cartoonbox::c_cartoonbox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_CARTOONBOX , parent_, property_) , 
		m_gb(nullptr) ,
		m_tex(nullptr) , 
		m_framerate(12) , 
		m_last_record(0) , 
		m_cur_frame(0) , 
		m_loop(true) , 
		m_playing(false)
	{
		m_gb = new c_geometry_buffer(RO_TRIANGLE_STRIP);
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		c_geometry_buffer::s_vertex_element fmt1;
		fmt1.semantic = VES_POSITION;
		fmt1.index = 0;
		fmt1.type = VET_FLOAT2;
		fmts.push_back(fmt1);
		c_geometry_buffer::s_vertex_element fmt2;
		fmt2.semantic = VES_TEXTURE_COORD;
		fmt2.index = 0;
		fmt2.type = VET_FLOAT2;
		fmts.push_back(fmt2);
		c_geometry_buffer::s_vertex_element fmt3;
		fmt3.semantic = VES_DIFFUSE;
		fmt3.index = 0;
		fmt3.type = VET_FLOAT4;
		fmts.push_back(fmt3);
		m_gb->bind_vertex_buffer(BU_STREAM_DRAW , nullptr , 32*sizeof(f32) , fmts);
	}
	//--------------------------------------------------------
	c_cartoonbox::~c_cartoonbox()
	{
		delete m_gb;
		if(m_tex)
			c_resource_manager::get_singleton_ptr()->discard_resource(m_tex->get_filename());
	}
	//--------------------------------------------------------
	void c_cartoonbox::set_film(const c_string& film_)
	{
		m_tex = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(film_.c_str());
	}
	//--------------------------------------------------------
	void c_cartoonbox::set_frame(const c_vector<c_rect>& frames_)
	{
		m_frames.clear();
		FOREACH(TYPEOF(c_vector<c_rect>)::const_iterator , iter , frames_)
			m_frames.push_back(*iter);
	}
	//--------------------------------------------------------
	void c_cartoonbox::play()
	{
		m_playing = true;
		m_cur_frame = 0;
	}
	//--------------------------------------------------------
	void c_cartoonbox::stop()
	{
		m_playing = false;
	}
	//--------------------------------------------------------
	void c_cartoonbox::_redraw()
	{
		if(!m_visible)
			return;
		if(!m_playing)
			c_ui_manager::get_singleton_ptr()->cache_batch(m_tex, m_frames[0], m_absolute_rect, m_clipping_rect, m_flipx, m_flipy);
	}
	//--------------------------------------------------------
	void c_cartoonbox::_update(c_technique* uit_, u32 dt_)
	{
		if(!m_visible)
			return;
		if(m_playing&&!m_frames.empty())
		{
			f32 delta = 1000.f/(f32)m_framerate;
			c_rect tr = m_frames[m_cur_frame];
			f32 tw = m_tex->get_width();
			f32 th = m_tex->get_height();
			tr.lt_pt().x() /= tw;
			tr.rb_pt().x() /= tw;
			tr.lt_pt().y() /= th;
			tr.rb_pt().y() /= th;
			c_rect cv = m_absolute_rect;
			rect_clip(cv , m_clipping_rect);
			f32 dx1 , dx2, dy1 , dy2;
			dx1 = (cv.lt_pt().x() - m_absolute_rect.lt_pt().x())/m_absolute_rect.width();
			dx2 = (cv.rb_pt().x() - m_absolute_rect.rb_pt().x())/m_absolute_rect.width();
			dy1 = (cv.lt_pt().y() - m_absolute_rect.lt_pt().y())/m_absolute_rect.height();
			dy2 = (cv.rb_pt().y() - m_absolute_rect.rb_pt().y())/m_absolute_rect.height();
			tr.lt_pt().x() += dx1*tr.width();
			tr.lt_pt().y() += dy1*tr.height();
			tr.rb_pt().x() += dx2*tr.width();
			tr.rb_pt().y() += dy2*tr.height();
			c_stream data;
			data.resize(sizeof(f32)*4*8);
			data.write_f32(cv.lt_pt().x());
			data.write_f32(cv.lt_pt().y());
			data.write_f32(tr.lt_pt().x());
			data.write_f32(tr.lt_pt().y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(cv.rb_pt().x());
			data.write_f32(cv.lt_pt().y());
			data.write_f32(tr.rb_pt().x());
			data.write_f32(tr.lt_pt().y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(cv.lt_pt().x());
			data.write_f32(cv.rb_pt().y());
			data.write_f32(tr.lt_pt().x());
			data.write_f32(tr.rb_pt().y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(cv.rb_pt().x());
			data.write_f32(cv.rb_pt().y());
			data.write_f32(tr.rb_pt().x());
			data.write_f32(tr.rb_pt().y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			m_gb->uploadvb(0 , data.get_pointer()  , data.get_size());
			uit_->get_pass(0)->get_sampler(L"tex")->tex = m_tex;
			c_device::get_singleton_ptr()->draw(uit_ , m_gb);
			if(m_last_record >= delta)
			{
				if((m_cur_frame == m_frames.size() - 1)&&!m_loop)
				{
					m_playing = false;
					m_cur_frame = 0;
					_sig_play_over.emit();
					c_ui_manager::get_singleton_ptr()->set_dirty(true);
				}
				else if((m_cur_frame == m_frames.size() - 1)&&m_loop)
					m_cur_frame = 0;
				else
					m_cur_frame++;
				m_last_record = 0;
			}
			else
				m_last_record+=dt_;
		}
		c_widget::_redraw();
	}
}