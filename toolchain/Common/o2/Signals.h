/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SIGNALS_H_INCLUDE
#define SIGNALS_H_INCLUDE
#include "List.h"
#include "FastDelegate.h"
#include "FastDelegateBind.h"
#ifdef emit
#	pragma push_macro("emit")
#	undef emit
#endif
namespace o2d{
	class Nill;
	//////////////////////////////////////////////////////////////////////////
	template<typename _Rt , typename _P1 = Nill , typename _P2 = Nill , typename _P3 = Nill>
	class c_signal
	{
	private:
		typedef fastdelegate::FastDelegate3<_P1 , _P2 , _P3 , _Rt> base_type;
		c_list<base_type> m_list;
	public:
		c_signal();
		void emit(_P1 p1_ , _P2 p2_ , _P3 p3_) const;
		template<typename _O , typename _F>
		void connect(_O obj_ , _F func_);
		template<typename _O , typename _F>
		void disconnect(_O obj_ , _F func_);
		bool vaild() const;
	};
	//--------------------------------------------------------
	template<typename _Rt , typename _P1 , typename _P2 , typename _P3>
	c_signal<_Rt , _P1 , _P2 , _P3>::c_signal(){}
	//--------------------------------------------------------
	template<typename _Rt , typename _P1 , typename _P2 , typename _P3>
	void c_signal<_Rt , _P1 , _P2 , _P3>::emit(_P1 p1_ , _P2 p2_ , _P3 p3_) const
	{
		for(typename c_list<base_type>::const_iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
			(*iter).operator()(p1_ , p2_ , p3_);
	}
	//--------------------------------------------------------
	template<typename _Rt , typename _P1 , typename _P2 , typename _P3>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , _P2 , _P3>::connect(_O obj_ , _F func_)
	{
		m_list.push_back(fastdelegate::MakeDelegate(obj_ , func_));
	}
	//--------------------------------------------------------
	template<typename _Rt , typename _P1 , typename _P2 , typename _P3>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , _P2 , _P3>::disconnect(_O obj_ , _F func_)
	{
		base_type temp = fastdelegate::MakeDelegate(obj_ , func_);
		for(typename c_list<base_type>::iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
		{
			if((*iter) == temp)
			{
				m_list.erase(iter);
				break;
			}
		}
	}
	//--------------------------------------------------------
	template<typename _Rt , typename _P1 , typename _P2 , typename _P3>
	bool c_signal<_Rt , _P1 , _P2 , _P3>::vaild() const
	{
		return !m_list.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename _Rt>
	class c_signal<_Rt , Nill , Nill , Nill>
	{
	private:
		typedef fastdelegate::FastDelegate0<_Rt> base_type;
		c_list<base_type> m_list;
	public:
		c_signal();
		void emit() const;
		template<typename _O , typename _F>
		void connect(_O obj_ , _F func_);
		template<typename _O , typename _F>
		void disconnect(_O obj_ , _F func_);
		bool vaild() const;
	};
	//--------------------------------------------------------
	template<typename _Rt>
	c_signal<_Rt , Nill , Nill , Nill>::c_signal(){}
	//--------------------------------------------------------
	template<typename _Rt>
	void c_signal<_Rt , Nill , Nill , Nill>::emit() const
	{
		for(typename c_list<base_type>::const_iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
			(*iter).operator()();
	}
	//--------------------------------------------------------
	template<typename _Rt>
	template<typename _O , typename _F>
	void c_signal<_Rt , Nill , Nill , Nill>::connect(_O obj_ , _F func_)
	{
		m_list.push_back(fastdelegate::MakeDelegate(obj_ , func_));
	}
	//--------------------------------------------------------
	template<typename _Rt>
	template<typename _O , typename _F>
	void c_signal<_Rt , Nill , Nill , Nill>::disconnect(_O obj_ , _F func_)
	{
		base_type temp = fastdelegate::MakeDelegate(obj_ , func_);
		for(typename c_list<base_type>::iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
		{
			if((*iter) == temp)
			{
				m_list.erase(iter);
				break;
			}
		}
	}
	//--------------------------------------------------------
	template<typename _Rt>
	bool c_signal<_Rt , Nill , Nill , Nill>::vaild() const
	{
		return !m_list.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename _Rt, typename _P1>
	class c_signal<_Rt , _P1 , Nill , Nill>
	{
	private:
		typedef fastdelegate::FastDelegate1<_P1 , _Rt> base_type;
		c_list<base_type> m_list;
	public:
		c_signal();
		void emit(_P1 p1_) const;
		template<typename _O , typename _F>
		void connect(_O obj_ , _F func_);
		template<typename _O , typename _F>
		void disconnect(_O obj_ , _F func_);
		bool vaild() const;
	};
	//--------------------------------------------------------
	template<typename _Rt, typename _P1>
	c_signal<_Rt , _P1 , Nill , Nill>::c_signal(){}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1>
	void c_signal<_Rt , _P1 , Nill , Nill>::emit(_P1 p1_) const
	{
		for(typename c_list<base_type>::const_iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
			(*iter).operator()(p1_);
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , Nill , Nill>::connect(_O obj_ , _F func_)
	{
		m_list.push_back(fastdelegate::MakeDelegate(obj_ , func_));
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , Nill , Nill>::disconnect(_O obj_ , _F func_)
	{
		base_type temp = fastdelegate::MakeDelegate(obj_ , func_);
		for(typename c_list<base_type>::iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
		{
			if((*iter) == temp)
			{
				m_list.erase(iter);
				break;
			}
		}
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1>
	bool c_signal<_Rt , _P1 , Nill , Nill>::vaild() const
	{
		return !m_list.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename _Rt, typename _P1 , typename _P2>
	class c_signal<_Rt , _P1 , _P2 , Nill>
	{
	private:
		typedef fastdelegate::FastDelegate2<_P1 , _P2 , _Rt> base_type;
		c_list<base_type> m_list;
	public:
		c_signal();
		void emit(_P1 p1_ , _P2 p2_) const;
		template<typename _O , typename _F>
		void connect(_O obj_ , _F func_);
		template<typename _O , typename _F>
		void disconnect(_O obj_ , _F func_);
		bool vaild() const;
	};	
	//--------------------------------------------------------
	template<typename _Rt, typename _P1 , typename _P2>
	c_signal<_Rt , _P1 , _P2 , Nill>::c_signal(){}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1 , typename _P2>
	void c_signal<_Rt , _P1 , _P2 , Nill>::emit(_P1 p1_ , _P2 p2_) const
	{
		for(typename c_list<base_type>::const_iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
			(*iter).operator()(p1_ , p2_);
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1 , typename _P2>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , _P2 , Nill>::connect(_O obj_ , _F func_)
	{
		m_list.push_back(fastdelegate::MakeDelegate(obj_ , func_));
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1 , typename _P2>
	template<typename _O , typename _F>
	void c_signal<_Rt , _P1 , _P2 , Nill>::disconnect(_O obj_ , _F func_)
	{
		base_type temp = fastdelegate::MakeDelegate(obj_ , func_);
		for(typename c_list<base_type>::iterator iter = m_list.begin() ; iter != m_list.end(); ++iter)
		{
			if((*iter) == temp)
			{
				m_list.erase(iter);
				break;
			}
		}
	}
	//--------------------------------------------------------
	template<typename _Rt, typename _P1 , typename _P2>
	bool c_signal<_Rt , _P1 , _P2 , Nill>::vaild() const
	{
		return !m_list.empty();
	}	
}
#ifdef emit
#	pragma pop_macro("emit")
#endif
#endif