/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "System.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "TabCtl.h"
namespace o2d{
	c_tabctl::c_tabctl(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_TABCTL , parent_, property_) ,
		m_txt_color(0xFFFFFFFF) , 
		m_page_offset(20) , 
		m_active_tab(-1) , 
		m_tab_posup(true),
		_VtxCurrentIdx(0)
	{
		memset(&_geos, 0, sizeof(_geos));
	}
	//--------------------------------------------------------
	c_tabctl::~c_tabctl(){}
	//--------------------------------------------------------
	void c_tabctl::set_tab_posup(bool up_)
	{
		if(m_tab_posup != up_)
		{
			m_tab_posup = up_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_page_offset(u32 offset_)
	{
		if(m_page_offset != offset_)
		{
			m_page_offset = offset_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_bg_image(const c_string& name_)
	{
		if(m_bg_img!= name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_tabctl::set_tab_nimage(const c_string& name_)
	{
		if(m_tab_nimage!=name_)
		{
			m_tab_nimage = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_tab_aimage(const c_string& name_)
	{
		if(m_tab_aimage!=name_)
		{
			m_tab_aimage = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_active(u32 idx_)
	{
		if(m_active_tab!=idx_)
		{
			if(m_active_tab != 0xFFFFFFFF)
			{
				FOREACH(TYPEOF(c_vector<u32>)::iterator , iter , m_tabs[m_active_tab].children)
				{
					c_widget* wid = get_widget(*iter , true);
					wid->set_visible(false);
				}
			}
			m_active_tab = (idx_ >= m_tabs.size())?m_tabs.size() - 1:idx_;	
			_sig_tab_change.emit(m_active_tab);
			if(m_active_tab != 0xFFFFFFFF)
			{		
				FOREACH(TYPEOF(c_vector<u32>)::iterator , iter , m_tabs[m_active_tab].children)
				{
					c_widget* wid = get_widget(*iter , true);
					wid->set_visible(true);
				}
			}
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::set_belong(u32 idx_, u32 id_)
	{
		if(idx_>=m_tabs.size())
			return;
		s_tab& tab = m_tabs[idx_];
		bool has = false;
		FOREACH(TYPEOF(c_vector<u32>)::iterator , iter , tab.children)
		{
			if(*iter == id_)
				has = true;
		}
		if(!has)
			tab.children.push_back(id_);
	}
	//--------------------------------------------------------
	void c_tabctl::add_tab(const c_string& name_)
	{
		s_tab tab;
		tab.name = name_;	
		f32 txtw , txth;
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(!ft)
			return;
		ft->measure_text(name_, txtw, txth);
		tab.width = txtw + 20;
		m_tabs.push_back(tab);	
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tabctl::rename_tab(u32 idx_, const c_string& name_)
	{
		m_tabs[idx_].name = name_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tabctl::remove_tab(u32 idx_)
	{
		assert(idx_ < m_tabs.size());
		c_vector<s_tab>::iterator iter = m_tabs.begin();
		iter+= idx_;
		FOREACH(TYPEOF(c_vector<u32>)::iterator , iter2 , iter->children)
		{
			c_widget* wid = get_widget(*iter2 , true);
			c_ui_manager::get_singleton_ptr()->remove_widget(wid);
		}
		m_tabs.erase(iter);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tabctl::set_tab_width(u32 idx_, u32 width_)
	{
		m_tabs[idx_].width = width_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tabctl::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tabctl::clear_all()
	{
		m_tabs.clear();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	u32 c_tabctl::get_tab_count() const
	{
		return m_tabs.size();
	}













	
	//--------------------------------------------------------
	void c_tabctl::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_tabctl::_on_touch_start()
	{		
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this &&!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
	}
	//--------------------------------------------------------
	void c_tabctl::_on_touch_finish()
	{
		if(!rect_contains(m_clipping_rect ,  c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
			return;
		}
		c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
		u32 offset = 0;
		u32 idx = 0;
		FOREACH(TYPEOF(c_vector<s_tab>)::iterator , iter , m_tabs)
		{
			c_rect rct = m_absolute_rect;
			rct.rb_pt().y() = rct.lt_pt().y() + m_page_offset;
			rct.lt_pt().x() += offset;
			rct.rb_pt().x() = rct.lt_pt().x() + iter->width;
			if(rect_contains(rct , pos))
			{
				set_active(idx);
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
			}
			idx++;
			offset += iter->width;
		}		
	}
	//--------------------------------------------------------
	void c_tabctl::_redraw()
	{
		glDisable(GL_CULL_FACE);
		if(!m_visible)
			return;
		_VtxCurrentIdx = 0;
		if (_geocache.fill)
			AddPolylineFilled(_geocache.points, _geocache.col, _geocache.closed, _geocache.fill, false);
		else
			AddPolyline(_geocache.points, _geocache.col, _geocache.closed, _geocache.fill, false);

		c_ui_manager::get_singleton_ptr()->cache_batch_lines(_geos.geo, _geos.fill, _geos.clolor);

		c_widget::_redraw();
		glEnable(GL_CULL_FACE);
	}

	static inline float  ImInvLength(const c_float2& lhs, float fail_value) { float d = lhs.x()*lhs.x() + lhs.y()*lhs.y(); if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }
	void c_tabctl::clear()
	{
		if (_geos.geo)
		{
			delete _geos.geo;
		}
		memset(&_geos, 0, sizeof(_geos));
		memset(&_geocache, 0, sizeof(_geocache));
		_VtxCurrentIdx = 0;
	}

	void c_tabctl::AddPolylineFilled(const c_vector<c_float2>& points, u32 col, bool closed, bool fill, bool cache)
	{
		c_float2 center = m_absolute_rect.lt_pt();
		center.x() += m_absolute_rect.width() * 0.5f;
		center.y() += m_absolute_rect.height() * 0.5f;
		c_stream vb;
		c_stream ib;
		const int points_count = points.size();
		if (cache)
		{
			_geocache.closed = closed;
			_geocache.col = col;
			_geocache.fill = fill;
			_geocache.points = points;
		}
		c_geometry_buffer* gbo;
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		if (!_geos.geo)
		{
			gbo = new c_geometry_buffer(RO_TRIANGLES);
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
		}
		{
			// Anti-aliased Fill
			const float AA_SIZE = 1.0f;
			const u32 col_trans = col & 0x00FFFFFF;
			const int idx_count = (points_count - 2) * 3 + points_count * 6;
			const int vtx_count = (points_count * 2);
			vb.resize(vtx_count * 8 * sizeof(f32));
			ib.resize(idx_count * sizeof(u16));

			// Add indexes for fill
			unsigned int vtx_inner_idx = _VtxCurrentIdx;
			unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
			for (int i = 2; i < points_count; i++)
			{
				ib.write_u16((vtx_inner_idx));
				ib.write_u16((vtx_inner_idx + ((i - 1) << 1))); 
				ib.write_u16((vtx_inner_idx + (i << 1)));
				//_IdxWritePtr += 3;
			}

			// Compute normals
			c_float2* temp_normals = (c_float2*)alloca(points_count * sizeof(c_float2));
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				const c_float2& p0 = points[i0];
				const c_float2& p1 = points[i1];
				c_float2 diff = p1 - p0;
				diff *= ImInvLength(diff, 1.0f);
				temp_normals[i0].x() = diff.y();
				temp_normals[i0].y() = -diff.x();
			}

			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				// Average normals
				const c_float2& n0 = temp_normals[i0];
				const c_float2& n1 = temp_normals[i1];
				c_float2 dm = (n0 + n1) * 0.5f;
				float dmr2 = dm.x()*dm.x() + dm.y()*dm.y();
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				dm *= AA_SIZE * 0.5f;

				// Add vertices
				vb.write_f32((points[i1] - dm).x() + center.x());
				vb.write_f32((points[i1] - dm).y() + center.y());
				vb.write_f32(0.5f);
				vb.write_f32(0.5f);
				vb.write_f32(c_color(col).r());
				vb.write_f32(c_color(col).g());
				vb.write_f32(c_color(col).b() + 0.000001);
				vb.write_f32(c_color(col).a() + 0.000001);


				vb.write_f32((points[i1] + dm).x() + center.x());
				vb.write_f32((points[i1] + dm).y() + center.y());
				vb.write_f32(0.5f);
				vb.write_f32(0.5f);
				vb.write_f32(c_color(col_trans).r());
				vb.write_f32(c_color(col_trans).g());
				vb.write_f32(c_color(col_trans).b() + 0.000001);
				vb.write_f32(c_color(col_trans).a() + 0.000001);


				ib.write_u16( (vtx_inner_idx + (i1 << 1)));
				ib.write_u16( (vtx_inner_idx + (i0 << 1)));
				ib.write_u16( (vtx_outer_idx + (i0 << 1)));
				ib.write_u16( (vtx_outer_idx + (i0 << 1)));
				ib.write_u16( (vtx_outer_idx + (i1 << 1))); 
				ib.write_u16( (vtx_inner_idx + (i1 << 1)));
			}
			_VtxCurrentIdx += vtx_count;
		}
		_geos.clolor = col;
		_geos.fill = true;
		if (_geos.geo)
		{
			_geos.geo->uploadvb(0, vb.get_pointer(), vb.get_size());
			_geos.geo->uploadib(0, ib.get_pointer(), ib.get_size());
		}
		else
		{
			_geos.geo = gbo;
			gbo->bind_vertex_buffer(BU_DYNAMIC_DRAW, vb.get_pointer(), vb.get_size(), fmts);
			gbo->bind_index_buffer(BU_DYNAMIC_DRAW, ib.get_pointer(), ib.get_size(), IF_16UI);
		}

		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	void c_tabctl::AddPolyline(const c_vector<c_float2>& points, u32 col, bool closed, bool fill, bool cache)
	{
		c_float2 center = m_absolute_rect.lt_pt();
		center.x() += m_absolute_rect.width() * 0.5f;
		center.y() += m_absolute_rect.height() * 0.5f;
		const int points_count = points.size();
		if (points_count < 2)
			return;
		if (cache)
		{
			_geocache.closed = closed;
			_geocache.col = col;
			_geocache.fill = fill;
			_geocache.points = points;
		}
		int count = points_count;
		if (!closed)
			count = points_count - 1;

		c_geometry_buffer* gbo;
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		if (!_geos.geo)
		{
			gbo = new c_geometry_buffer(RO_TRIANGLES);
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
		}
		c_stream vb;
		c_stream ib;

		const bool thick_line = false;
		{
			// Anti-aliased stroke
			const float AA_SIZE = 1.0f;
			//~fixme
			const u32 col_trans = col & 0x00FFFFFF;

			const int idx_count = thick_line ? count * 18 : count * 12;
			const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
			vb.resize(vtx_count * 8 *sizeof(f32));
			ib.resize(idx_count * sizeof(u16));
			//PrimReserve(idx_count, vtx_count);

			// Temporary buffer
			c_float2* temp_normals = (c_float2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(c_float2));
			c_float2* temp_points = temp_normals + points_count;

			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				c_float2 diff = points[i2] - points[i1];
				diff *= ImInvLength(diff, 1.0f);
				temp_normals[i1].x() = diff.y();
				temp_normals[i1].y() = -diff.x();
			}
			if (!closed)
				temp_normals[points_count - 1] = temp_normals[points_count - 2];

			if (!thick_line)
			{
				if (!closed)
				{
					temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
					temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
					temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
					temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
				}

				// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
				unsigned int idx1 = _VtxCurrentIdx;
				for (int i1 = 0; i1 < count; i1++)
				{
					const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
					unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

					// Average normals
					c_float2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
					float dmr2 = dm.x()*dm.x() + dm.y()*dm.y();
					if (dmr2 > 0.000001f)
					{
						float scale = 1.0f / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					dm *= AA_SIZE;
					temp_points[i2 * 2 + 0] = points[i2] + dm;
					temp_points[i2 * 2 + 1] = points[i2] - dm;

					// Add indexes
					ib.write_u16(idx2 + 0); 
					ib.write_u16(idx1 + 0); 
					ib.write_u16(idx1 + 2);
					ib.write_u16(idx1 + 2);
					ib.write_u16(idx2 + 2);
					ib.write_u16(idx2 + 0);
					ib.write_u16(idx2 + 1); 
					ib.write_u16(idx1 + 1); 
					ib.write_u16(idx1 + 0);
					ib.write_u16(idx1 + 0); 
					ib.write_u16(idx2 + 0); 
					ib.write_u16(idx2 + 1);

					idx1 = idx2;
				}

				// Add vertexes
				for (int i = 0; i < points_count; i++)
				{
					vb.write_f32(points[i].x() + center.x());
					vb.write_f32(points[i].y() + center.y());
					vb.write_f32(0.5f);
					vb.write_f32(0.5f);
					vb.write_f32(c_color(col).r());
					vb.write_f32(c_color(col).g());
					vb.write_f32(c_color(col).b() + 0.000001);
					vb.write_f32(c_color(col).a() + 0.000001);
					vb.write_f32(temp_points[i * 2].x() + center.x());
					vb.write_f32(temp_points[i * 2].y() + center.y());
					vb.write_f32(0.5f);
					vb.write_f32(0.5f);
					vb.write_f32(c_color(col_trans).r());
					vb.write_f32(c_color(col_trans).g());
					vb.write_f32(c_color(col_trans).b() + 0.000001);
					vb.write_f32(c_color(col_trans).a() + 0.000001);
					vb.write_f32(temp_points[i * 2 + 1].x() + center.x());
					vb.write_f32(temp_points[i * 2 + 1].y() + center.y());
					vb.write_f32(0.5f);
					vb.write_f32(0.5f);
					vb.write_f32(c_color(col_trans).r());
					vb.write_f32(c_color(col_trans).g());
					vb.write_f32(c_color(col_trans).b() + 0.000001);
					vb.write_f32(c_color(col_trans).a() + 0.000001);
					//_VtxWritePtr += 3;
				}
			}
			_VtxCurrentIdx += vtx_count;
		}
		_geos.clolor = col;
		_geos.fill = false;
		if (_geos.geo)
		{
			_geos.geo->uploadvb(0, vb.get_pointer(), vb.get_size());
			_geos.geo->uploadib(0, ib.get_pointer(), ib.get_size());
		}
		else
		{
			_geos.geo = gbo;
			gbo->bind_vertex_buffer(BU_DYNAMIC_DRAW, vb.get_pointer(), vb.get_size(), fmts);
			gbo->bind_index_buffer(BU_DYNAMIC_DRAW, ib.get_pointer(), ib.get_size(), IF_16UI);
		}
	
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
}