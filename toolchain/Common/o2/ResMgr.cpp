/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Texture.h"
#include "Shader.h"
#include "Skin.h"
#include "Font.h"
#include "Interface.h"
#include "ResMgr.h"
namespace o2d{
	c_resource_manager::c_resource_manager():m_loading_thread(nullptr){}
	//--------------------------------------------------------
	c_resource_manager::~c_resource_manager(){}
	//--------------------------------------------------------
	bool c_resource_manager::initialize(const utf16* sysrespath_)
	{
		if(sysrespath_)
			_setup_buildin_resource(sysrespath_);
		m_loading_thread = new c_thread(_load_thread_func ,  _load_wakeup_func , this);
		LOG(LL_INFORMATION , "resource manager initialize sucessfully");
		return true;
	}
	//--------------------------------------------------------
	void c_resource_manager::shutdown()
	{		
		delete m_loading_thread;
		m_loading_thread = nullptr;
		FOREACH(TYPEOF(c_safe_list<c_resource*>)::iterator , iter , m_loading_queue)
		{
			(*iter)->unload();
			delete (*iter);
			(*iter) = nullptr;
		}
		clear_res_pool(RT_SHADER);
		LOG(LL_INFORMATION , "resource manager shutdown");
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::fetch_buildin_res(e_buildin_res_id id_, c_manual_loader* loader_ /*= nullptr*/)
	{
		AUTO(TYPEOF(c_map<e_buildin_res_id , c_resource*>)::iterator , iter, m_buildins.find(id_));
		if(iter!=m_buildins.end())
			return iter->second();
		else
		{
			if(!loader_)
				return nullptr;
			else
			{
				c_resource* res = c_resource_manager::get_singleton_ptr()->fetch_resource(loader_);
				m_buildins.insert(id_ , res);
				return res;
			}
		}
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::fetch_resource(const utf16* filename_, bool async_ /*= true*/)
	{
		c_resource* res(nullptr);
		if(!utf16_cmp(get_ext(filename_) , L"mp3"))
			res = _fetch_sound(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"wav"))
			res = _fetch_sound(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"glfx"))
			res = _fetch_effect(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"ktx"))
			res = _fetch_texture(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"ttf"))
			res = _fetch_font(filename_, async_);
		else if (!utf16_cmp(get_ext(filename_), L"ttc"))
			res = _fetch_font(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"bmg"))
			res = _fetch_skin(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"bui"))
			res = _fetch_interface(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"act"))
			res = _fetch_actor(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"par"))
			res = _fetch_particle(filename_, async_);
		else if(!utf16_cmp(get_ext(filename_) , L"ter"))
			res = _fetch_map(filename_, async_);
		else
		{
			LOG(LL_ERROR , "%s resource type is incorrect" , (const ansi*)c_string(filename_).to_utf8());
			return nullptr;
		}
		res->increment();
		return res;
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::fetch_resource(c_manual_loader* loader_)
	{
		if(!loader_->load())
			return nullptr;
		else
			return loader_->get_res();
	}
	//--------------------------------------------------------
	void c_resource_manager::discard_resource(const utf16* filename_)
	{
		if(!utf16_cmp(get_ext(filename_) , L"mp3"))
			_discard_sound(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"wav"))
			_discard_sound(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"glfx"))
			_discard_shader(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"ktx"))
			_discard_texture(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"ttf"))
			_discard_font(filename_);
		else if (!utf16_cmp(get_ext(filename_), L"ttc"))
			_discard_font(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"bmg"))
			_discard_skin(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"bui"))
			_discard_interface(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"act"))
			_discard_actor(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"par"))
			_discard_particle(filename_);
		else if(!utf16_cmp(get_ext(filename_) , L"ter"))
			_discard_map(filename_);
		else
			LOG(LL_ERROR , "%s resource type is incorrect" , (const ansi*)c_string(filename_).to_utf8());
	}
	//--------------------------------------------------------
	void c_resource_manager::discard_resource(c_manual_loader* loader_)
	{
		loader_->unload();
	}
	//--------------------------------------------------------
	void c_resource_manager::clear_res_pool(e_resource_type type_)
	{
		switch(type_)
		{
		case RT_SOUND:
			{
				FOREACH(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_sound_res_pool)
				{
					iter->second()->unload();
					delete iter->second();
					iter->second() = nullptr;
				}
				m_sound_res_pool.clear();
			}
			break;
		case RT_SHADER:
			{
				FOREACH(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_shader_res_pool)
				{
					iter->second()->unload();
					delete iter->second();
					iter->second() = nullptr;
				}
				m_shader_res_pool.clear();
			}
			break;
		case RT_TEXTURE:
			{
				FOREACH(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_texture_res_pool)
				{
					iter->second()->unload();
					delete iter->second();
					iter->second() = nullptr;
				}
				m_texture_res_pool.clear();
			}
			break;
		case RT_SPRITE:
			{
				FOREACH(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_sprite_res_pool)
				{
					iter->second()->unload();
					delete iter->second();
					iter->second() = nullptr;
				}
				m_sprite_res_pool.clear();
			}
			break;
		case RT_UI:
			{
				FOREACH(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_ui_res_pool)
				{
					iter->second()->unload();
					delete iter->second();
					iter->second() = nullptr;
				}
				m_ui_res_pool.clear();
			}
			break;
		default:
			break;
		}
	}
	//--------------------------------------------------------
	u32 c_resource_manager::get_res_pool_size(e_resource_type type_)
	{
		switch(type_)
		{
		case RT_SOUND:
			return m_sound_res_pool.size();
		case RT_SHADER:
			return m_shader_res_pool.size();
		case RT_TEXTURE:
			return m_texture_res_pool.size();
		case RT_SPRITE:
			return m_sprite_res_pool.size();
		case RT_UI:
			return m_ui_res_pool.size();
		default:
			return 0;
		}
	}
	//--------------------------------------------------------
	void c_resource_manager::step(u32 dt_)
	{
		m_setup_queue.lock();
		while(!m_setup_queue.empty())
		{
			c_resource* res = m_setup_queue.pop_front();	
			res->setup();
			switch(res->get_res_type())
			{
			case RT_SOUND:
				m_sound_res_pool.lock();
				m_sound_res_pool.insert(res->get_id() , res);
				m_sound_res_pool.unlock();
				break;
			case RT_TEXTURE:
				m_texture_res_pool.lock();
				m_texture_res_pool.insert(res->get_id() , res);
				m_texture_res_pool.unlock();
				break;
			case RT_SHADER:
				m_shader_res_pool.lock();
				m_shader_res_pool.insert(res->get_id() , res);
				m_shader_res_pool.unlock();
				break;
			case RT_SPRITE:
				m_sprite_res_pool.lock();
				m_sprite_res_pool.insert(res->get_id() , res);
				m_sprite_res_pool.unlock();
				break;
			case RT_UI:
				m_ui_res_pool.lock();
				m_ui_res_pool.insert(res->get_id() , res);
				m_ui_res_pool.unlock();
				break;
            default:assert(0);break;
			}
			AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_processing_res.find(res->get_id()));
			assert(iter!=m_processing_res.end());
			m_processing_res.erase(iter);
		}
		m_setup_queue.unlock();
	}
	//--------------------------------------------------------
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
	u32 c_resource_manager::_load_thread_func(void* thread_)
#else 
	void* c_resource_manager::_load_thread_func(void* thread_)
#endif
	{
		c_resource_manager* self = (c_resource_manager*)((c_thread*)thread_)->lock_user_data();
		((c_thread*)thread_)->unlock_user_data();
		do
		{
            self->m_loading_queue.lock();
			self->m_loading_queue.wait();
            self->m_loading_queue.unlock();
			while(!self->m_loading_queue.empty())
			{
                self->m_loading_queue.lock();
				c_resource* res = self->m_loading_queue.pop_front();
				if(!res->load())
				{
					res->unload();
					delete res;
					res = 0;								
					continue;
				}
				self->m_loading_queue.unlock();
				self->m_setup_queue.lock();
				self->m_setup_queue.push_back(res);
				self->m_setup_queue.unlock();
			}
		} while (((c_thread*)thread_)->is_running());
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		return 0xdead;
#else 
		return (void*)0xdead;
#endif
	}
	//--------------------------------------------------------
	void c_resource_manager::_load_wakeup_func(void* thread_)
	{
		c_resource_manager* self = (c_resource_manager*)((c_thread*)thread_)->lock_user_data();
		self->m_loading_queue.notify();
		((c_thread*)thread_)->unlock_user_data();
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_add_2_res_queue(c_resource* res_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter , m_processing_res.find(res_->get_id()));
		if(iter != m_processing_res.end())
			return iter->second();
		m_loading_queue.lock();
		m_loading_queue.push_back(res_);
		m_loading_queue.notify();
		m_loading_queue.unlock();
		m_processing_res.insert(res_->get_id() , res_);
		return nullptr;
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_sound(const utf16* name_ , bool async_)
	{
		c_resource* res(nullptr);
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_sound(const utf16* name_)
	{
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_effect(const utf16* name_ ,bool async_)
	{
		c_resource* res(nullptr);
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_shader_res_pool.find(hash(name_)));
		if(iter!=m_shader_res_pool.end())
		{
			assert(iter->second()->get_res_state() == RS_SETUPED);
			res = iter->second();
		}
		else
		{				
			res = new c_shader(name_);
			if(async_)
			{
				c_resource* another = _add_2_res_queue(res);
				if(another)
				{
					delete res;
					res = another;
				}
			}
			else
			{
				if(res->load())
				{
					res->setup();
					m_shader_res_pool.insert(hash(res->get_filename()) , res);
				}
			}				
		}
		if(!res)
			LOG(LL_ERROR , "the resource file %s is not exist" , (const ansi*)c_string(name_).to_utf8());
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_shader(const utf16* name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_shader_res_pool.find(hash(name_)));
		if(iter!=m_shader_res_pool.end())
		{
			iter->second()->decrement();
			if(iter->second()->ref_count() == 1)
			{
				iter->second()->unload();
				delete iter->second();
				iter->second() = nullptr;
				m_shader_res_pool.erase(iter);
			}
		}
		else
			LOG(LL_WARNING , "%s doesn't exist unload failed" , (const ansi*)c_string(name_).to_utf8());
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_texture(const utf16* name_, bool async_)
	{
		c_resource* res(nullptr);
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_texture_res_pool.find(hash(name_)));
		if(iter!=m_texture_res_pool.end())
		{
			assert(iter->second()->get_res_state() == RS_SETUPED);
			res = iter->second();
		}
		else
		{		
			res = new c_texture(name_);
			if(async_)
			{
				c_resource* another = _add_2_res_queue(res);
				if(another)
				{
					delete res;
					res = another;
				}
			}
			else
			{
				if(res->load())
				{
					res->setup();
					m_texture_res_pool.insert(hash(res->get_filename()) , res);
				}
			}		
		}
		if(!res)
			LOG(LL_ERROR , "the resource file %s is not exist" , (const ansi*)c_string(name_).to_utf8());
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_texture(const utf16* name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_texture_res_pool.find(hash(name_)));
		if(iter!=m_texture_res_pool.end())
		{
			iter->second()->decrement();
			if(iter->second()->ref_count() == 1)
			{
				iter->second()->unload();
				delete iter->second();
				iter->second() = nullptr;
				m_texture_res_pool.erase(iter);
			}
		}
		else
			LOG(LL_WARNING , "%s doesn't exist unload failed" , (const ansi*)c_string(name_).to_utf8());
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_font(const utf16* name_, bool async_)
	{
		c_resource* res(nullptr);
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			assert(iter->second()->get_res_state() == RS_SETUPED);
			res = iter->second();
		}
		else
		{		
			res = new c_font(name_);
			if(async_)
			{
				c_resource* another = _add_2_res_queue(res);
				if(another)
				{
					delete res;
					res = another;
				}
			}
			else
			{
				if(res->load())
				{
					res->setup();
					m_ui_res_pool.insert(hash(res->get_filename()) , res);
				}
			}		
		}
		if(!res)
			LOG(LL_ERROR , "the resource file %s is not exist" , (const ansi*)c_string(name_).to_utf8());
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_font(const utf16* name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			iter->second()->decrement();
			if(iter->second()->ref_count() == 1)
			{
				iter->second()->unload();
				delete iter->second();
				iter->second() = nullptr;
				m_ui_res_pool.erase(iter);
			}
		}
		else
			LOG(LL_WARNING , "%s doesn't exist unload failed" , (const ansi*)c_string(name_).to_utf8());
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_skin(const utf16* name_, bool async_)
	{
		c_resource* res(nullptr);
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			assert(iter->second()->get_res_state() == RS_SETUPED);
			res = iter->second();
		}
		else
		{		
			res = new c_skin(name_);
			if(async_)
			{
				c_resource* another = _add_2_res_queue(res);
				if(another)
				{
					delete res;
					res = another;
				}
			}
			else
			{
				if(res->load())
				{
					res->setup();
					m_ui_res_pool.insert(hash(res->get_filename()) , res);
				}
			}		
		}
		if(!res)
			LOG(LL_ERROR , "the resource file %s is not exist" , (const ansi*)c_string(name_).to_utf8());
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_skin(const utf16* name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			iter->second()->decrement();
			if(iter->second()->ref_count() == 1)
			{
				iter->second()->unload();
				delete iter->second();
				iter->second() = nullptr;
				m_ui_res_pool.erase(iter);
			}
		}
		else
			LOG(LL_WARNING , "%s doesn't exist unload failed" , (const ansi*)c_string(name_).to_utf8());
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_interface(const utf16* name_ , bool async_)
	{
		c_resource* res(nullptr);
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			assert(iter->second()->get_res_state() == RS_SETUPED);
			res = iter->second();
		}
		else
		{		
			res = new c_interface(name_);
			if(async_)
			{
				c_resource* another = _add_2_res_queue(res);
				if(another)
				{
					delete res;
					res = another;
				}
			}
			else
			{
				if(res->load())
				{
					res->setup();
					m_ui_res_pool.insert(hash(res->get_filename()) , res);
				}
			}		
		}
		if(!res)
			LOG(LL_ERROR , "the resource file %s is not exist" , (const ansi*)c_string(name_).to_utf8());
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_interface(const utf16* name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_resource*>)::iterator , iter ,m_ui_res_pool.find(hash(name_)));
		if(iter!=m_ui_res_pool.end())
		{
			iter->second()->decrement();
			if(iter->second()->ref_count() == 1)
			{
				iter->second()->unload();
				delete iter->second();
				iter->second() = nullptr;
				m_ui_res_pool.erase(iter);
			}
		}
		else
			LOG(LL_WARNING , "%s doesn't exist unload failed" , (const ansi*)c_string(name_).to_utf8());
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_actor(const utf16* name_ , bool async_)
	{
		c_resource* res(nullptr);
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_actor(const utf16* name_)
	{
	}	
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_particle(const utf16* name_ , bool async_)
	{
		c_resource* res(nullptr);
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_particle(const utf16* name_)
	{
	}
	//--------------------------------------------------------
	c_resource* c_resource_manager::_fetch_map(const utf16* name_ , bool async_)
	{
		c_resource* res(nullptr);
		return res;
	}
	//--------------------------------------------------------
	void c_resource_manager::_discard_map(const utf16* name_)
	{
	}
	//--------------------------------------------------------
	void c_resource_manager::_setup_buildin_resource(const c_string& path_)
	{
		c_resource* res;
		c_string tmp;
		tmp = path_;
		tmp.append(L"/GLFX/UI.glfx");
		res = (c_shader*)fetch_resource(tmp.c_str() , false);
#if !defined(O2D_USE_DX_API)
		((c_shader*)res)->active(L"ui_gl");
#else
		((c_shader*)res)->active(L"ui_dx");
#endif
		m_buildins.insert(BRI_UI_GLFX , res);	
	}
}