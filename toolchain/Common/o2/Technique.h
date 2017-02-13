/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TECHNIQUE_H_INCLUDE
#define TECHNIQUE_H_INCLUDE
#include "Pass.h"
namespace o2d{
	class O2D_API c_technique
	{
		FRIEND_CLASS(c_shader)
	private:
		c_vector<c_pass*> m_passes;
	public:
		c_technique();
		~c_technique();
	public:
		u32 num_pass() const;
		c_pass* add_pass(const c_string& name_);
		c_pass* get_pass(u32 index_);
		c_pass* get_pass(const c_string& name_);
	};
}
#endif