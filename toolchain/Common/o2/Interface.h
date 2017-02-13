/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef INTERFACE_H_INCLUDE
#define INTERFACE_H_INCLUDE
#include "Widget.h"
#include "ManagedRes.h"
namespace o2d{
	class O2D_API c_interface:public c_resource
	{
	private:
		c_widget* m_root;
	public:
		c_interface(const utf16* filename_);
		virtual ~c_interface();		
	protected:
		bool _load_impl();
		void _unload_impl();
	private:
		e_widget_type _str_2_wtype(const c_string& str_) const;
		e_text_alignment _str_2_align(const c_string& str_) const;
		c_rect _str_2_rect(const c_string& str_) const;
		c_float2 _str_2_size(const c_string& str_) const;
	};
}
#endif