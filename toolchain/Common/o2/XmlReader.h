/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef XMLREADER_H_INCLUDE
#define XMLREADER_H_INCLUDE
#include "Str.h"
#include "Stream.h"
#include "rapidxml.hpp"
namespace o2d{
	class O2D_API c_xml_reader
	{
	private:
		rapidxml::xml_document<utf8>* m_xml_document;
		rapidxml::xml_node<utf8>* m_cur_node;
	public:
		c_xml_reader();
		~c_xml_reader();
	public:
		bool parse(u8* data_, bool full_);
		bool is_leaf(const utf8* name_, u32 sz_);
		bool is_the_node(const c_string& name_);
		bool is_the_node(const utf8* name_, u32 sz_);
		bool set_to_first_child(const c_string& name_ = c_string::EmptyStr);
		bool set_to_first_child(const utf8* name_, u32 sz_);
		bool set_to_next_sibling(const c_string& name_ = c_string::EmptyStr);
		bool set_to_next_sibling(const utf8* name_, u32 sz_);
		bool set_to_parent();
		bool retrieve();
		bool has_attribute(const c_string& attr_) const;
		bool has_attribute(const utf8* attr_, u32 sz_) const;
		u32 get_depth() const;
		c_string get_current_node_name() const;
		c_vector<c_string> get_attributes() const;
		void get_cdata(c_string& str_) const;
		void get_cdata(c_stream& ss_) const;
		c_string get_attribute_as_string(const c_string& attr_) const;
		c_string get_attribute_as_string(const utf8* attr_, u32 sz_) const;
		bool get_attribute_as_boolean(const c_string& attr_) const;
		bool get_attribute_as_boolean(const utf8* attr_, u32 sz_) const;
		s32 get_attribute_as_integer(const c_string& attr_) const;
		s32 get_attribute_as_integer(const utf8* attr_, u32 sz_) const;
		u32 get_attribute_as_uinteger(const c_string& attr_) const;
		u32 get_attribute_as_uinteger(const utf8* attr_, u32 sz_) const;
		f32 get_attribute_as_float(const c_string& attr_) const;
		f32 get_attribute_as_float(const utf8* attr_, u32 sz_) const;
	};
}
#endif