/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "FileMgr.h"
#include "Label.h"
#include "Button.h"
#include "CartoonBox.h"
#include "Dialog.h"
#include "DragDropSlot.h"
#include "EditBox.h"
#include "ListBox.h"
#include "ProgressBar.h"
#include "Radio.h"
#include "Slider.h"
#include "ScrollBox.h"
#include "TabCtl.h"
#include "TableBox.h"
#include "TreeBox.h"
#include "RichBox.h"
#include "Widget.h"
#include "UIMgr.h"
#include "Interface.h"
namespace o2d{
	c_interface::c_interface(const utf16* filename_)
		:c_resource(filename_ , RT_UI) , 
		m_root(nullptr)
	{}
	//--------------------------------------------------------
	c_interface::~c_interface(){}
	//--------------------------------------------------------
	bool c_interface::_load_impl()
	{
		c_xml_reader xml;
		if(!c_file_manager::get_singleton_ptr()->obtain_xml_reader(m_filename , xml , true))
			return false;
		bool root = true;
		e_widget_type wt;
		c_widget* curnode = nullptr;
		do 
		{
			if(xml.is_the_node((const utf8*)"element", 8))
			{
				c_string cla = xml.get_attribute_as_string((const utf8*)"class" , 6);
				wt = _str_2_wtype(cla);
				u32 pid = xml.get_attribute_as_uinteger((const utf8*)"parentid" , 9);
				c_widget* parent = nullptr;
				if(pid != hash(L"environment"))
					parent = c_ui_manager::get_singleton_ptr()->get_environment()->get_widget(pid , true);
				c_widget::s_widget_property pro;
				pro.name = xml.get_attribute_as_string((const utf8*)"name" , 5);
				pro.skinid = xml.get_attribute_as_uinteger((const utf8*)"skinid" , 7);
				pro.fntid = xml.get_attribute_as_uinteger((const utf8*)"fntid" , 6);
				pro.font_height = xml.get_attribute_as_integer((const utf8*)"fntheight" , 10);
				c_string reg = xml.get_attribute_as_string((const utf8*)"region" , 7);
				pro.dim = _str_2_rect(reg);
				curnode = c_ui_manager::get_singleton_ptr()->add_widget(wt , parent , pro);
				if(root)
				{
					m_root = curnode;
					root = false;
				}
			}
			else if(xml.is_the_node((const utf8*)"base", 5))
			{
				c_string minsz = xml.get_attribute_as_string((const utf8*)"minsz" , 6);
				c_string maxsz = xml.get_attribute_as_string((const utf8*)"maxsz" , 6);
				curnode->set_min_size(_str_2_size(minsz));
				curnode->set_max_size(_str_2_size(maxsz));
				curnode->set_tooltip(xml.get_attribute_as_string((const utf8*)"tooltip" , 8));
				curnode->set_visible(xml.get_attribute_as_boolean((const utf8*)"visible" , 8));
				curnode->set_no_clipped(xml.get_attribute_as_boolean((const utf8*)"noclip" , 7));
				curnode->set_transmit(xml.get_attribute_as_boolean((const utf8*)"transmit" , 9));
				curnode->set_penetrate(xml.get_attribute_as_boolean((const utf8*)"penetrate" , 10));
			}
			else if(xml.is_the_node((const utf8*)"ext", 4))
			{
				switch(wt)
				{
				case WT_LABEL:
					{
						c_label* la = ((c_label*)curnode);
						la->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						la->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						la->set_text(xml.get_attribute_as_string((const utf8*)"text" , 5));
						la->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));
						la->set_wordwrap(xml.get_attribute_as_boolean((const utf8*)"wordwrap" ,9));
						c_string va = xml.get_attribute_as_string((const utf8*)"valign" , 7);
						c_string ha = xml.get_attribute_as_string((const utf8*)"halign" , 7);
						la->set_text_align(_str_2_align(ha) , _str_2_align(va));
 						c_string margin = xml.get_attribute_as_string((const utf8*)"margin" ,7);
 						la->set_margin(_str_2_size(margin));
					}
					break;
				case WT_BUTTON:
					{
						c_string reg;
						c_button* bu = ((c_button*)curnode);
						bu->set_normal_image(xml.get_attribute_as_string((const utf8*)"nimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"nzoomreg" , 9);
						bu->set_nor_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						bu->set_hovered_image(xml.get_attribute_as_string((const utf8*)"himage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"hzoomreg" , 9);
						bu->set_hov_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						bu->set_pressed_image(xml.get_attribute_as_string((const utf8*)"pimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"pzoomreg" , 9);
						bu->set_pre_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						bu->set_disable_image(xml.get_attribute_as_string((const utf8*)"dimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"dzoomreg" , 9);
						bu->set_dis_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						bu->set_enable(xml.get_attribute_as_boolean((const utf8*)"enable" ,7));
						bu->set_text(xml.get_attribute_as_string((const utf8*)"text" , 5));
						bu->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));
						c_string va = xml.get_attribute_as_string((const utf8*)"valign" , 7);
						c_string ha = xml.get_attribute_as_string((const utf8*)"halign" , 7);
						bu->set_text_align(_str_2_align(ha) , _str_2_align(va));
					}
					break;
				case WT_RADIO:
					{
						c_string reg;
 						c_radio* ra = ((c_radio*)curnode);
 						ra->set_normal_image(xml.get_attribute_as_string((const utf8*)"nimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"nzoomreg" , 9);
						ra->set_nor_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						ra->set_pressed_image(xml.get_attribute_as_string((const utf8*)"pimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"pzoomreg" , 9);
						ra->set_pre_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						ra->set_disable_image(xml.get_attribute_as_string((const utf8*)"dimage" ,7));
						reg = xml.get_attribute_as_string((const utf8*)"dzoomreg" , 9);
						ra->set_dis_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						ra->set_pushed(xml.get_attribute_as_boolean((const utf8*)"pushed" ,7));
 						ra->set_enable(xml.get_attribute_as_boolean((const utf8*)"enable" ,7));
						ra->set_text(xml.get_attribute_as_string((const utf8*)"text" , 5));
						ra->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));
						c_string va = xml.get_attribute_as_string((const utf8*)"valign" , 7);
						c_string ha = xml.get_attribute_as_string((const utf8*)"halign" , 7);
						ra->set_text_align(_str_2_align(ha) , _str_2_align(va));
					}
					break;
				case  WT_PROGRESSBAR:
					{
 						c_progressbar* pb = ((c_progressbar*)curnode);
 						pb->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						pb->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						pb->set_fill_image(xml.get_attribute_as_string((const utf8*)"fillimage" ,10));
 						pb->set_percent(xml.get_attribute_as_float((const utf8*)"per" ,4));
						c_string offset = xml.get_attribute_as_string((const utf8*)"offset" ,7);
						pb->set_offset(_str_2_size(offset));
						pb->set_text(xml.get_attribute_as_string((const utf8*)"text" , 5));
						pb->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));						
					}
					break;
				case WT_EDITBOX:
					{
 						c_editbox* eb = ((c_editbox*)curnode);
 						eb->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						eb->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						eb->set_max(xml.get_attribute_as_integer((const utf8*)"maxc" ,5));
 						c_string range = xml.get_attribute_as_string((const utf8*)"range" ,6);
 						eb->set_range(_str_2_size(range));
 						c_string margin = xml.get_attribute_as_string((const utf8*)"margin" ,7);
 						eb->set_margin(_str_2_size(margin));
 						eb->set_wordwrap(xml.get_attribute_as_boolean((const utf8*)"wordwrap" ,9));
 						eb->set_multiline(xml.get_attribute_as_boolean((const utf8*)"multiline" ,10));
 						eb->set_autoscroll(xml.get_attribute_as_boolean((const utf8*)"autoscroll" ,11));
 						eb->set_password_box(xml.get_attribute_as_boolean((const utf8*)"password" ,9));
 						eb->set_numeric_box(xml.get_attribute_as_boolean((const utf8*)"numeric" ,8));
 						eb->set_enable(xml.get_attribute_as_boolean((const utf8*)"enable" ,7));
						c_string va = xml.get_attribute_as_string((const utf8*)"valign" , 7);
						c_string ha = xml.get_attribute_as_string((const utf8*)"halign" , 7);
						eb->set_text_align(_str_2_align(ha) , _str_2_align(va));
						eb->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));		
					}
					break;
				case WT_DIALOG:
					{
 						c_dialog* dlg = ((c_dialog*)curnode);
 						dlg->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						dlg->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						dlg->set_title_dim(c_float2(dlg->get_title_dim().x() , xml.get_attribute_as_float((const utf8*)"titleheight" ,12)));
					}
					break;
				case WT_SLIDER:
					{
 						c_slider* sl = ((c_slider*)curnode);
 						sl->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						sl->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						sl->set_thumb_image(xml.get_attribute_as_string((const utf8*)"thumbimage" ,11));
 						sl->set_thumb_disable_image(xml.get_attribute_as_string((const utf8*)"thumbdimage" ,12));
 						c_string size = xml.get_attribute_as_string((const utf8*)"thumbsz" ,8);
 						sl->set_thumb_size(_str_2_size(size));
 						c_string range = xml.get_attribute_as_string((const utf8*)"range" ,6);
 						sl->set_range(_str_2_size(range));
 						sl->set_step(xml.get_attribute_as_integer((const utf8*)"step" ,5));
 						sl->set_position(xml.get_attribute_as_integer((const utf8*)"pos" ,4));
 						sl->set_horizontal(xml.get_attribute_as_boolean((const utf8*)"horizontal" ,11));
 						sl->set_enable(xml.get_attribute_as_boolean((const utf8*)"enable" ,7));
					}
					break;
				case WT_LISTBOX:
					{
 						c_listbox* lb = ((c_listbox*)curnode);
 						lb->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						lb->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
 						lb->set_row_height(xml.get_attribute_as_integer((const utf8*)"rowheight" ,10));
						s32 num = xml.get_attribute_as_integer((const utf8*)"listnum" ,8);
						for(s32 i = 0 ; i < num ; ++i)
						{
							c_string tag = L"ele";
							tag+=c_string(i);
							const utf8* tagstr = tag.to_utf8();
							lb->add_item(xml.get_attribute_as_string(tagstr , tag.get_length()+1));
						}
						lb->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));	
					}
					break;
				case WT_TABCTL:
					{
						c_tabctl* tab = ((c_tabctl*)curnode);
 						tab->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						tab->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						tab->set_tab_aimage(xml.get_attribute_as_string((const utf8*)"aheaderimg" ,11));
 						tab->set_tab_nimage(xml.get_attribute_as_string((const utf8*)"nheaderimg" ,11));
						tab->set_page_offset(xml.get_attribute_as_integer((const utf8*)"pageoffset" ,11));
						tab->set_active(xml.get_attribute_as_integer((const utf8*)"active" , 7));
						tab->set_tab_posup(xml.get_attribute_as_boolean((const utf8*)"tabup" , 6));
						tab->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));		
						s32 pagenum = xml.get_attribute_as_integer((const utf8*)"pagenum" , 8);
						for(s32 i = 0 ; i < pagenum ; ++i)
						{
							c_string tag = L"ele";
							tag+=c_string(i);
							const utf8* tagstr = tag.to_utf8();
							tab->add_tab(xml.get_attribute_as_string(tagstr , tag.get_length()+1));
						}
					}
					break;
				case WT_CARTOONBOX:
					{
 						c_cartoonbox* car = ((c_cartoonbox*)curnode);
 						car->set_loop(xml.get_attribute_as_boolean((const utf8*)"loop" ,5));
						car->set_framerate(xml.get_attribute_as_integer((const utf8*)"framerate" ,10));
					}
					break;
				case WT_TABLEBOX:
					{
						c_tablebox* car = ((c_tablebox*)curnode);
 						car->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						car->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						car->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));	
						bool rs = xml.get_attribute_as_boolean((const utf8*)"rowsep" , 7);	
						bool cs = xml.get_attribute_as_boolean((const utf8*)"colsep" , 7);
						car->set_separator(rs , cs);
						s32 num = xml.get_attribute_as_integer((const utf8*)"colnum" , 7);
						for(s32 i = 0 ; i < num ; ++i)
						{
							c_string tag = L"ele";
							tag+=c_string(i);
							const utf8* tagstr = tag.to_utf8();
							car->add_column(xml.get_attribute_as_string(tagstr , tag.get_length()+1) , i);
						}
					}
					break;
				case WT_DRAGDROPSLOT:
					{
						c_dragdrop_slot* dds = ((c_dragdrop_slot*)curnode);
 						dds->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string va = xml.get_attribute_as_string((const utf8*)"valign" , 7);
						c_string ha = xml.get_attribute_as_string((const utf8*)"halign" , 7);
						dds->set_text_align(_str_2_align(ha) , _str_2_align(va));
						dds->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));	
					}
					break;
				case WT_SCROLLBOX:
					{
						c_scrollbox* sb = ((c_scrollbox*)curnode);
						sb->set_strict(xml.get_attribute_as_boolean((const utf8*)"strict" ,7));
					}
					break;
				case WT_TREEBOX:
					{
						c_treebox* tb = ((c_treebox*)curnode);
						tb->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						tb->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						tb->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));	
						tb->set_lines_visible(xml.get_attribute_as_boolean((const utf8*)"drawl" , 6));	
						tb->set_xoffset(xml.get_attribute_as_integer((const utf8*)"xoffset" , 8));	
					}
					break;
				case WT_RICHBOX:
					{
						c_richbox* rb = ((c_richbox*)curnode);
						rb->set_bg_image(xml.get_attribute_as_string((const utf8*)"bgimage" ,8));
						c_string reg = xml.get_attribute_as_string((const utf8*)"bgzoomreg" , 10);
						rb->set_bg_fragment(_str_2_rect(reg).lt_pt() , _str_2_rect(reg).rb_pt());
						rb->set_text_color(xml.get_attribute_as_uinteger((const utf8*)"txtcolor" , 9));	
						rb->set_text(xml.get_attribute_as_string((const utf8*)"text" , 5));
						c_string margin = xml.get_attribute_as_string((const utf8*)"margin" ,7);
						rb->set_margin(_str_2_size(margin));
					}
					break;
				default:assert(0);break;
				}
			}
			else;
		} while (xml.retrieve());
		return true;
	}
	//--------------------------------------------------------
	void c_interface::_unload_impl()
	{
		if(m_root)
			c_ui_manager::get_singleton_ptr()->remove_widget(m_root);
	}
	//--------------------------------------------------------
	e_widget_type c_interface::_str_2_wtype(const c_string& str_) const
	{
		if(str_ == L"lab")
			return WT_LABEL;
		else if(str_ == L"dlg")
			return WT_DIALOG;
		else if(str_ == L"btn")
			return WT_BUTTON;
		else if(str_ == L"rad")
			return WT_RADIO;
		else if(str_ == L"tab")
			return WT_TABCTL;
		else if(str_ == L"ctn")
			return WT_CARTOONBOX;
		else if(str_ == L"edi")
			return WT_EDITBOX;
		else if(str_ == L"pro")
			return WT_PROGRESSBAR;
		else if(str_ == L"sli")
			return WT_SLIDER;
		else if(str_ == L"dds")
			return WT_DRAGDROPSLOT;
		else if(str_ == L"lib")
			return WT_LISTBOX;
		else if(str_ == L"tbb")
			return WT_TABLEBOX;
		else if(str_ == L"trb")
			return WT_TREEBOX;
		else if(str_ == L"rcb")
			return WT_RICHBOX;
		else if(str_ == L"sbb")
			return WT_SCROLLBOX;
		else
		{
			assert(0);
			return WT_WIDGET;
		}
	}
	//--------------------------------------------------------
	e_text_alignment c_interface::_str_2_align(const c_string& str_) const
	{
		if(str_ == L"left")
			return TA_H_LEFT;
		else if(str_ == L"right")
			return TA_H_RIGHT;
		else if(str_ == L"center")
			return TA_H_CENTER;
		else if(str_ == L"top")
			return TA_V_TOP;
		else if(str_ == L"middle")
			return TA_V_CENTER;
		else if(str_ == L"bottom")
			return TA_V_BOTTOM;
		else
		{
			assert(0);
			return TA_V_CENTER;
		}
	}
	//--------------------------------------------------------
	c_rect c_interface::_str_2_rect(const c_string& str_) const
	{
		c_vector<c_string> rect;
		str_.tokenize(L"," , rect);
		c_rect ret;
		ret.lt_pt().x() = rect[0].to_float();
		ret.lt_pt().y() = rect[1].to_float();
		ret.rb_pt().x() = rect[2].to_float();
		ret.rb_pt().y() = rect[3].to_float();
		return ret;
	}
	//--------------------------------------------------------
	c_float2 c_interface::_str_2_size(const c_string& str_) const
	{
		c_vector<c_string> size;
		str_.tokenize(L"," , size);
		c_float2 ret;
		ret.x() = size[0].to_float();
		ret.y() = size[1].to_float();
		return ret;
	}
}