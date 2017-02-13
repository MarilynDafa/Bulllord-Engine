/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "XmlReader.h"
#include "System.h"
#include "Tools.h"
namespace o2d{
	c_xml_reader::c_xml_reader():m_cur_node(nullptr)
	{
		m_xml_document = new rapidxml::xml_document<utf8>;
	}
	//--------------------------------------------------------
	c_xml_reader::~c_xml_reader()
	{
		delete m_xml_document;
	}
	//--------------------------------------------------------
	bool c_xml_reader::parse(u8* data_, bool full_)
	{
		if(full_)
		{
			if(!m_xml_document->parse<rapidxml::parse_full>((u8*)data_))
				return false;
		}
		else
		{
			if(!m_xml_document->parse<rapidxml::parse_fastest>((u8*)data_))
				return false;
		}
		m_cur_node = m_xml_document->first_node();
		return true;
	}
	//--------------------------------------------------------
	bool c_xml_reader::is_leaf(const utf8* name_, u32 sz_)
	{
		if(name_)
			return m_cur_node->first_node(name_ , sz_ - 1)?false:true;
		else
			return m_cur_node->first_node()?false:true;
	}
	//--------------------------------------------------------
	bool c_xml_reader::is_the_node(const c_string& name_)
	{
		const utf8* nn = name_.to_utf8();
		u32 ns = name_.utf8_size();
		return !strncmp((const ansi*)m_cur_node->name() , (const ansi*)nn , ns-1);
	}
	//--------------------------------------------------------
	bool c_xml_reader::is_the_node(const utf8* name_, u32 sz_)
	{
		return !strncmp((const ansi*)m_cur_node->name() , (const ansi*)name_ , sz_-1);
	}
	//--------------------------------------------------------
	bool c_xml_reader::set_to_first_child(const c_string& name_ /*= String::EmptyStr*/)
	{
		rapidxml::xml_node<utf8>* child = 0;
		if(name_.is_empty())
			child = m_cur_node->first_node();
		else
		{
			const utf8* nn = name_.to_utf8();
			u32 ns = name_.utf8_size();
			child = m_cur_node->first_node(nn , ns);
		}
		if(child)
		{
			m_cur_node = child;
			return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------
	bool c_xml_reader::set_to_first_child(const utf8* name_, u32 sz_)
	{
		rapidxml::xml_node<utf8>* child = 0;
		if(!name_)
			child = m_cur_node->first_node();
		else
			child = m_cur_node->first_node(name_ , sz_-1);
		if(child)
		{
			m_cur_node = child;
			return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------
	bool c_xml_reader::set_to_next_sibling(const c_string& name_ /*= String::EmptyStr*/)
	{
		rapidxml::xml_node<utf8>* sib = 0;
		if(name_.is_empty())
			sib = m_cur_node->next_sibling();
		else
		{
			const utf8* nn = name_.to_utf8();
			u32 ns = name_.utf8_size();
			sib = m_cur_node->next_sibling(nn , ns);
		}
		if(sib)
		{
			m_cur_node = sib;
			return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------
	bool c_xml_reader::set_to_next_sibling(const utf8* name_, u32 sz_)
	{
		rapidxml::xml_node<utf8>* sib = 0;
		if(!name_)
			sib = m_cur_node->next_sibling();
		else
			sib = m_cur_node->next_sibling(name_, sz_-1);
		if(sib)
		{
			m_cur_node = sib;
			return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------
	bool c_xml_reader::set_to_parent()
	{
		if(m_cur_node == m_xml_document->last_node())
			return false;
		rapidxml::xml_node<utf8>* parent = m_cur_node->parent();
		if(parent)
		{
			m_cur_node = parent;
			return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------
	bool c_xml_reader::retrieve()
	{
		if(set_to_first_child())
			return true;
		else if(set_to_next_sibling())
			return true;
		else
		{
			while(1)
			{
				if(!set_to_parent())
					return false;
				if(set_to_next_sibling())
					return true;
			}
		}
	}
	//--------------------------------------------------------
	bool c_xml_reader::has_attribute(const c_string& attr_) const
	{
		const utf8* nn = attr_.to_utf8();
		u32 ns = attr_.utf8_size();
		return (0 != m_cur_node->first_attribute(nn , ns));
	}
	//--------------------------------------------------------
	bool c_xml_reader::has_attribute(const utf8* attr_ , u32 sz_) const
	{
		return (0 != m_cur_node->first_attribute(attr_ , sz_-1));
	}
	//--------------------------------------------------------
	u32 c_xml_reader::get_depth() const
	{
		u32 depth = 0;
		rapidxml::xml_node<utf8>* parent = m_cur_node->parent();
		while(parent)
		{
			depth++;
			parent = parent->parent();
		}
		return depth;
	}
	//--------------------------------------------------------
	c_string c_xml_reader::get_current_node_name() const
	{
		return c_string(m_cur_node->name(), (u32)m_cur_node->name_size());
	}
	//--------------------------------------------------------
	c_vector<c_string> c_xml_reader::get_attributes() const
	{
		c_vector<c_string> res;
		rapidxml::xml_attribute<utf8>* attr = m_cur_node->first_attribute();
		while(0 != attr)
		{
			c_string att(attr->value(), (u32)attr->value_size());
			res.push_back(att);
			attr = attr->next_attribute();
		}
		return res;
	}
	//--------------------------------------------------------
	void c_xml_reader::get_cdata(c_string& str_) const
	{
		str_.clear();
		utf8* val = m_cur_node->value();  
		u32 sz = (u32)m_cur_node->value_size();
		assert(val!=nullptr);
		str_.append(val, sz);
	}
	//--------------------------------------------------------
	void c_xml_reader::get_cdata(c_stream& ss_) const
	{
		ss_.reset();
		utf8* val = m_cur_node->value();  
		u32 sz = (u32)m_cur_node->value_size();
		ss_.resize(sz);
		ss_.read_buf(val , sz);
	}
	//--------------------------------------------------------
	c_string c_xml_reader::get_attribute_as_string(const c_string& attr_) const
	{
		c_string str;
		const utf8* nn = attr_.to_utf8();
		u32 ns = attr_.utf8_size();
		utf8* val = m_cur_node->first_attribute(nn, ns)->value();  
		u32 sz = (u32)m_cur_node->first_attribute(nn, ns)->value_size();
		if (0 == val)
		{
			c_string msg(m_cur_node->value(), (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s", msg.c_str());
		}
		else
			str.append(val, sz);
		return str;
	}
	//--------------------------------------------------------
	c_string c_xml_reader::get_attribute_as_string(const utf8* attr_, u32 sz_) const
	{
		c_string str;
		utf8* val = m_cur_node->first_attribute(attr_, sz_-1)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(attr_, sz_-1)->value_size();
		if (!val)
		{
			c_string msg(m_cur_node->value(), (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s" , msg.c_str());
		}
		else
			str.append(val, vsz);
		return str;
	}
	//--------------------------------------------------------
	bool c_xml_reader::get_attribute_as_boolean(const c_string& attr_) const
	{
		return get_attribute_as_string(attr_).to_boolean();
	}
	//--------------------------------------------------------
	bool c_xml_reader::get_attribute_as_boolean(const utf8* attr_, u32 sz_) const
	{
		return get_attribute_as_string(attr_ , sz_).to_boolean();
	}
	//--------------------------------------------------------
	s32 c_xml_reader::get_attribute_as_integer(const c_string& attr_) const
	{
		const utf8* nn = attr_.to_utf8();
		u32 ns = attr_.utf8_size();
		utf8* val = m_cur_node->first_attribute(nn, ns)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(nn, ns)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value(), (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s", msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf, (const ansi*)val, vsz);
		buf[vsz] = 0;
		return (s32)strtol(buf , nullptr , 0);
	}
	//--------------------------------------------------------
	s32 c_xml_reader::get_attribute_as_integer(const utf8* attr_, u32 sz_) const
	{
		utf8* val = m_cur_node->first_attribute(attr_, sz_-1)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(attr_, sz_-1)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value(), (u32)m_cur_node->value_size());
			LOG(LL_ERROR, "xml doc doesn't exist %s", msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf, (const ansi*)val, vsz);
		buf[vsz] = 0;
		return (s32)strtol(buf , nullptr , 0);
	}
	//--------------------------------------------------------
	u32 c_xml_reader::get_attribute_as_uinteger(const c_string& attr_) const
	{
		const utf8* nn = attr_.to_utf8();
		u32 ns = attr_.utf8_size();
		utf8* val = m_cur_node->first_attribute(nn ,ns)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(nn , ns)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value() , (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s" , msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf , (const ansi*)val , vsz);
		buf[vsz] = 0;
		return (u32)strtoul(buf , nullptr , 0);
	}
	//--------------------------------------------------------
	u32 c_xml_reader::get_attribute_as_uinteger(const utf8* attr_, u32 sz_) const
	{
		utf8* val = m_cur_node->first_attribute(attr_ , sz_-1)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(attr_ , sz_-1)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value() , (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s" , msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf , (const ansi*)val , vsz);
		buf[vsz] = 0;
		return (u32)strtoul(buf , nullptr , 0);
	}
	//--------------------------------------------------------
	f32 c_xml_reader::get_attribute_as_float(const c_string& attr_) const
	{
		const utf8* nn = attr_.to_utf8();
		u32 ns = attr_.utf8_size();
		utf8* val = m_cur_node->first_attribute(nn ,ns)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(nn , ns)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value() , (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s" , msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf , (const ansi*)val , vsz);
		buf[vsz] = 0;
		f32 ret = atof(buf);
		return ret;
	}
	//--------------------------------------------------------
	f32 c_xml_reader::get_attribute_as_float(const utf8* attr_, u32 sz_) const
	{
		utf8* val = m_cur_node->first_attribute(attr_ , sz_-1)->value();  
		u32 vsz = (u32)m_cur_node->first_attribute(attr_ , sz_-1)->value_size();
		if(!val)
		{
			c_string msg(m_cur_node->value() , (u32)m_cur_node->value_size());
			LOG(LL_ERROR , "xml doc doesn't exist %s" , msg.c_str());
			return 0;
		}
		ansi buf[32];
		strncpy(buf , (const ansi*)val , vsz);
		buf[vsz] = 0;
		f32 ret = atof(buf);
		return ret;
	}
}