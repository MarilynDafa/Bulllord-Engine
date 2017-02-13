/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Window.h"
#include "Shader.h"
#include "InputMgr.h"
#include "ResMgr.h"
#include "Label.h"
#include "Button.h"
#include "Radio.h"
#include "EditBox.h"
#include "Menu.h"
#include "ProgressBar.h"
#include "Slider.h"
#include "Dialog.h"
#include "TabCtl.h"
#include "CartoonBox.h"
#include "DragDropSlot.h"
#include "ListBox.h"
#include "ScrollBox.h"
#include "TableBox.h"
#include "TreeBox.h"
#include "RichBox.h"
#include "UIMgr.h"
namespace o2d{
	c_ui_manager::c_ui_manager()
		:m_billboard_loader(nullptr) , 
		m_placeholder_loader(nullptr) , 
		m_ui_tech(nullptr) , 
		m_batch_gbo(nullptr) , 
		m_billboard_gbo(nullptr) , 
		m_fbo(nullptr) ,  
		m_environment(nullptr) ,
		m_hovered_widget(nullptr) , 
		m_focus_widget(nullptr) , 
		m_dirty(false)
	{
		memset(m_uu_clr , 0 , sizeof(m_uu_clr));
	}
	//--------------------------------------------------------
	c_ui_manager::~c_ui_manager(){}
	//--------------------------------------------------------
	bool c_ui_manager::initialize()
	{
		const c_float2& dim = c_device::get_singleton_ptr()->get_window()->get_dim();		
		c_shader* sh = (c_shader*)c_resource_manager::get_singleton_ptr()->fetch_buildin_res(BRI_UI_GLFX);
#if !defined(O2D_USE_DX_API)
		m_ui_tech = sh->get_tech(L"ui_gl");
#else
		m_ui_tech = sh->get_tech(L"ui_dx");
#endif
		m_fbo = new c_framebuffer();
		m_billboard_loader = new c_tex_loader(true , dim.x() , dim.y());
		c_resource_manager::get_singleton_ptr()->fetch_resource(m_billboard_loader);
		m_fbo->attach_color_buffer(0 , (c_texture*)m_billboard_loader->get_res() , 0);
		c_widget::s_widget_property p;
		p.name = L"Root";
		p.dim = c_rect(0.f , 0.f , dim.x() , dim.y());
		m_environment = new c_widget(WT_WIDGET, nullptr, p);
		m_placeholder_loader = new c_tex_loader(false , 2 , 2);
		c_resource_manager::get_singleton_ptr()->fetch_resource(m_placeholder_loader);
		u32 tex[4];
		memset(tex , 0xFFFFFFFF , 4*sizeof(u32));
		((c_texture*)m_placeholder_loader->get_res())->upload(0 , 0 , 0 , 2 , 2 , tex , 4*sizeof(u32));
		set_uu_color(UUC_EDIT_HIGHLIGHT , MAKE_U32(MAKE_U16(30 , 30) , MAKE_U16(140 , 50)));
		set_uu_color(UUC_EDIT_CURSOR , MAKE_U32(MAKE_U16(240 , 240) , MAKE_U16(240 , 240)));
		set_uu_color(UUC_MENU_SEPARATOR , MAKE_U32(MAKE_U16(101 , 195) , MAKE_U16(195 , 195)));
		set_uu_color(UUC_MENU_SELECTING , MAKE_U32(MAKE_U16(240 , 22) , MAKE_U16(207 , 181)));
		set_uu_color(UUC_TABLE_SEPARATOR , MAKE_U32(MAKE_U16(50 , 40) , MAKE_U16(50 , 100)));
		set_uu_color(UUC_TABLE_SELECTING , MAKE_U32(MAKE_U16(22 , 22) , MAKE_U16(50 , 120)));
		set_uu_color(UUC_TABLE_HIGHLIGHT , MAKE_U32(MAKE_U16(200 , 207) , MAKE_U16(207 , 255)));
		set_uu_color(UUC_LIST_SELECTING , MAKE_U32(MAKE_U16(22 , 50) , MAKE_U16(50 , 120)));
		set_uu_color(UUC_LIST_HIGHLIGHT , MAKE_U32(MAKE_U16(200 , 207) , MAKE_U16(207 , 255)));
		set_uu_color(UUC_TREE_LINE , MAKE_U32(MAKE_U16(50 , 100) , MAKE_U16(207 , 180)));
		set_uu_color(UUC_TREE_SELECTING , MAKE_U32(MAKE_U16(22 , 22) , MAKE_U16(50 , 120)));
		set_uu_color(UUC_TXT_DISABLE_CLR , MAKE_U32(MAKE_U16(240 , 130) , MAKE_U16(130 , 130)));
		_build_geo_buffer(dim.x() , dim.y());
		return true;
	}
	//--------------------------------------------------------
	void c_ui_manager::shutdown()
	{
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator , iter , m_skins)
			c_resource_manager::get_singleton_ptr()->discard_resource((*iter)->get_filename());
		FOREACH(TYPEOF(c_vector<c_font*>)::iterator , iter , m_fonts)
			c_resource_manager::get_singleton_ptr()->discard_resource((*iter)->get_filename());
		m_fbo->detach_color_buffer(0);
		delete m_fbo;
		if(m_billboard_loader)
		{
			c_resource_manager::get_singleton_ptr()->discard_resource(m_billboard_loader);
			delete m_billboard_loader;
		}
		if(m_placeholder_loader)
		{
			c_resource_manager::get_singleton_ptr()->discard_resource(m_placeholder_loader);
			delete m_placeholder_loader;
		}
		delete m_billboard_gbo;
		delete m_batch_gbo;
		delete m_environment;
	}
	//--------------------------------------------------------
	void c_ui_manager::set_uu_color(e_uniform_ui_color idx_, u32 clr_)
	{
		if(m_uu_clr[idx_] != clr_)
		{
			m_uu_clr[idx_] = clr_;
			m_dirty = true;
		}
	}
	//--------------------------------------------------------
	c_skin* c_ui_manager::add_skin(const c_string& name_)
	{
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator, iter, m_skins)
		{
			if (name_ == (*iter)->get_filename())
			{
				return *iter;
			}
		}
		c_skin* skin = (c_skin*)c_resource_manager::get_singleton_ptr()->fetch_resource(name_.c_str() , false);
		if(skin)
			m_skins.push_back(skin);
		return skin;
	}
	//--------------------------------------------------------
	c_skin* c_ui_manager::get_skin(const c_string& name_)
	{
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator, iter, m_skins)
		{
			if (name_ == (*iter)->get_filename())
			{
				return *iter;
			}
		}
		return NULL;
	}
	//--------------------------------------------------------
	bool c_ui_manager::remove_skin(const c_string& name_)
	{
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator , iter , m_skins)
		{
			if(name_ == (*iter)->get_filename())
			{
				c_resource_manager::get_singleton_ptr()->discard_resource((*iter)->get_filename());
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------
	c_font* c_ui_manager::add_font(const c_string& name_)
	{
		c_font* fnt = (c_font*)c_resource_manager::get_singleton_ptr()->fetch_resource(name_.c_str() , false);
		if(fnt)
			m_fonts.push_back(fnt);
		return fnt;
	}
	//--------------------------------------------------------
	bool c_ui_manager::remove_font(const c_string& name_)
	{
		FOREACH(TYPEOF(c_vector<c_font*>)::iterator , iter , m_fonts)
		{
			if(name_ == (*iter)->get_filename())
			{
				c_resource_manager::get_singleton_ptr()->discard_resource((*iter)->get_filename());
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------
	c_skin* c_ui_manager::get_skin(u32 skid_)
	{
		c_skin* skin(nullptr);
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator , iters , m_skins)
		{
			if(skid_ == (*iters)->get_id())
				skin = *iters;
		}
		return skin;
	}
	//--------------------------------------------------------
	c_font* c_ui_manager::get_font(u32 fntid_)
	{
		c_font* ft(nullptr);
		FOREACH(TYPEOF(c_vector<c_font*>)::iterator , iterf , m_fonts)
		{
			if(fntid_ == (*iterf)->get_id())
				ft = *iterf;
		}
		return ft;
	}
	//--------------------------------------------------------
	c_texture* c_ui_manager::get_placeholder_tex()
	{
		return (c_texture*)m_placeholder_loader->get_res();
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::get_widget(u32 id_)
	{
		return m_environment->get_widget(id_, true);
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::get_widget(const c_string& name_)
	{
		return m_environment->get_widget(hash((const char*)name_.to_utf8()) , true);
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::add_widget(c_widget* wid_ , c_widget* parent_)
	{
		parent_->_add_child(wid_);
		return wid_;
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::add_widget(e_widget_type type_, c_widget* parent_, const c_widget::s_widget_property& property_)
	{
		c_widget* ret(nullptr);
		switch(type_)
		{
		case WT_WIDGET:
			ret = new c_widget(WT_WIDGET, parent_ ? parent_ : m_environment, property_);
			break;
		case WT_LABEL:
			ret = new c_label(parent_?parent_:m_environment , property_);
			break;
		case WT_BUTTON:
			ret = new c_button(parent_?parent_:m_environment , property_);
			break;
		case WT_RADIO:
			ret = new c_radio(parent_?parent_:m_environment , property_);
			break;
		case WT_EDITBOX:
			ret = new c_editbox(parent_?parent_:m_environment , property_);
			break;
		case WT_PROGRESSBAR:
			ret = new c_progressbar(parent_?parent_:m_environment , property_);
			break;
		case WT_DIALOG:
			ret = new c_dialog(parent_?parent_:m_environment , property_);
			break;
		case WT_SLIDER:
			ret = new c_slider(parent_?parent_:m_environment , property_);
			break;
		case WT_MENU:
			ret = new c_menu(parent_?parent_:m_environment , property_);
			break;
		case WT_TABCTL:
			ret = new c_tabctl(parent_?parent_:m_environment , property_);
			break;
		case WT_TABLEBOX:
			ret = new c_tablebox(parent_?parent_:m_environment , property_);
			break;
		case WT_LISTBOX:
			ret = new c_listbox(parent_?parent_:m_environment , property_);
			break;
		case WT_CARTOONBOX:
			ret = new c_cartoonbox(parent_?parent_:m_environment , property_);
			break;
		case WT_DRAGDROPSLOT:
			ret = new c_dragdrop_slot(parent_?parent_:m_environment , property_);
			break;
		case WT_SCROLLBOX:
			ret = new c_scrollbox(parent_?parent_:m_environment , property_);
			break;
		case WT_TREEBOX:
			ret = new c_treebox(parent_?parent_:m_environment , property_);
			break;
		case WT_RICHBOX:
			ret = new c_richbox(parent_?parent_:m_environment , property_);
			break;
		default:
			break;
		}
		m_dirty = true;
		return ret;
	}
	//--------------------------------------------------------
	void c_ui_manager::remove_widget(c_widget* widget_)
	{
		widget_->m_need_delete = true;
		remove_focus(widget_);
		reset_hovered();
		m_dirty = true;
	}
	//--------------------------------------------------------
	void c_ui_manager::clear()
	{
		if(m_hovered_widget && m_hovered_widget != m_environment)
			m_hovered_widget = nullptr;
		if(m_focus_widget)
			m_focus_widget = nullptr;
		FOREACH(TYPEOF(c_list<c_widget*>)::const_iterator , iter , m_environment->get_children())
			delete *iter;
		m_environment->m_children.clear();
	}
	//--------------------------------------------------------
	bool c_ui_manager::set_focus(c_widget* widget_)
	{
		if(m_focus_widget == widget_)
			return false;
		if(widget_ == m_environment)
			widget_ = nullptr;
		if(m_focus_widget&&m_focus_widget->_lost_focus(widget_))
			return false;
		if(widget_&&widget_->_gain_focus(m_focus_widget))
			return false;
		m_focus_widget = widget_;
		return true;
	}
	//--------------------------------------------------------
	bool c_ui_manager::remove_focus(c_widget* widget_)
	{
		if(m_focus_widget && m_focus_widget==widget_&&m_focus_widget->_lost_focus(nullptr))
			return false;
		if(m_focus_widget)
			m_focus_widget = nullptr;
		return true;
	}
	//--------------------------------------------------------
	void c_ui_manager::reset_hovered()
	{
		m_hovered_widget = nullptr;
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(c_texture* tex_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy)
	{
		if(!tex_)
			return;
		s_batch ba;
		ba.tex = tex_;
		c_rect cv = vr_;
		c_rect tr = c_rect(0.f , 0.f , 1.f , 1.f);
		rect_clip(cv , cr_);
		f32 tw = tr.width();
		f32 th = tr.height();
		tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x())/vr_.width();
		tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y())/vr_.height();
		tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x())/vr_.width();
		tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y())/vr_.height();
		ba.vb.resize(4*8*sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));  
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x()); 
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));	
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(c_texture* tex_, const c_rect& tr_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy)
	{
		if(!tex_)
			return;
		s_batch ba;
		ba.tex = tex_;
		c_rect cv = vr_;
		c_rect tr = tr_;
		rect_clip(cv , cr_);
		f32 tw = tr.width();
		f32 th = tr.height();
		tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x())/vr_.width();
		tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y())/vr_.height();
		tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x())/vr_.width();
		tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y())/vr_.height();
		ba.vb.resize(4*8*sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 skinid_, const c_string& img_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy, bool disable)
	{
		if(img_.is_empty())
			return;
		c_skin* sk(nullptr);
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator , iterk , m_skins)
		{
			if(skinid_ == (*iterk)->get_id())
			{
				sk = *iterk;
				break;
			}
		}
		s_batch ba;
		c_rect cv = vr_;
		c_rect tr;
		if(!sk)
		{
			u32 clr = img_.sub_str(4).to_uinteger();
			ba.tex = (c_texture*)m_placeholder_loader->get_res();
			tr = c_rect(0.f , 0.f , 1.f , 1.f);
			ba.vb.resize(4*8*sizeof(f32));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
			ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
			ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
			ba.vb.write_f32(c_color(clr).r());
			ba.vb.write_f32(c_color(clr).g());
			ba.vb.write_f32(c_color(clr).b());
			ba.vb.write_f32(c_color(clr).a());
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
			ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
			ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
			ba.vb.write_f32(c_color(clr).r());
			ba.vb.write_f32(c_color(clr).g());
			ba.vb.write_f32(c_color(clr).b());
			ba.vb.write_f32(c_color(clr).a());
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
			ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
			ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
			ba.vb.write_f32(c_color(clr).r());
			ba.vb.write_f32(c_color(clr).g());
			ba.vb.write_f32(c_color(clr).b());
			ba.vb.write_f32(c_color(clr).a());
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
			ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
			ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
			ba.vb.write_f32(c_color(clr).r());
			ba.vb.write_f32(c_color(clr).g());
			ba.vb.write_f32(c_color(clr).b());
			ba.vb.write_f32(c_color(clr).a());
		}
		else
		{
			ba.tex = sk->get_skin_tex();
			tr = sk->get_texcoord(img_);
			tr.lt_pt().x() /= ba.tex->get_width();
			tr.rb_pt().x() /= ba.tex->get_width();
			tr.lt_pt().y() /= ba.tex->get_height();
			tr.rb_pt().y() /= ba.tex->get_height();
			rect_clip(cv , cr_);
			f32 tw = tr.width();
			f32 th = tr.height();
			tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x())/vr_.width();
			tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y())/vr_.height();
			tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x())/vr_.width();
			tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y())/vr_.height();
			ba.vb.resize(4*8*sizeof(f32));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
			ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
			ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(disable ? -1.f : 1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
			ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
			ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(disable ? -1.f : 1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
			ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
			ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(disable ? -1.f : 1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
			ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
			ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
			ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(1.f);
			ba.vb.write_f32(disable ? -1.f : 1.f);
			ba.vb.write_f32(1.f);
		}	
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, bool flipx, bool flipy, bool disable)
	{
		if(img_.is_empty())
			return;
		c_skin* sk = m_skins[0];
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator , iterk , m_skins)
		{
			if(skinid_ == (*iterk)->get_id())
			{
				sk = *iterk;
				break;
			}
		}
		s_batch ba;
		c_rect cv = vr_;
		c_rect tr;
        if(sk)
            ba.tex = sk->get_skin_tex();
		tr = tr_;
		rect_clip(cv , cr_);
		f32 tw = tr.width();
		f32 th = tr.height();
		tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x())/vr_.width();
		tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y())/vr_.height();
		tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x())/vr_.width();
		tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y())/vr_.height();
		ba.vb.resize(4*8*sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(disable ? -1.f : 1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(disable ? -1.f : 1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(disable ? -1.f : 1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(disable ? -1.f : 1.f);
		ba.vb.write_f32(1.f);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}

	static c_float2 pt_rotate(const c_float2& pt, const c_float2& center, float rad, bool clockwise)
	{
		float radf = clockwise ? rad : -rad;
		float x = pt.x();
		float y = pt.y();
		float x0, y0;
		float rx0 = center.x();
		float ry0 = center.y();
		x0 = (x - rx0)*cosf(radf) - (y - ry0)*sinf(radf) + rx0;
		y0 = (x - rx0)*sinf(radf) + (y - ry0)*cosf(radf) + ry0;
		return c_float2(x0, y0);
	}
	void c_ui_manager::cache_batch_rot(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, int rot, bool clockwise, bool flipx, bool flipy)
	{
		if (img_.is_empty())
			return;
		c_skin* sk = m_skins[0];
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator, iterk, m_skins)
		{
			if (skinid_ == (*iterk)->get_id())
			{
				sk = *iterk;
				break;
			}
		}
		s_batch ba;
		c_rect cv = vr_;
		c_rect tr;
		if (sk)
			ba.tex = sk->get_skin_tex();
		tr = tr_; 
		float rad = DEG_2_RAD * rot;
		c_float2 lt = cv.lt_pt();
		c_float2 rt = cv.lt_pt();
		rt.x() = cv.rb_pt().x();
		c_float2 lb = lt;
		lb.y() = cv.rb_pt().y();
		c_float2 rb = cv.rb_pt();
		f32 tw = tr.width();
		f32 th = tr.height();
		c_float2 ct = (lt + rb)*0.5;
		c_float2 ltr, rtr, lbr, rbr;
		ltr =  pt_rotate(lt, ct, rad, clockwise);
		rtr =  pt_rotate(rt, ct, rad, clockwise);
		lbr =  pt_rotate(lb, ct, rad, clockwise);
		rbr =  pt_rotate(rb, ct, rad, clockwise);
		//tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x()) / vr_.width();
		//tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y()) / vr_.height();
		//tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x()) / vr_.width();
		//tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y()) / vr_.height();
		ba.vb.resize(4 * 8 * sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(ltr.x()));
		ba.vb.write_f32(PIXEL_ALIGNED(ltr.y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(rtr.x()));
		ba.vb.write_f32(PIXEL_ALIGNED(rtr.y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(lbr.x()));
		ba.vb.write_f32(PIXEL_ALIGNED(lbr.y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(rbr.x()));
		ba.vb.write_f32(PIXEL_ALIGNED(rbr.y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch_rot(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, const c_rect& str_, int rot, bool clockwise, bool flipx, bool flipy)
	{
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, const c_rect& str_, bool flipx, bool flipy)
	{
		if (img_.is_empty())
			return;
		c_skin* sk = m_skins[0];
		FOREACH(TYPEOF(c_vector<c_skin*>)::iterator, iterk, m_skins)
		{
			if (skinid_ == (*iterk)->get_id())
			{
				sk = *iterk;
				break;
			}
		}
		s_batch ba;
		c_rect cv = vr_;
		c_rect tr;
		if (sk)
			ba.tex = sk->get_skin_tex();
		tr = tr_;
		rect_clip(cv, cr_);
		f32 tw = tr.width();
		f32 th = tr.height();
		tr.lt_pt().x() += tw*(cv.lt_pt().x() - vr_.lt_pt().x()) / vr_.width();
		tr.lt_pt().y() += th*(cv.lt_pt().y() - vr_.lt_pt().y()) / vr_.height();
		tr.rb_pt().x() += tw*(cv.rb_pt().x() - vr_.rb_pt().x()) / vr_.width();
		tr.rb_pt().y() += th*(cv.rb_pt().y() - vr_.rb_pt().y()) / vr_.height();
		ba.vb.resize(4 * 8 * sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(str_.lt_pt().x());
		ba.vb.write_f32(str_.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(-1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.rb_pt().y() : tr.lt_pt().y());
		ba.vb.write_f32(str_.rb_pt().x());
		ba.vb.write_f32(str_.lt_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(-1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.rb_pt().x() : tr.lt_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(str_.lt_pt().x());
		ba.vb.write_f32(str_.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(-1.f);
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(flipx ? tr.lt_pt().x() : tr.rb_pt().x());
		ba.vb.write_f32(flipy ? tr.lt_pt().y() : tr.rb_pt().y());
		ba.vb.write_f32(str_.rb_pt().x());
		ba.vb.write_f32(str_.rb_pt().y());
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(-1.f);
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch_lines(c_geometry_buffer* geo,  bool fill, const c_color& clr)
	{
	//	if (!fill)
		//	glPolygonOffset(GL_FRONT_AND_BACK, GL_LINE);
		/*s_batch ba;
		ba.tex = ;
		c_rect tr;
		ba.vb.resize(pts.size() * 8 * sizeof(f32));
		for (int i = 0; i < pts.size(); ++i)
		{
			ba.vb.write_f32(pts[i].x() + offset.x());
			ba.vb.write_f32(pts[i].y() + offset.y());
			ba.vb.write_f32(0.f);
			ba.vb.write_f32(0.f);
			ba.vb.write_f32(clr.r());
			ba.vb.write_f32(clr.g());
			ba.vb.write_f32(clr.b());
			ba.vb.write_f32(clr.a());
		}*/
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = (c_texture*)m_placeholder_loader->get_res();
		c_device::get_singleton_ptr()->draw(m_ui_tech, geo);
	//	if (!fill)
		//	glPolygonOffset(GL_FRONT_AND_BACK, GL_FILL);
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(e_uniform_ui_color img_, const c_rect& vr_, const c_rect& cr_)
	{
		s_batch ba;
		ba.tex = (c_texture*)m_placeholder_loader->get_res();
		c_rect tr;
		c_rect cv = vr_;
		rect_clip(cv , cr_);
		c_color clr = m_uu_clr[img_];
		ba.vb.resize(4*8*sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));  
		ba.vb.write_f32(0.f); 
		ba.vb.write_f32(0.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));	
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(0.f);  
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(0.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 clr_, const c_rect& vr_, const c_rect& cr_)
	{
		s_batch ba;
		ba.tex = (c_texture*)m_placeholder_loader->get_res();
		c_rect tr;
		c_rect cv = vr_;
		rect_clip(cv , cr_);
		c_color clr = clr_;
		ba.vb.resize(4*8*sizeof(f32));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));  
		ba.vb.write_f32(0.f); 
		ba.vb.write_f32(0.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));	
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(0.f);  
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(0.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
		ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(1.f);
		ba.vb.write_f32(clr.r());
		ba.vb.write_f32(clr.g());
		ba.vb.write_f32(clr.b());
		ba.vb.write_f32(clr.a());
		m_batches.push_back(ba);
#ifndef TESTSTENCIL
		m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
		c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 fntid_, u32 fth_, u32 fmt_, bool single_, const c_string& txt_, const c_rect& r_, const c_rect& cr_, u32 clr_,bool pwd_/*=false*/, u16 flag )
	{
		if(txt_.is_empty())
			return;
		c_font* ft(nullptr);
		c_string str = pwd_?c_string(txt_.get_length() , L'*'):txt_;
		FOREACH(TYPEOF(c_vector<c_font*>)::iterator , iter , m_fonts)
		{
			if((*iter)->get_id() == fntid_)
			{
				ft = *iter;
				break;
			}
		}
		if(!ft || !ft->active_height(fth_))
			return;
		if(!single_)
			ft->text_break_word(str, r_.width(), flag);
		f32 w , h;
		ft->measure_text(str , w , h, flag);
		c_float2 pt;
		pt.x() = r_.lt_pt().x();
		pt.y() = r_.lt_pt().y();
		if(fmt_ & TA_V_CENTER)
			pt.y() = (r_.lt_pt().y() + r_.rb_pt().y()) / 2.f - h / 2.f;
		else if (fmt_ & TA_V_BOTTOM)
			pt.y() = r_.rb_pt().y() - h;
		else
			pt.y() = r_.lt_pt().y();
		if(single_)
		{
			if(fmt_ & TA_H_CENTER)                             
				pt.x() = (r_.lt_pt().x() + r_.rb_pt().x()) / 2.f - w / 2.f;            
			else if(fmt_ & TA_H_RIGHT)             
				pt.x() = r_.rb_pt().x() - w;
			else            
				pt.x() = r_.lt_pt().x();
			for(u32 i = 0; i < str.get_length(); ++i)
			{
				utf16 c = str[i];
				if(c == L'\r' || c == L'\n')
					continue;
				c_font::s_glyph_info* gi = ft->get_glyph(MAKEU32(c, flag));
				if(!gi)
					continue;
				c_rect verts(pt.x()+gi->offset.x() , pt.y()+gi->offset.y() , 0.f , 0.f);
				verts.rb_pt().x() = verts.lt_pt().x() + (gi->rect.rb_pt().x() - gi->rect.lt_pt().x());
				verts.rb_pt().y() = verts.lt_pt().y() + (gi->rect.rb_pt().y() - gi->rect.lt_pt().y());
				c_rect cv = verts;
				c_rect tr;
				rect_clip(cv , cr_);
				f32 dx1 , dx2, dy1 , dy2;
				dx1 = (cv.lt_pt().x() - verts.lt_pt().x())/verts.width();
				dx2 = (cv.rb_pt().x() - verts.rb_pt().x())/verts.width();
				dy1 = (cv.lt_pt().y() - verts.lt_pt().y())/verts.height();
				dy2 = (cv.rb_pt().y() - verts.rb_pt().y())/verts.height();
				tr.lt_pt().x() = (gi->rect.lt_pt().x() + dx1*gi->rect.width())/256.f;
				tr.lt_pt().y() = (gi->rect.lt_pt().y() + dy1*gi->rect.height())/256.f;
				tr.rb_pt().x() = (gi->rect.rb_pt().x() + dx2*gi->rect.width())/256.f;
				tr.rb_pt().y() = (gi->rect.rb_pt().y() + dy2*gi->rect.height())/256.f;
				if(cv.lt_pt().x() < cv.rb_pt().x() && cv.lt_pt().y()  < cv.rb_pt().y())
				{
					c_color clr(clr_);
					clr.b() = max(0.00001f, clr.b());
					s_batch ba;
					ba.tex = gi->tex;
					ba.vb.resize(4*8*sizeof(f32));
					ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
					ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
					ba.vb.write_f32(tr.lt_pt().x());
					ba.vb.write_f32(tr.lt_pt().y());
					ba.vb.write_f32(clr.r());
					ba.vb.write_f32(clr.g());
					ba.vb.write_f32(clr.b());
					ba.vb.write_f32(clr.a());
					ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
					ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
					ba.vb.write_f32(tr.rb_pt().x());
					ba.vb.write_f32(tr.lt_pt().y());
					ba.vb.write_f32(clr.r());
					ba.vb.write_f32(clr.g());
					ba.vb.write_f32(clr.b());
					ba.vb.write_f32(clr.a());
					ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
					ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
					ba.vb.write_f32(tr.lt_pt().x());
					ba.vb.write_f32(tr.rb_pt().y());
					ba.vb.write_f32(clr.r());
					ba.vb.write_f32(clr.g());
					ba.vb.write_f32(clr.b());
					ba.vb.write_f32(clr.a());
					ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x())); 
					ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y())); 
					ba.vb.write_f32(tr.rb_pt().x()); 
					ba.vb.write_f32(tr.rb_pt().y());
					ba.vb.write_f32(clr.r());
					ba.vb.write_f32(clr.g());
					ba.vb.write_f32(clr.b());
					ba.vb.write_f32(clr.a());
					m_batches.push_back(ba);
#ifndef TESTSTENCIL
					m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
					m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
					c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
				}
				pt.x() += gi->adv;
			}    
		}
		else
		{
			u32 ls = 0, le = 0;
			c_string line;
			while(le < str.get_length())
			{
				if((le = str.find_first_of(L'\n', ls)) == NPOS)
					le = str.get_length();
				line = str.sub_str(ls, le - ls);
				ls = le + 1;
				if(fmt_ & TA_H_CENTER) 
				{
					f32 linew ,lineh;
					ft->measure_text(line, linew ,lineh);
					pt.x() = (r_.lt_pt().x() + r_.rb_pt().x()) / 2.f - linew / 2.f;
				}
				else if(fmt_ & TA_H_RIGHT) 
				{
					f32 linew ,lineh;
					ft->measure_text(line, linew ,lineh);
					pt.x() = r_.rb_pt().x() - linew;
				}
				else
					pt.x() = r_.lt_pt().x();
				for(u32 i = 0; i < line.get_length(); ++i)
				{
					utf16 c = line[i];
					if ( c == L'\r' || c == L'\n' )
						continue;
					c_font::s_glyph_info* gi = ft->get_glyph(MAKEU32(c, flag));
					if ( !gi->tex )
						continue;
					c_rect verts(pt.x()+gi->offset.x() , pt.y()+gi->offset.y() , 0.f , 0.f);
					verts.rb_pt().x() = verts.lt_pt().x() + (gi->rect.rb_pt().x() - gi->rect.lt_pt().x());
					verts.rb_pt().y() = verts.lt_pt().y() + (gi->rect.rb_pt().y() - gi->rect.lt_pt().y());
					c_rect cv = verts;
					c_rect tr;
					cv = verts;
					rect_clip(cv , cr_);
					f32 dx1 , dx2, dy1 , dy2;
					dx1 = (cv.lt_pt().x() - verts.lt_pt().x())/verts.width();
					dx2 = (cv.rb_pt().x() - verts.rb_pt().x())/verts.width();
					dy1 = (cv.lt_pt().y() - verts.lt_pt().y())/verts.height();
					dy2 = (cv.rb_pt().y() - verts.rb_pt().y())/verts.height();
					tr.lt_pt().x() = (gi->rect.lt_pt().x() + dx1*gi->rect.width())/256.f;
					tr.lt_pt().y() = (gi->rect.lt_pt().y() + dy1*gi->rect.height())/256.f;
					tr.rb_pt().x() = (gi->rect.rb_pt().x() + dx2*gi->rect.width())/256.f;
					tr.rb_pt().y() = (gi->rect.rb_pt().y() + dy2*gi->rect.height())/256.f;
					if(cv.lt_pt().x() < cv.rb_pt().x() && cv.lt_pt().y()  < cv.rb_pt().y())
					{
						c_color clr(clr_);
						s_batch ba;
						ba.tex = gi->tex;
						ba.vb.resize(4*8*sizeof(f32));
						ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
						ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
						ba.vb.write_f32(tr.lt_pt().x());
						ba.vb.write_f32(tr.lt_pt().y());
						ba.vb.write_f32(clr.r());
						ba.vb.write_f32(clr.g());
						ba.vb.write_f32(clr.b());
						ba.vb.write_f32(clr.a());
						ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
						ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().y()));
						ba.vb.write_f32(tr.rb_pt().x()); 
						ba.vb.write_f32(tr.lt_pt().y());
						ba.vb.write_f32(clr.r());
						ba.vb.write_f32(clr.g());
						ba.vb.write_f32(clr.b());
						ba.vb.write_f32(clr.a());
						ba.vb.write_f32(PIXEL_ALIGNED(cv.lt_pt().x()));
						ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y()));
						ba.vb.write_f32(tr.lt_pt().x());
						ba.vb.write_f32(tr.rb_pt().y());
						ba.vb.write_f32(clr.r());
						ba.vb.write_f32(clr.g());
						ba.vb.write_f32(clr.b());
						ba.vb.write_f32(clr.a());
						ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().x()));
						ba.vb.write_f32(PIXEL_ALIGNED(cv.rb_pt().y())); 
						ba.vb.write_f32(tr.rb_pt().x());
						ba.vb.write_f32(tr.rb_pt().y());
						ba.vb.write_f32(clr.r());
						ba.vb.write_f32(clr.g());
						ba.vb.write_f32(clr.b());
						ba.vb.write_f32(clr.a());
						m_batches.push_back(ba);
#ifndef TESTSTENCIL
						m_batch_gbo->uploadvb(0, ba.vb.get_pointer(), ba.vb.get_size());
						m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = ba.tex;
						c_device::get_singleton_ptr()->draw(m_ui_tech, m_batch_gbo);
#endif
					}
					pt.x() += gi->adv;
				}   
				pt.y() += fth_;
			}
		}

	}
	//--------------------------------------------------------
	void c_ui_manager::cache_batch(u32 fntid_, u32 fth_, const c_string& txt_, const c_float2& pt_, u32 clr_)
	{
		if(txt_.is_empty())
			return;
		c_rect r;
		r.lt_pt() = pt_;
		r.rb_pt().x() = pt_.x() + 9999.f;
		r.rb_pt().y() = pt_.y() + 9999.f;
		cache_batch(fntid_ , fth_ , 0 , true , txt_ , r , c_rect::Zero , clr_);
	}
	//--------------------------------------------------------
	void c_ui_manager::resize(f32 w_, f32 h_)
	{
		_width = w_;
		_height = h_;
		m_fbo->detach_color_buffer(0);
		if(m_billboard_loader)
		{
			c_resource_manager::get_singleton_ptr()->discard_resource(m_billboard_loader);
			delete m_billboard_loader;
		}
		m_billboard_loader = new c_tex_loader(true , w_, h_);
		c_resource_manager::get_singleton_ptr()->fetch_resource(m_billboard_loader);
		m_fbo->attach_color_buffer(0 , (c_texture*)m_billboard_loader->get_res() , 0);
		delete m_billboard_gbo;
		m_billboard_gbo = new c_geometry_buffer(RO_TRIANGLE_STRIP);
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
		c_stream data;
		data.resize(32*sizeof(f32));
		data.write_f32(0.f);
		data.write_f32(0.f);
#if !defined(O2D_USE_DX_API)
		data.write_f32(0.f);
		data.write_f32(1.f);
#else
		data.write_f32(0.f);
		data.write_f32(0.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(w_);
		data.write_f32(0.f);
#if !defined(O2D_USE_DX_API)
		data.write_f32(1.f);
		data.write_f32(1.f);
#else
		data.write_f32(1.f);
		data.write_f32(0.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(0.f);
		data.write_f32(h_);
#if !defined(O2D_USE_DX_API)
		data.write_f32(0.f);
		data.write_f32(0.f);
#else
		data.write_f32(0.f);
		data.write_f32(1.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(w_);
		data.write_f32(h_);
#if !defined(O2D_USE_DX_API)
		data.write_f32(1.f);
		data.write_f32(0.f);
#else
		data.write_f32(1.f);
		data.write_f32(1.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		m_billboard_gbo->bind_vertex_buffer(BU_STREAM_DRAW , data.get_pointer() , data.get_size() , fmts);
		m_dirty = true;
	}
	//--------------------------------------------------------
	void c_ui_manager::step(u32 dt_)
	{
		switch(c_input_manager::get_singleton_ptr()->get_input_type())
		{
		case IE_TOUCH_START:
			_inject_touch_start();
			break;
		case IE_TOUCH_FINISH:
			_inject_touch_finish();
			break;
		case IE_TOUCH_MOVE:
			_inject_touch_move();
			break;
		case IE_TOUCH_DOUBLE:
			_inject_double_tap();
			break;
		case IE_PAN:
			_inject_pan();
			break;
		case IE_CHAR:
			_inject_char();
			break;
		default:break;
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::draw(u32 dt_)
	{
		const c_float2& dim = c_device::get_singleton_ptr()->get_window()->get_dim();
		if(m_environment->m_absolute_rect.width() != dim.x() || m_environment->m_absolute_rect.height() != dim.y())
		{
			m_environment->m_desired_rect.rb_pt().x() = dim.x();
			m_environment->m_desired_rect.rb_pt().y() = dim.y();
			m_environment->m_clipping_rect = m_environment->m_desired_rect;
			m_environment->m_absolute_rect = m_environment->m_desired_rect;
			m_environment->_update_position();
			m_dirty = true;
		}
		f32* ssz = m_ui_tech->get_pass(0)->get_uniform(L"screensz")->var.fpvar;
		ssz[0] = 1/dim.x();
		ssz[1] = 1/dim.y();
		if(m_dirty)
		{
			c_device::get_singleton_ptr()->set_framebuffer(dim, m_fbo);
			m_fbo->clear(CM_COLOR);
			m_environment->_redraw();
			//FOREACH(TYPEOF(c_vector<s_batch>)::iterator , iter , m_batches)
			//{
				//m_batch_gbo->uploadvb(0 , iter->vb.get_pointer() , iter->vb.get_size());
				//m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = iter->tex;
				//c_device::get_singleton_ptr()->draw(m_ui_tech , m_batch_gbo);
			//}
			//while(!m_batches.empty())
//				m_batches.pop_back();
			m_batches.clear();
			m_dirty = false;
		}
		c_device::get_singleton_ptr()->set_framebuffer(dim);
		m_ui_tech->get_pass(0)->get_sampler(L"tex")->tex = (c_texture*)m_billboard_loader->get_res();
		c_device::get_singleton_ptr()->draw(m_ui_tech , m_billboard_gbo);
		m_environment->_update(m_ui_tech, dt_);
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_touch_move()
	{
		_update_hovered(c_input_manager::get_singleton_ptr()->get_touch_pos());
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_touch_move();	
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_touch_move();
		}
		if(!m_focus_widget && m_hovered_widget&&m_hovered_widget->is_visible())
		{
			_get_penetrate_widget(m_hovered_widget)->_on_touch_move();					
			c_widget* wid = _get_transmit_widget(m_hovered_widget);
			if(wid)
				wid->_on_touch_move();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_touch_start()
	{
		_update_hovered(c_input_manager::get_singleton_ptr()->get_touch_pos());
		if((m_hovered_widget && m_hovered_widget != m_focus_widget) || !m_focus_widget)
			set_focus(m_hovered_widget);
		c_widget* lastfocus = m_focus_widget;
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_touch_start();						
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_touch_start();
		}
		if(!lastfocus && m_hovered_widget&&m_hovered_widget->is_visible())
		{
			_get_penetrate_widget(m_hovered_widget)->_on_touch_start();							
			c_widget* wid = _get_transmit_widget(m_hovered_widget);
			if(wid)
				wid->_on_touch_start();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_touch_finish()
	{
		c_widget* lastfocus = m_focus_widget;
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_touch_finish();						
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_touch_finish();
		}
		if((!lastfocus && m_hovered_widget&&m_hovered_widget->is_visible()))
		{
			_get_penetrate_widget(m_hovered_widget)->_on_touch_finish();				
			c_widget* wid = _get_transmit_widget(m_hovered_widget);
			if(wid)
				wid->_on_touch_finish();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_double_tap()
	{
		c_widget* lastfocus = m_focus_widget;
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_double_tap();	
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_double_tap();
		}
		if((!lastfocus && m_hovered_widget&&m_hovered_widget->is_visible()))
		{
			_get_penetrate_widget(m_hovered_widget)->_on_double_tap();			
			c_widget* wid = _get_transmit_widget(m_hovered_widget);
			if(wid)
				wid->_on_double_tap();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_pan()
	{
		c_widget* lastfocus = m_focus_widget;
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_pan();	
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_pan();
		}
		if((!lastfocus && m_hovered_widget&&m_hovered_widget->is_visible()))
		{
			_get_penetrate_widget(m_hovered_widget)->_on_pan();		
			c_widget* wid = _get_transmit_widget(m_hovered_widget);
			if(wid)
				wid->_on_pan();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_inject_char()
	{
		if(m_focus_widget&&m_focus_widget->is_visible())
		{
			_get_penetrate_widget(m_focus_widget)->_on_char();			
			c_widget* wid = _get_transmit_widget(m_focus_widget);
			if(wid)
				wid->_on_char();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_update_hovered(const c_float2& pos_)
	{
		c_widget* lasthovered = m_hovered_widget;
		m_hovered_widget = m_environment->get_widget(pos_);
		if(m_hovered_widget != lasthovered)
		{
			if(lasthovered)
				lasthovered->_sig_leave.emit();
			if(m_hovered_widget)
				m_hovered_widget->_sig_enter.emit();
		}
	}
	//--------------------------------------------------------
	void c_ui_manager::_build_geo_buffer(f32 w_, f32 h_)
	{
		m_batch_gbo = new c_geometry_buffer(RO_TRIANGLE_STRIP);
		m_line_gbo = new c_geometry_buffer(RO_LINE_STRIP);
		m_billboard_gbo = new c_geometry_buffer(RO_TRIANGLE_STRIP);
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
		c_stream data;
		data.resize(32*sizeof(f32));
		data.write_f32(0.f);
		data.write_f32(0.f);
#if !defined(O2D_USE_DX_API)
		data.write_f32(0.f);
		data.write_f32(1.f);
#else
		data.write_f32(0.f);
		data.write_f32(0.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(w_);
		data.write_f32(0.f);
#if !defined(O2D_USE_DX_API)
		data.write_f32(1.f);
		data.write_f32(1.f);
#else
		data.write_f32(1.f);
		data.write_f32(0.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(0.f);
		data.write_f32(h_);
#if !defined(O2D_USE_DX_API)
		data.write_f32(0.f);
		data.write_f32(0.f);
#else
		data.write_f32(0.f);
		data.write_f32(1.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(w_);
		data.write_f32(h_);
#if !defined(O2D_USE_DX_API)
		data.write_f32(1.f);
		data.write_f32(0.f);
#else
		data.write_f32(1.f);
		data.write_f32(1.f);
#endif
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		data.write_f32(1.f);
		m_batch_gbo->bind_vertex_buffer(BU_STREAM_DRAW ,data.get_pointer() , data.get_size() , fmts);
		m_billboard_gbo->bind_vertex_buffer(BU_STREAM_DRAW , data.get_pointer() , data.get_size() , fmts);
		m_line_gbo->bind_vertex_buffer(BU_STREAM_DRAW, data.get_pointer(), data.get_size(), fmts);
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::_get_penetrate_widget(c_widget* wid_)
	{
		c_widget* wid = wid_;
		while(wid->is_penetrate())
			wid = wid->get_parent();
		return wid;
	}
	//--------------------------------------------------------
	c_widget* c_ui_manager::_get_transmit_widget(c_widget* wid_)
	{
		c_widget* wid = wid_;
		while(wid->is_transmit())
			wid = wid->get_parent();
		if(wid == wid_)
			return nullptr;
		return wid;
	}
}