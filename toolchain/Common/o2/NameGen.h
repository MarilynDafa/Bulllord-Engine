/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef NAMEGEN_H_INCLUDE
#define NAMEGEN_H_INCLUDE
#include "Str.h"
#include "Mutex.h"
namespace o2d{
	class O2D_API c_name_generator
	{
	protected:
		c_string m_prefix;
		c_mutex m_mutex;
		u64 m_next;
	public:
		c_name_generator(const c_name_generator& rhs_);
		c_name_generator(const c_string& prefix_);
		~c_name_generator();
	public:
		c_string generate();
		void reset();
		void set_next(u64 val_);
		u64 get_next() const;
	};
}
#endif