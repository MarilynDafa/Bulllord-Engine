/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Texture.h"
#include "ResMgr.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Font.h"
#include "DragDropSlot.h"
namespace o2d{
	c_dragdrop_slot::c_dragdrop_slot(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_DRAGDROPSLOT , parent_, property_) ,	
		m_clock_gb(nullptr) , 
		m_v_align(TA_V_BOTTOM) , 
		m_h_align(TA_H_RIGHT) , 
		m_cd(0) , 
		m_last_record(0) , 
		m_txt_color(0xFFFFFFFF) , 
		m_mask_tex(nullptr) , 
		m_in_cd(false) , 
		m_pressed(false) , 
		m_dragging(false)
	{
		m_icon.id = -1;
		m_icon.tex = nullptr;
		m_clock_gb = new c_geometry_buffer(RO_TRIANGLES);
		for(u32 i = 0 ; i < 7 ; ++i)
			m_triangle_gb[i] = new c_geometry_buffer(RO_TRIANGLES);
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
		m_clock_gb->bind_vertex_buffer(BU_STREAM_DRAW , nullptr , 24*sizeof(f32) , fmts);
		for(u32 i = 0 ; i < 7 ; ++i)
			m_triangle_gb[i]->bind_vertex_buffer(BU_STATIC_DRAW, nullptr , 24*sizeof(f32) , fmts);
	}
	//--------------------------------------------------------
	c_dragdrop_slot::~c_dragdrop_slot()
	{
		delete m_clock_gb;
		for(u32 i = 0 ; i < 7 ; ++i)
			delete m_triangle_gb[i];
		if(m_icon.tex)
			c_resource_manager::get_singleton_ptr()->discard_resource(m_icon.tex->get_filename());
		if(m_mask_tex)
			c_resource_manager::get_singleton_ptr()->discard_resource(m_mask_tex->get_filename());
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_text(const c_string& text_)
	{
		if(m_text != text_)
		{
			m_text = text_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_text_align(e_text_alignment ha_, e_text_alignment va_)
	{
		if(m_v_align != va_ || m_h_align != ha_)
		{
			m_v_align = va_;
			m_h_align = ha_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_bg_image(const c_string& name_)
	{
		if(m_bg_img != name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_mask_image(const c_string& mtex_)
	{
		if(m_mask_tex)
			c_resource_manager::get_singleton_ptr()->discard_resource(m_mask_tex->get_filename());
		m_mask_tex = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(mtex_.c_str() , false);
		if(m_mask_tex->get_width() != m_mask_tex->get_height())
			LOG(LL_WARNING, "the mask texture is not square");
		c_float2 eightpt[8];
		eightpt[0] = c_float2(m_absolute_rect.rb_pt().x(), m_absolute_rect.lt_pt().y());
		eightpt[1] = c_float2(m_absolute_rect.rb_pt().x(), 0.5f*(m_absolute_rect.lt_pt().y() + m_absolute_rect.rb_pt().y()));
		eightpt[2] = c_float2(m_absolute_rect.rb_pt().x(), m_absolute_rect.rb_pt().y());
		eightpt[3] = c_float2(0.5f*(m_absolute_rect.lt_pt().x() + m_absolute_rect.rb_pt().x()), m_absolute_rect.rb_pt().y());
		eightpt[4] = c_float2(m_absolute_rect.lt_pt().x(), m_absolute_rect.rb_pt().y());
		eightpt[5] = c_float2(m_absolute_rect.lt_pt().x(), 0.5f*(m_absolute_rect.lt_pt().y() + m_absolute_rect.rb_pt().y()));
		eightpt[6] = c_float2(m_absolute_rect.lt_pt().x(), m_absolute_rect.lt_pt().y());
		eightpt[7] = c_float2(0.5f*(m_absolute_rect.lt_pt().x() + m_absolute_rect.rb_pt().x()), m_absolute_rect.lt_pt().y());
		c_float2 eighttr[8];
		eighttr[0] = c_float2(1.f, 0.f);
		eighttr[1] = c_float2(1.f, 0.5f);
		eighttr[2] = c_float2(1.f, 1.f);
		eighttr[3] = c_float2(0.5f, 1.f);
		eighttr[4] = c_float2(0.f, 1.f);
		eighttr[5] = c_float2(0.f, 0.5f);
		eighttr[6] = c_float2(0.f, 0.f);
		eighttr[7] = c_float2(0.5f, 0.f);
		c_stream data;
		for(u32 i = 0 ;i < 7 ;++i)
		{
			data.reset(24*sizeof(f32));			
			data.write_f32(eightpt[i].x());
			data.write_f32(eightpt[i].y());
			data.write_f32(eighttr[i].x());
			data.write_f32(eighttr[i].y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);		
			data.write_f32(eightpt[i+1].x());
			data.write_f32(eightpt[i+1].y());
			data.write_f32(eighttr[i+1].x());
			data.write_f32(eighttr[i+1].y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(0.5f*(m_absolute_rect.lt_pt().x()+m_absolute_rect.rb_pt().x()));
			data.write_f32(0.5f*(m_absolute_rect.lt_pt().y()+m_absolute_rect.rb_pt().y()));
			data.write_f32(0.5f);
			data.write_f32(0.5f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(1.f);
			m_triangle_gb[i]->uploadvb(0, data.get_pointer() , data.get_size());			
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_cd(u32 ms_)
	{
		if(m_cd != ms_)
		{
			m_cd = ms_;
			m_last_record = 0;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::set_icon(u32 id_, const c_string& tex_)
	{
		if(m_icon.id != id_)
		{
			m_icon.id = id_;
			m_last_record = 0;
			m_icon.tex = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(tex_.c_str() , false);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::reset_icon()
	{
		if(m_icon.id != 0xFFFFFFFF)
		{
			m_icon.id = -1;
			m_last_record = 0;
			c_resource_manager::get_singleton_ptr()->discard_resource(m_icon.tex->get_filename());
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::cooldown()
	{
		if(!m_in_cd)
		{
			m_in_cd = true;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_on_touch_move()
	{
		if(m_pressed)
		{
			m_dragging = true;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_on_touch_start()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this &&!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
			return;
		}
		m_pressed = true;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_on_touch_finish()
	{
		m_pressed = false;
		if(!rect_contains(m_clipping_rect ,  c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
			return;
		}
		if(m_dragging)
		{
			m_dragging = false;
			c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
			_sig_release.emit(pos.x() , pos.y());
		}
		else
		{
			_sig_click.emit();
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_on_double_tap()
	{
		_sig_dtap.emit();
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_redraw()
	{
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img , m_absolute_rect , m_absolute_rect, m_flipx, m_flipy);
		if(m_icon.tex)
		{
			if(!m_dragging)
				c_ui_manager::get_singleton_ptr()->cache_batch(m_icon.tex, m_absolute_rect , m_absolute_rect, m_flipx, m_flipy);
			else
			{
				c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
				c_float2 vec = pos - m_absolute_rect.lt_pt();
				c_rect rc = m_absolute_rect;
				rc.lt_pt() += vec;
				rc.rb_pt() += vec;
				c_ui_manager::get_singleton_ptr()->cache_batch(m_icon.tex, rc , rc, m_flipx, m_flipy);
			}
		}
		c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid , m_fnt_height ,m_v_align|m_h_align , true, m_text , m_absolute_rect, m_clipping_rect, m_txt_color);
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_dragdrop_slot::_update(c_technique* uit_, u32 dt_)
	{ 
		if(!m_visible)
			return;
		if(!m_in_cd || !m_cd || !m_mask_tex)
		{
			c_widget::_update(uit_, dt_);
			return;
		}
		f32 theta = (TWO_PI/(f32)m_cd)*m_last_record;
		if(theta>TWO_PI)
		{
			m_in_cd = false;
			m_last_record = 0.f;
			c_widget::_update(uit_, dt_);
			return;
		}
		else
		{
			c_stream data;			
			c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
			if(!sk)
			{
				assert(0);
				return;
			}
			uit_->get_pass(0)->get_sampler(L"tex")->tex = m_mask_tex;
			switch(_calc_pt_list(m_absolute_rect , theta , data))
			{
			case 0:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				break;
			case 1:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				break;
			case 2:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				break;
			case 3:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[4]);
				break;
			case 4:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[4]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[3]);
				break;
			case 5:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[4]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[3]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[2]);
				break;
			case 6:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[4]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[3]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[2]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[1]);
				break;
			case 7:
				m_clock_gb->uploadvb(0, data.get_pointer(), data.get_size());
				c_device::get_singleton_ptr()->draw(uit_, m_clock_gb);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[6]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[5]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[4]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[3]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[2]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[1]);
				c_device::get_singleton_ptr()->draw(uit_, m_triangle_gb[0]);
				break;
			default:break;
			}
			m_last_record +=dt_;
		}
	}
	//--------------------------------------------------------
	s32 c_dragdrop_slot::_calc_pt_list(const c_rect& r_, f32 theta_, c_stream& data_)
	{
		f32 clr[4] = {1.f,1.f,1.f,1.f};
		if(theta_ >= 0 && theta_ < ONEFOURTH_PI)
		{
			f32 theta = theta_ - 0.f;
			data_.reset(24*sizeof(f32));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()) + r_.height()*0.5f*tan(theta));
			data_.write_f32(r_.lt_pt().y());
			data_.write_f32(0.5f + 0.5f*tan(theta));
			data_.write_f32(0.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.rb_pt().x());
			data_.write_f32(r_.lt_pt().y());
			data_.write_f32(1.f);
			data_.write_f32(0.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 7;
		}
		else if(theta_ >= ONEFOURTH_PI && theta_ < HALF_PI)
		{
			f32 theta = HALF_PI - theta_;
			data_.reset(24*sizeof(f32));
			data_.write_f32(r_.rb_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()) - r_.width()*0.5f*tan(theta));
			data_.write_f32(1.f);
			data_.write_f32(0.5f - 0.5f*tan(theta));
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.rb_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(1.f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 6;
		}
		else if(theta_ >= HALF_PI && theta_ < ONEFOURTH_PI*3)
		{
			f32 theta = theta_ - HALF_PI;
			data_.reset(24*sizeof(f32));
			data_.write_f32(r_.rb_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()) + r_.width()*0.5f*tan(theta));
			data_.write_f32(1.f);
			data_.write_f32(0.5f + 0.5f*tan(theta));
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.rb_pt().x());
			data_.write_f32(r_.rb_pt().y());
			data_.write_f32(1.f);
			data_.write_f32(1.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 5;
		}
		else if(theta_ >= ONEFOURTH_PI*3 && theta_ < PI)
		{
			f32 theta = PI - theta_;
			data_.reset(24*sizeof(f32));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()) + r_.height()*0.5f*tan(theta));
			data_.write_f32(r_.rb_pt().y());
			data_.write_f32(0.5f + 0.5f*tan(theta));
			data_.write_f32(1.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(r_.rb_pt().y());
			data_.write_f32(0.5f);
			data_.write_f32(1.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 4;
		}
		else if(theta_ >= PI && theta_ < 5*ONEFOURTH_PI)
		{
			f32 theta = theta_ - PI;
			data_.reset(24*sizeof(f32));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()) - r_.height()*0.5f*tan(theta));
			data_.write_f32(r_.rb_pt().y());
			data_.write_f32(0.5f - 0.5f*tan(theta));
			data_.write_f32(1.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.lt_pt().x());
			data_.write_f32(r_.rb_pt().y());
			data_.write_f32(0.f);
			data_.write_f32(1.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 3;
		}
		else if(theta_ >= 5*ONEFOURTH_PI && theta_ < 6*ONEFOURTH_PI)
		{
			f32 theta = 6*ONEFOURTH_PI - theta_;
			data_.reset(24*sizeof(f32));
			data_.write_f32(r_.lt_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()) + r_.width()*0.5f*tan(theta));
			data_.write_f32(0.f);
			data_.write_f32(0.5f + 0.5f*tan(theta));
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.lt_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 2;
		}
		else if(theta_ >= 6*ONEFOURTH_PI && theta_ < 7*ONEFOURTH_PI)
		{
			f32 theta = theta_ - 6*ONEFOURTH_PI;
			data_.reset(24*sizeof(f32));
			data_.write_f32(r_.lt_pt().x());
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y())- r_.width()*0.5f*tan(theta));
			data_.write_f32(0.f);
			data_.write_f32(0.5f - 0.5f*tan(theta));
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(r_.lt_pt().x());
			data_.write_f32(r_.lt_pt().y());
			data_.write_f32(0.f);
			data_.write_f32(0.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 1;
		}
		else
		{
			f32 theta = TWO_PI - theta_;
			data_.reset(24*sizeof(f32));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()) - r_.height()*0.5f*tan(theta));
			data_.write_f32(r_.lt_pt().y());
			data_.write_f32(0.5f - 0.5f*tan(theta));
			data_.write_f32(0.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(r_.lt_pt().y());
			data_.write_f32(0.5f);
			data_.write_f32(0.f);
			data_.write_buf(clr, sizeof(clr));
			data_.write_f32(0.5f*(r_.lt_pt().x() + r_.rb_pt().x()));
			data_.write_f32(0.5f*(r_.lt_pt().y() + r_.rb_pt().y()));
			data_.write_f32(0.5f);
			data_.write_f32(0.5f);
			data_.write_buf(clr, sizeof(clr));
			return 0;
		}
	}
}

