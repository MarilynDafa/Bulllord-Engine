/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "UIMgr.h"
#include "Label.h"
namespace o2d{
	c_label::c_label(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_LABEL, parent_, property_) ,
		m_bg_zoom_region(property_.dim) , 
		m_v_align(TA_V_TOP) , 
		m_h_align(TA_H_LEFT) ,
		m_txt_color(0xFFFFFFFF) ,
		m_start_angle(0),
		m_end_angle(360),
		m_wordwrap(false),
		m_cut_angle(false),
		m_clock_wise(false)
	{
		m_start_angle = 0;
		m_end_angle = 270;
	}
	//--------------------------------------------------------
	c_label::~c_label(){}
	//--------------------------------------------------------
	void c_label::set_wordwrap(bool wrap_)
	{
		if(m_wordwrap!=wrap_)
		{
			m_wordwrap = wrap_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_text(const c_string& text_)
	{
		if(m_text != text_)
		{
			m_text = text_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_text_align(e_text_alignment ha_, e_text_alignment va_)
	{
		if(m_v_align != va_ || m_h_align != ha_)
		{
			m_v_align = va_;
			m_h_align = ha_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_margin(const c_float2& margin_)
	{
		if(m_margin.x()!=margin_.x() || m_margin.y()!= margin_.y())
		{
			m_margin.x() = margin_.x();
			m_margin.y() = margin_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_bg_image(const c_string& name_)
	{
		if(m_bg_img!= name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_bg_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_bg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_bg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_bg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_bg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_bg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_bg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_bg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_bg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_bg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_bg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_bg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_label::set_start_angle(int angle)
	{
		if (m_start_angle != angle)
		{
			m_start_angle = angle;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_end_angle(int angle)
	{
		if (m_end_angle != angle)
		{
			m_end_angle = angle;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_label::set_stencil_image(const c_string& img_)
	{
		if (img_.get_length() == 0)
			return;
		m_stencil_img = img_;
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_stencil_img);
			f32 w = sk->get_skin_tex()->get_width();
			f32 h = sk->get_skin_tex()->get_height();
			m_stencil_rect.lt_pt().x() = tc.lt_pt().x() / w;
			m_stencil_rect.lt_pt().y() = tc.lt_pt().y() / h;
			m_stencil_rect.rb_pt().x() = tc.rb_pt().x() / w;
			m_stencil_rect.rb_pt().y() = tc.rb_pt().y() / h;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_label::set_common_image(const c_string& img_)
	{
		if (img_.get_length() == 0)
			return;
		m_bg_img = img_;
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_bg_img);
			f32 w = sk->get_skin_tex()->get_width();
			f32 h = sk->get_skin_tex()->get_height();
			m_commmon_rect.lt_pt().x() = tc.lt_pt().x() / w;
			m_commmon_rect.lt_pt().y() = tc.lt_pt().y() / h;
			m_commmon_rect.rb_pt().x() = tc.rb_pt().x() / w;
			m_commmon_rect.rb_pt().y() = tc.rb_pt().y() / h;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_label::set_clock_wise(bool flag)
	{
		m_clock_wise = flag;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_label::set_cutAngle(bool falg)
	{
		m_cut_angle = falg;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_label::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_label::xiangxian(int angle, c_float2& pt, int& xx, bool& addpt, c_float2& texpt)
	{
		c_float2 center = (m_absolute_rect.lt_pt() + m_absolute_rect.rb_pt()) * 0.5;
		c_float2 centertex = (m_commmon_rect.lt_pt() + m_commmon_rect.rb_pt()) * 0.5;
		c_float2 lt(m_absolute_rect.lt_pt());
		c_float2 mt(center.x(), m_absolute_rect.lt_pt().y());
		c_float2 rt(m_absolute_rect.rb_pt().x(), m_absolute_rect.lt_pt().y());
		c_float2 lm(m_absolute_rect.lt_pt().x(), center.y());
		c_float2 rm(m_absolute_rect.rb_pt().x(), center.y());
		c_float2 lb(m_absolute_rect.lt_pt().x(), m_absolute_rect.rb_pt().y());
		c_float2 mb(center.x(), m_absolute_rect.rb_pt().y());
		c_float2 rb(m_absolute_rect.rb_pt().x(), m_absolute_rect.rb_pt().y());

		c_float2 texlt(m_commmon_rect.lt_pt());
		c_float2 texmt(centertex.x(), m_commmon_rect.lt_pt().y());
		c_float2 texrt(m_commmon_rect.rb_pt().x(), m_commmon_rect.lt_pt().y());
		c_float2 texlm(m_commmon_rect.lt_pt().x(), centertex.y());
		c_float2 texrm(m_commmon_rect.rb_pt().x(), centertex.y());
		c_float2 texlb(m_commmon_rect.lt_pt().x(), m_commmon_rect.rb_pt().y());
		c_float2 texmb(centertex.x(), m_commmon_rect.rb_pt().y());
		c_float2 texrb(m_commmon_rect.rb_pt());

		float w = 0.5* (m_absolute_rect.rb_pt().x() - m_absolute_rect.lt_pt().x());
		float h = 0.5* (m_absolute_rect.rb_pt().y() - m_absolute_rect.lt_pt().y());
		float texw = 0.5* (m_commmon_rect.rb_pt().x() - m_commmon_rect.lt_pt().x());
		float texh = 0.5* (m_commmon_rect.rb_pt().y() - m_commmon_rect.lt_pt().y());

		int ang45 = round(atan((double)w/ (double)h) * RAD_2_DEG);
		addpt = true;
		if (angle == 0)
		{
			pt = mt;
			texpt = texmt;
			xx = 1;
			addpt = false;
		}
		else if (angle > 0 && angle < ang45)
		{
			pt = mt;
			texpt = texmt;
			pt.x() += tan(DEG_2_RAD * angle) * h;
			texpt.x()  += (pt.x() - center.x()) / w *texw;
			xx = 1;
		}
		else if (angle == ang45)
		{
			pt = rt;
			texpt = texrt;
			xx = 1;
			addpt = false;
		}
		else if (angle > ang45 && angle < 90)
		{
			pt = rm;
			texpt = texrm;
			pt.y() -= tan(HALF_PI - DEG_2_RAD * angle) * w;
			texpt.y() -= (-pt.y() + center.y()) / h *texh;
			xx = 2;
		}
		else if (angle == 90)
		{
			pt = rm;
			texpt = texrm;
			xx = 2;
			addpt = false;
		}
		else if (angle > 90 && angle < 180 - ang45)
		{
			pt = rm;
			texpt = texrm;
			pt.y() += tan( DEG_2_RAD * angle - HALF_PI) * w;
			texpt.y() += (pt.y() - center.y()) / h *texh;
			xx = 3;
		}
		else if (angle == 180- ang45)
		{
			pt = rb;
			texpt = texrb;
			xx =  3;
			addpt = false;
		}
		else if (angle > 180- ang45 && angle < 180)
		{
			pt = mb;
			texpt = texmb;
			pt.x() += tan(PI - DEG_2_RAD * angle) * h;
			texpt.x() += (pt.x() - center.x()) / w *texw;
			xx = 4;
		}
		else if (angle == 180)
		{
			pt = mb;
			texpt = texmb;
			xx = 4;
			addpt = false;
		}
		else if (angle > 180 && angle < 180+ ang45)
		{
			pt = mb;
			texpt = texmb;
			pt.x() -= tan( DEG_2_RAD * angle - PI) * h;
			texpt.x() -= (-pt.x() + center.x()) / w *texw;
			xx = 5;
		}
		else if (angle == 180+ ang45)
		{
			pt = lb;
			texpt = texlb;
			xx = 5;
			addpt = false;
		}
		else if (angle > 180+ ang45 && angle < 270)
		{
			pt = lm;
			pt.y() += tan(6 * ONEFOURTH_PI - DEG_2_RAD * angle) * w;
			texpt = texlm;
			texpt.y() += (pt.y() - center.y()) / h *texh;
			xx = 6;
		}
		else if (angle == 270)
		{
			pt = lm;
			texpt = texlm;
			xx =  6;
			addpt = false;
		}
		else if (angle > 270 && angle < 360- ang45)
		{
			pt = lm;
			pt.y() -= tan(DEG_2_RAD * angle - 6 * ONEFOURTH_PI) * w;
			texpt = texlm;
			texpt.y() -= (-pt.y() + center.y()) / h *texh;
			xx = 7;
		}
		else if (angle == 360- ang45)
		{
			pt = lt;
			texpt = texlt;
			xx =  7;
			addpt = false;
		}
		else if (angle > 360- ang45 && angle < 360)
		{
			pt = mt;
			texpt = texmt;
			pt.x() -= tan(TWO_PI - DEG_2_RAD * angle) * h;
			texpt.x() -= (-pt.x() + center.x()) / w *texw;
			xx = 8;
		}
		else 
		{
			pt = mt;
			texpt = texmt;
			xx =  8;
			addpt = false;
		}
	}

	//--------------------------------------------------------
	void c_label::_redraw()
	{
		if(!m_visible)
			return;
		if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
		{
			c_rect tr;
			tr.lt_pt() = m_commmon_rect.lt_pt();
			tr.rb_pt() = m_commmon_rect.rb_pt();
			c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, false, false);
		}
		else
		{
			if (m_cut_angle)
			{
				m_triangle_gb = new c_geometry_buffer(RO_TRIANGLE_FAN);

				c_float2 spt, ept, stexpt, etexpt;
				int sxx, exx;
				bool sadd, eadd;
				float start = (m_start_angle + 360) % 360;
				float end = (m_end_angle + 360) % 360;
				xiangxian(start, spt, sxx, sadd, stexpt);
				xiangxian(end, ept, exx, eadd, etexpt);

				int dif;
				if(exx - sxx >= 0)
					dif = exx - sxx;
				else
					dif = 8 - (sxx - exx);
				int num = dif;
				if (!m_clock_wise)
					num = 8 - dif;
			

				c_stream data_;
				data_.reset(8 * sizeof(f32) * (3+ num));
				c_float2 center = (m_absolute_rect.lt_pt() + m_absolute_rect.rb_pt()) * 0.5;
				c_float2 centertex = (m_commmon_rect.lt_pt() + m_commmon_rect.rb_pt()) * 0.5;

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
				c_float2 lt(m_absolute_rect.lt_pt());
				c_float2 mt(center.x(), m_absolute_rect.lt_pt().y());
				c_float2 rt(m_absolute_rect.rb_pt().x(), m_absolute_rect.lt_pt().y());
				c_float2 lm(m_absolute_rect.lt_pt().x(), center.y());
				c_float2 rm(m_absolute_rect.rb_pt().x(), center.y());
				c_float2 lb(m_absolute_rect.lt_pt().x(), m_absolute_rect.rb_pt().y());
				c_float2 mb(center.x(), m_absolute_rect.rb_pt().y());
				c_float2 rb(m_absolute_rect.rb_pt().x(), m_absolute_rect.rb_pt().y());


				c_float2 texlt(m_commmon_rect.lt_pt());
				c_float2 texmt(centertex.x(), m_commmon_rect.lt_pt().y());
				c_float2 texrt(m_commmon_rect.rb_pt().x(), m_commmon_rect.lt_pt().y());
				c_float2 texlm(m_commmon_rect.lt_pt().x(), centertex.y());
				c_float2 texrm(m_commmon_rect.rb_pt().x(), centertex.y());
				c_float2 texlb(m_commmon_rect.lt_pt().x(), m_commmon_rect.rb_pt().y());
				c_float2 texmb(centertex.x(), m_commmon_rect.rb_pt().y());
				c_float2 texrb(m_commmon_rect.rb_pt());



				c_float2 cc[8] = { mt, rt, rm, rb, mb, lb, lm, lt };
				c_float2 cctex[8] = { texmt, texrt, texrm, texrb, texmb, texlb, texlm, texlt };

				if (m_clock_wise)
				{
					//center
					data_.write_f32(center.x());
					data_.write_f32(center.y());
					data_.write_f32(centertex.x());
					data_.write_f32(centertex.y());
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					//pt1
					//if (sadd)
					{
						data_.write_f32(spt.x());
						data_.write_f32(spt.y());
						data_.write_f32(stexpt.x());
						data_.write_f32(stexpt.y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
					}
					int middp = dif;
					for (int i = 0; i < num; ++i)
					{
						data_.write_f32(cc[(sxx + i ) % 8].x());
						data_.write_f32(cc[(sxx + i )%8].y());
						data_.write_f32(cctex[(sxx + i)%8].x());
						data_.write_f32(cctex[(sxx + i)%8].y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
					}

					//if (eadd)
					{
						data_.write_f32(ept.x());
						data_.write_f32(ept.y());
						data_.write_f32(etexpt.x());
						data_.write_f32(etexpt.y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
					}

					c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
					c_texture* tex = NULL;
					if(skin)
					tex = skin->get_skin_tex();

					m_triangle_gb->bind_vertex_buffer(BU_STREAM_DRAW, data_.get_pointer(), (3 + num) * 8 * sizeof(f32), fmts);
					c_ui_manager::get_singleton_ptr()->getTech()->get_pass(0)->get_sampler(L"tex")->tex = tex;
					c_device::get_singleton_ptr()->draw(c_ui_manager::get_singleton_ptr()->getTech(), m_triangle_gb);
				}
				else
				{
					//center
					data_.write_f32(center.x());
					data_.write_f32(center.y());
					data_.write_f32(centertex.x());
					data_.write_f32(centertex.y());
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					data_.write_f32(1.f);
					//if (eadd)
					{
						data_.write_f32(ept.x());
						data_.write_f32(ept.y());
						data_.write_f32(etexpt.x());
						data_.write_f32(etexpt.y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);

					}
					for (int i = 0; i < num; ++i)
					{
						data_.write_f32(cc[(exx + i)%8].x());
						data_.write_f32(cc[(exx + i)%8].y());
						data_.write_f32(cctex[(exx + i)%8].x());
						data_.write_f32(cctex[(exx + i)%8].y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
					}


					//pt1
					//if (sadd)
					{
						data_.write_f32(spt.x());
						data_.write_f32(spt.y());
						data_.write_f32(stexpt.x());
						data_.write_f32(stexpt.y());
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
						data_.write_f32(1.f);
					}
					c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
					c_texture* tex = NULL;
					if (skin)
						tex = skin->get_skin_tex();

					m_triangle_gb->bind_vertex_buffer(BU_STREAM_DRAW, data_.get_pointer(), (3 + num) * 8 * sizeof(f32), fmts);
					c_ui_manager::get_singleton_ptr()->getTech()->get_pass(0)->get_sampler(L"tex")->tex = tex;
					c_device::get_singleton_ptr()->draw(c_ui_manager::get_singleton_ptr()->getTech(), m_triangle_gb);
				}
				delete m_triangle_gb;
			}
			else
			{
				c_rect rc;
				rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
				rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
				rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
				rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch_rot(m_skinid, m_bg_img, m_commmon_rect, rc, m_clipping_rect, m_start_angle, m_clock_wise, false, false);
			}
		}
		c_widget::_redraw();
	}
}