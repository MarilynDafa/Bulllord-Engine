/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "FileMgr.h"
#include "NameGen.h"
#include "Device.h"
#include "Shader.h"
namespace o2d{
	c_shader::c_shader(const utf16* filename_):c_resource(filename_, RT_SHADER){}
	//--------------------------------------------------------
	c_shader::~c_shader(){}
	//--------------------------------------------------------
	c_technique* c_shader::get_tech(const c_string& name_)
	{
		AUTO(TYPEOF(c_map<u32 , c_technique*>)::iterator , iter ,m_teches.find(hash(name_.c_str())));
		if(iter != m_teches.end())
			return iter->second();
		else
		{
			LOG(LL_ERROR , "can not find the tech %s" , (const ansi*)name_.to_utf8());
			return nullptr;
		}
	}
	//--------------------------------------------------------
	void c_shader::active(const c_string& name_)
	{
		c_technique* tech = get_tech(name_);
		if(!tech)
			return;
		FOREACH(TYPEOF(c_vector<c_pass*>)::iterator , iter , tech->m_passes)
		{
			c_string vs , fs , gs;
			AUTO(TYPEOF(c_map<u32 , c_string>)::iterator , iterv ,m_vs_prog.find((*iter)->m_vs_id));
			if(iterv!=m_vs_prog.end())
				vs = iterv->second();
			AUTO(TYPEOF(c_map<u32 , c_string>)::iterator , iterf ,m_fs_prog.find((*iter)->m_fs_id));
			if(iterf!=m_fs_prog.end())
				fs = iterf->second();
			(*iter)->compile((const ansi*)vs.to_utf8() , (const ansi*)fs.to_utf8());
		}
	}
	//--------------------------------------------------------
	bool c_shader::_load_impl()
	{
		c_xml_reader xml;
		if(!c_file_manager::get_singleton_ptr()->obtain_xml_reader(m_filename , xml , true))
			return false;
		c_map<u32 , c_vector<s_unifrom_desc> > uniforms;
		c_map<u32 , c_vector<s_sampler_desc> > samplers;
		c_map<u32 , c_vector<s_attrib_desc> > attributes;
		do
		{
			if(xml.is_the_node((const utf8*)"vert", 5))
			{
				c_string vname = xml.get_attribute_as_string((const utf8*)"name" , 5);
				c_string context;
				c_vector<s_unifrom_desc> us;
				c_vector<s_sampler_desc> ss;
				c_vector<s_attrib_desc> as;
				if(xml.set_to_first_child((const utf8*)"variant" , 8))
				{
					u32 offset = 0;
					do
					{
						c_string qualifier = xml.get_attribute_as_string((const utf8*)"qualifier" , 10);
#if !defined(O2D_USE_DX_API)
						c_string count = L"1";
						if(qualifier == L"attribute")
						{
							context.append(qualifier);
							context.append(L" ");
						}
						else if(qualifier == L"varying")
						{
							context.append(qualifier);
#	if defined(O2D_USE_GLES_API)
							context.append(L" mediump ");
#	else
							context.append(L" ");
#	endif
						}
						else if(qualifier == L"uniform")
						{
							context.append(qualifier);
							context.append(L" ");
							count = xml.get_attribute_as_string((const utf8*)"count" , 6);
						}
						else
						{
							context.append(L"uniform");
							context.append(L" ");
							context.append(qualifier);
						}
#endif
						c_string type = xml.get_attribute_as_string((const utf8*)"type" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(type);
						context.append(L" ");
#endif
						c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(name);
						if(count!=L"1")
						{
							context.append(L'[');
							context.append(count);
							context.append(L']');
						}
						context.append(L";\n");
#endif
						if(qualifier == L"sampler")
						{
							s_sampler_desc sd;
							sd.name = name;
							sd.type = L"sampler";
							sd.type += type;
							sd.unit = xml.get_attribute_as_integer((const utf8*)"unit" , 5);
							ss.push_back(sd);
						}
						else if(qualifier == L"uniform")
						{
							s_unifrom_desc ud;
							ud.name = name;
							ud.type = type;
#if !defined(O2D_USE_DX_API)
							ud.handle = -1;
							ud.count = count.to_uinteger();
#else
							ud.count = xml.get_attribute_as_string((const utf8*)"count" , 6).to_integer();
							ud.handle = offset;							
							if(ud.type == L"float")
								offset += 4;
							else if(ud.type == L"vec2")
								offset += 8;
							else if(ud.type == L"vec3")
								offset += 12;
							else if(ud.type == L"vec4")
								offset += 16;
							else if(ud.type == L"int")
								offset += 4;
							else if(ud.type == L"ivec2")
								offset += 8;
							else if(ud.type == L"ivec3")
								offset += 12;
							else if(ud.type == L"ivec4")
								offset += 16;
							else if(ud.type == L"mat2")
								offset += 16;
							else if(ud.type == L"mat3")
								offset += 36;
							else if(ud.type == L"mat4")
								offset += 64;
							else assert(0);
#endif
							us.push_back(ud);
						}
						else if(qualifier == L"attribute")
						{
							s_attrib_desc ad;
							ad.name = name;
							ad.type = type;
							c_string semantic = xml.get_attribute_as_string((const utf8*)"semantic" , 9);
							if(semantic == L"POSITION")
								ad.semantic = VES_POSITION;
							else if(semantic == L"DIFFUSE")
								ad.semantic = VES_DIFFUSE;
							else if(semantic == L"TEXTURE_COORD")
								ad.semantic = VES_TEXTURE_COORD;
							else;
							ad.index = xml.get_attribute_as_integer(L"index");
							as.push_back(ad);
						}
						else;
					}while(xml.set_to_next_sibling((const utf8*)"variant" , 8));
				}
				if(xml.set_to_next_sibling((const utf8*)"shader" , 7))
				{
					xml.set_to_first_child(nullptr , 0);
					c_string shastr;
					xml.get_cdata(shastr);
					context.append(shastr);
					m_vs_prog.insert(hash(vname.c_str()) , context);
					if(!us.empty())
						uniforms.insert(hash(vname.c_str()) , us);
					if(!ss.empty())
						samplers.insert(hash(vname.c_str()) , ss);
					if(!as.empty())
						attributes.insert(hash(vname.c_str()) , as);
				}
			}
			else if(xml.is_the_node((const utf8*)"frag" , 5))
			{
				c_string fname = xml.get_attribute_as_string((const utf8*)"name" , 5);
				c_string context;
				c_vector<s_unifrom_desc> us;
				c_vector<s_sampler_desc> ss;
				if(xml.set_to_first_child((const utf8*)"variant" , 8))
				{
					u32 offset = 0;
					do
					{
						c_string qualifier = xml.get_attribute_as_string((const utf8*)"qualifier" , 10);
#if !defined(O2D_USE_DX_API)
						c_string count = L"1";
						if(qualifier == L"attribute")
						{
							context.append(qualifier);
							context.append(L" ");
						}
						else if(qualifier == L"varying")
						{
							context.append(qualifier);
#	if defined(O2D_USE_GLES_API)
							context.append(L" mediump ");
#	else
							context.append(L" ");
#	endif
						}
						else if(qualifier == L"uniform")
						{
							context.append(qualifier);
							context.append(L" ");
							count = xml.get_attribute_as_string((const utf8*)"count" , 6);
						}
						else
						{
							context.append(L"uniform");
							context.append(L" ");
							context.append(qualifier);
						}
#endif
						c_string type = xml.get_attribute_as_string((const utf8*)"type" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(type);
						context.append(L" ");
#endif
						c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(name);
						if(count != L"1")
						{
							context.append(L'[');
							context.append(count);
							context.append(L']');
						}
						context.append(L";\n");
#endif
						if(qualifier == L"sampler")
						{
							s_sampler_desc ud;
							ud.name = name;
							ud.type = L"sampler";
							ud.type += type;
							ud.unit = xml.get_attribute_as_integer((const utf8*)"unit" , 5);
							ss.push_back(ud);
						}
						else if(qualifier == L"uniform")
						{
							s_unifrom_desc ud;
							ud.name = name;
							ud.type = type;
#if !defined(O2D_USE_DX_API)
							ud.handle = -1;
							ud.count = count.to_uinteger();
#else
							ud.count = xml.get_attribute_as_string((const utf8*)"count" , 6).to_integer();
							ud.handle = offset;							
							if(ud.type == L"float")
								offset += 4;
							else if(ud.type == L"vec2")
								offset += 8;
							else if(ud.type == L"vec3")
								offset += 12;
							else if(ud.type == L"vec4")
								offset += 16;
							else if(ud.type == L"int")
								offset += 4;
							else if(ud.type == L"ivec2")
								offset += 8;
							else if(ud.type == L"ivec3")
								offset += 12;
							else if(ud.type == L"ivec4")
								offset += 16;
							else if(ud.type == L"mat2")
								offset += 16;
							else if(ud.type == L"mat3")
								offset += 36;
							else if(ud.type == L"mat4")
								offset += 64;
							else assert(0);
#endif
							us.push_back(ud);
						}
						else;
					}while(xml.set_to_next_sibling((const utf8*)"variant" , 8));
				}
				if(xml.set_to_next_sibling((const utf8*)"shader" , 7))
				{
					xml.set_to_first_child(nullptr ,  0);
					c_string shastr;
					xml.get_cdata(shastr);
					context.append(shastr);
					m_fs_prog.insert(hash(fname.c_str()) , context);
					if(!ss.empty())
						samplers.insert(hash(fname.c_str()) , ss);
					if(!us.empty())
						uniforms.insert(hash(fname.c_str()) , us);
				}
			}
			else if(xml.is_the_node((const utf8*)"tech", 5))
			{
				c_technique* tech = new c_technique();
				u32 id = hash(xml.get_attribute_as_string((const utf8*)"name" , 5).c_str());
				if(xml.set_to_first_child((const utf8*)"pass" , 5))
				{
					do
					{
						c_pass* pass = tech->add_pass(xml.get_attribute_as_string((const utf8*)"name" , 5));
						c_string vname = xml.get_attribute_as_string((const utf8*)"vert" , 5);
						c_string fname = xml.get_attribute_as_string((const utf8*)"frag" , 5);
						pass->m_vs_id = hash(vname.c_str());
						pass->m_fs_id = hash(fname.c_str());
						AUTO(TYPEOF(c_map<u32 , c_vector<s_sampler_desc> >)::iterator , iter1 ,samplers.find(hash(vname.c_str())));
						if(iter1!=samplers.end())
						{
							if(iter1->second().size())
							{
								FOREACH(TYPEOF(c_vector<s_sampler_desc>)::iterator , iter , iter1->second())
									pass->add_sampler(iter->name , iter->unit);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<s_sampler_desc> >)::iterator , iter2 ,samplers.find(hash(fname.c_str())));
						if(iter2!=samplers.end())
						{
							if(iter2->second().size())
							{
								FOREACH(TYPEOF(c_vector<s_sampler_desc>)::iterator , iter , iter2->second())
									pass->add_sampler(iter->name , iter->unit);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<s_unifrom_desc> >)::iterator , iter3 ,uniforms.find(hash(vname.c_str())));
						if(iter3!=uniforms.end())
						{
							if(iter3->second().size())
							{
								FOREACH(TYPEOF(c_vector<s_unifrom_desc>)::iterator , iter , iter3->second())
									pass->add_uniform(iter->name, iter->type , iter->count ,true, iter->handle);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<s_unifrom_desc> >)::iterator , iter4 ,uniforms.find(hash(fname.c_str())));
						if(iter4!=uniforms.end())
						{
							if(iter4->second().size())
							{
								FOREACH(TYPEOF(c_vector<s_unifrom_desc>)::iterator , iter , iter4->second())
									pass->add_uniform(iter->name, iter->type , iter->count , false, iter->handle);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<s_attrib_desc> >)::iterator , iter5 ,attributes.find(hash(vname.c_str())));
						if(iter5!=attributes.end())
						{
							if(iter5->second().size())
							{
								u32 handle = 0;
								FOREACH(TYPEOF(c_vector<s_attrib_desc>)::iterator , iter , iter5->second())
								{
									if(iter->type == L"float")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT1 , iter->name, handle);
									else if(iter->type == L"vec2")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT2 , iter->name, handle);
									else if(iter->type == L"vec3")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT3 , iter->name, handle);
									else if(iter->type == L"vec4")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT4 , iter->name, handle);
									else if(iter->type == L"int")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER1 , iter->name, handle);
									else if(iter->type == L"ivec2")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER2 , iter->name, handle);
									else if(iter->type == L"ivec3")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER3 , iter->name, handle);
									else if(iter->type == L"ivec4")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER4 , iter->name, handle);
									else assert(0);
									handle++;
								}
							}
						}
						if(xml.set_to_first_child((const utf8*)"renderstate" , 12))
						{
							do
							{
								c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
								c_string val  = xml.get_attribute_as_string((const utf8*)"value" , 6);
								if(name == L"depth_enable")
									pass->m_states[RS_DEPTH_ENABLE] = _str_2_glenum(val);
								else if(name == L"depth_func")
									pass->m_states[RS_DEPTH_FUNC] = _str_2_glenum(val);
								else if(name == L"blend_enable")
									pass->m_states[RS_BLEND_ENABLE] = _str_2_glenum(val);
								else if(name == L"blend_op")
									pass->m_states[RS_BLEND_OP] = _str_2_glenum(val);
								else if(name == L"blend_op_alpha")
									pass->m_states[RS_BLEND_OP_ALPHA] = _str_2_glenum(val);
								else if(name == L"src_factor")
									pass->m_states[RS_SRC_FACTOR] = _str_2_glenum(val);
								else if(name == L"dst_factor")
									pass->m_states[RS_DST_FACTOR] = _str_2_glenum(val);
								else if(name == L"src_alpha_factor")
									pass->m_states[RS_SRC_ALPHA_FACTOR] = _str_2_glenum(val);
								else if(name == L"dst_alpha_factor")
									pass->m_states[RS_DST_ALPHA_FACTOR] = _str_2_glenum(val);
								else
									assert(0);
							} while (xml.set_to_next_sibling((const utf8*)"renderstate" , 12));
						}
						xml.set_to_parent();
					}while(xml.set_to_next_sibling((const utf8*)"pass" , 5));
				}
				m_teches.insert(id, tech);
			}
			else;
		} while (xml.retrieve());
#if defined(O2D_USE_GL_API)
		c_string prefix = L"#version 110\n";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , m_vs_prog)
        iter->second() = prefix + iter->second();
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , m_fs_prog)
        iter->second() = prefix + iter->second();
#elif defined(O2D_USE_GLES_API)
		c_string prefix = L"#version 100\n";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , m_vs_prog)
        iter->second() = prefix + iter->second();
		prefix << L"precision lowp float;\n";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , m_fs_prog)
        iter->second() = prefix + iter->second();
#endif
		return true;
	}
	//--------------------------------------------------------
	void c_shader::_unload_impl()
	{
		FOREACH(TYPEOF(c_map<u32 , c_technique*>)::iterator , iter , m_teches)
        delete iter->second();
	}
	//--------------------------------------------------------
	s32 c_shader::_str_2_glenum(const c_string& str_)
	{
		if(str_ == L"GL_TRUE")
			return 1;
		else if(str_ == L"GL_FALSE")
			return 0;
		else if(str_ == L"GL_NEVER")
			return CF_ALWAYS_FAIL;
		else if(str_ == L"GL_LESS")
			return CF_LESS;
		else if(str_ == L"GL_EQUAL")
			return CF_EQUAL;
		else if(str_ == L"GL_LEQUAL")
			return CF_LESS_EQUAL;
		else if(str_ == L"GL_GREATER")
			return CF_GREATER;
		else if(str_ == L"GL_NOTEQUAL")
			return CF_NOT_EQUAL;
		else if(str_ == L"GL_GEQUAL")
			return CF_GREATER_EQUAL;
		else if(str_ == L"GL_ALWAYS")
			return CF_ALWAYS_PASS;
		else if(str_ == L"GL_FUNC_ADD")
			return BO_ADD;
		else if(str_ == L"GL_FUNC_SUBTRACT")
			return BO_SUBTRACT;
		else if(str_ == L"GL_FUNC_REVERSE_SUBTRACT")
			return BO_REVERSE_SUBTRACT;
		else if(str_ == L"GL_ZERO")
			return BF_ZERO;
		else if(str_ == L"GL_ONE")
			return BF_ONE;
		else if(str_ == L"GL_SRC_COLOR")
			return BF_SRC_COLOR;
		else if(str_ == L"GL_ONE_MINUS_SRC_COLOR")
			return BF_ONE_MINUS_SRC_COLOR;
		else if(str_ == L"GL_DST_COLOR")
			return BF_DEST_COLOR;
		else if(str_ == L"GL_ONE_MINUS_DST_COLOR")
			return BF_ONE_MINUS_DEST_COLOR;
		else if(str_ == L"GL_SRC_ALPHA")
			return BF_SRC_ALPHA;
		else if(str_ == L"GL_ONE_MINUS_SRC_ALPHA")
			return BF_ONE_MINUS_SRC_ALPHA;
		else if(str_ == L"GL_DST_ALPHA")
			return BF_DEST_ALPHA;
		else if(str_ == L"GL_ONE_MINUS_DST_ALPHA")
			return BF_ONE_MINUS_DEST_ALPHA;
		else
		{
			assert(0);
			return 0;
		}
	}
	//--------------------------------------------------------
	c_shader_loader::c_shader_loader(const u8* mem_)
	{
		static c_name_generator gen(L"manual_shader");
		m_res = new c_shader(gen.generate().c_str());
		const ansi* data = (const ansi*)mem_;
		u32 sz = (u32)strlen(data) + 1;
		m_data.resize(sz);
		m_data.write_buf(data , sz);
		m_data.set_zero_end();
	}
	//--------------------------------------------------------
	c_shader_loader::~c_shader_loader()	{}
	//--------------------------------------------------------
	bool c_shader_loader::load()
	{
		c_xml_reader xml;
		xml.parse((u8*)m_data.get_pointer() , true);
		c_map<u32 , c_vector<c_shader::s_unifrom_desc> > uniforms;
		c_map<u32 , c_vector<c_shader::s_sampler_desc> > samplers;
		c_map<u32 , c_vector<c_shader::s_attrib_desc> > attributes;
		do
		{
			if(xml.is_the_node((const utf8*)"vert", 5))
			{
				c_string vname = xml.get_attribute_as_string((const utf8*)"name" , 5);
				c_string context;
				c_vector<c_shader::s_unifrom_desc> us;
				c_vector<c_shader::s_sampler_desc> ss;
				c_vector<c_shader::s_attrib_desc> as;
				if(xml.set_to_first_child((const utf8*)"variant" , 8))
				{
					do
					{
						c_string qualifier = xml.get_attribute_as_string((const utf8*)"qualifier" , 10);
#if !defined(O2D_USE_DX_API)
						c_string count = L"1";
						if(qualifier == L"attribute")
						{
							context.append(qualifier);
							context.append(L" ");
						}
						else if(qualifier == L"varying")
						{
							context.append(qualifier);
#	if defined(O2D_USE_GLES_API)
							context.append(L" mediump ");
#	else
							context.append(L" ");
#	endif
						}
						else if(qualifier == L"uniform")
						{
							context.append(qualifier);
							context.append(L" ");
							count = xml.get_attribute_as_string((const utf8*)"count" , 6);
						}
						else
						{
							context.append(L"uniform");
							context.append(L" ");
							context.append(qualifier);
						}
#endif
						c_string type = xml.get_attribute_as_string((const utf8*)"type" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(type);
						context.append(L" ");
#endif
						c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(name);
						if(count!=L"1")
						{
							context.append(L'[');
							context.append(count);
							context.append(L']');
						}
						context.append(L";\n");
#endif
						if(qualifier == L"sampler")
						{
							c_shader::s_sampler_desc sd;
							sd.name = name;
							sd.type = L"sampler";
							sd.type += type;
							sd.unit = xml.get_attribute_as_integer((const utf8*)"unit" , 5);
							ss.push_back(sd);
						}
						else if(qualifier == L"uniform")
						{
							c_shader::s_unifrom_desc ud;
							ud.name = name;
							ud.type = type;
#if !defined(O2D_USE_DX_API)
							ud.count = count.to_uinteger();
#else
							ud.count = xml.get_attribute_as_string((const utf8*)"count" , 6).to_integer();
#endif
							us.push_back(ud);
						}
						else if(qualifier == L"attribute")
						{
							c_shader::s_attrib_desc ad;
							ad.name = name;
							ad.type = type;
							c_string semantic = xml.get_attribute_as_string((const utf8*)"semantic" , 9);
							if(semantic == L"POSITION")
								ad.semantic = VES_POSITION;
							else if(semantic == L"DIFFUSE")
								ad.semantic = VES_DIFFUSE;
							else if(semantic == L"TEXTURE_COORD")
								ad.semantic = VES_TEXTURE_COORD;
							else;
							ad.index = xml.get_attribute_as_integer(L"index");
							as.push_back(ad);
						}
						else;
					}while(xml.set_to_next_sibling((const utf8*)"variant" , 8));
				}
				if(xml.set_to_next_sibling((const utf8*)"shader" , 7))
				{
					xml.set_to_first_child(nullptr , 0);
					c_string shastr;
					xml.get_cdata(shastr);
					context.append(shastr);
					((c_shader*)m_res)->m_vs_prog.insert(hash(vname.c_str()) , context);
					if(!us.empty())
						uniforms.insert(hash(vname.c_str()) , us);
					if(!ss.empty())
						samplers.insert(hash(vname.c_str()) , ss);
					if(!as.empty())
						attributes.insert(hash(vname.c_str()) , as);
				}
			}
			else if(xml.is_the_node((const utf8*)"frag" , 5))
			{
				c_string fname = xml.get_attribute_as_string((const utf8*)"name" , 5);
				c_string context;
				c_vector<c_shader::s_unifrom_desc> us;
				c_vector<c_shader::s_sampler_desc> ss;
				if(xml.set_to_first_child((const utf8*)"variant" , 8))
				{
					do
					{
						c_string qualifier = xml.get_attribute_as_string((const utf8*)"qualifier" , 10);
#if !defined(O2D_USE_DX_API)
						c_string count = L"1";
						if(qualifier == L"attribute")
						{
							context.append(qualifier);
							context.append(L" ");
						}
						else if(qualifier == L"varying")
						{
							context.append(qualifier);
#	if defined(O2D_USE_GLES_API)
							context.append(L" mediump ");
#	else
							context.append(L" ");
#	endif
						}
						else if(qualifier == L"uniform")
						{
							context.append(qualifier);
							context.append(L" ");
							count = xml.get_attribute_as_string((const utf8*)"count" , 6);
						}
						else
						{
							context.append(L"uniform");
							context.append(L" ");
							context.append(qualifier);
						}
#endif
						c_string type = xml.get_attribute_as_string((const utf8*)"type" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(type);
						context.append(L" ");
#endif
						c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
#if !defined(O2D_USE_DX_API)
						context.append(name);
						if(count != L"1")
						{
							context.append(L'[');
							context.append(count);
							context.append(L']');
						}
						context.append(L";\n");
#endif
						if(qualifier == L"sampler")
						{
							c_shader::s_sampler_desc ud;
							ud.name = name;
							ud.type = L"sampler";
							ud.type += type;
							ud.unit = xml.get_attribute_as_integer((const utf8*)"unit" , 5);
							ss.push_back(ud);
						}
						else if(qualifier == L"uniform")
						{
							c_shader::s_unifrom_desc ud;
							ud.name = name;
							ud.type = type;
#if !defined(O2D_USE_DX_API)
							ud.count = count.to_uinteger();
#else
							ud.count = xml.get_attribute_as_string((const utf8*)"count" , 6).to_integer();
#endif
							us.push_back(ud);
						}
						else;
					}while(xml.set_to_next_sibling((const utf8*)"variant" , 8));
				}
				if(xml.set_to_next_sibling((const utf8*)"shader" , 7))
				{
					xml.set_to_first_child(nullptr ,  0);
					c_string shastr;
					xml.get_cdata(shastr);
					context.append(shastr);
					((c_shader*)m_res)->m_fs_prog.insert(hash(fname.c_str()) , context);
					if(!ss.empty())
						samplers.insert(hash(fname.c_str()) , ss);
					if(!us.empty())
						uniforms.insert(hash(fname.c_str()) , us);
				}
			}
			else if(xml.is_the_node((const utf8*)"tech", 5))
			{
				c_technique* tech = new c_technique();
				u32 id = hash(xml.get_attribute_as_string((const utf8*)"name" , 5).c_str());
				if(xml.set_to_first_child((const utf8*)"pass" , 5))
				{
					do
					{
						c_pass* pass = tech->add_pass(xml.get_attribute_as_string((const utf8*)"name" , 5));
						c_string vname = xml.get_attribute_as_string((const utf8*)"vert" , 5);
						c_string fname = xml.get_attribute_as_string((const utf8*)"frag" , 5);
						pass->m_vs_id = hash(vname.c_str());
						pass->m_fs_id = hash(fname.c_str());
						AUTO(TYPEOF(c_map<u32 , c_vector<c_shader::s_sampler_desc> >)::iterator , iter1 ,samplers.find(hash(vname.c_str())));
						if(iter1!=samplers.end())
						{
							if(iter1->second().size())
							{
								FOREACH(TYPEOF(c_vector<c_shader::s_sampler_desc>)::iterator , iter , iter1->second())
                                pass->add_sampler(iter->name , iter->unit);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<c_shader::s_sampler_desc> >)::iterator , iter2 ,samplers.find(hash(fname.c_str())));
						if(iter2!=samplers.end())
						{
							if(iter2->second().size())
							{
								FOREACH(TYPEOF(c_vector<c_shader::s_sampler_desc>)::iterator , iter , iter2->second())
                                pass->add_sampler(iter->name , iter->unit);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<c_shader::s_unifrom_desc> >)::iterator , iter3 ,uniforms.find(hash(vname.c_str())));
						if(iter3!=uniforms.end())
						{
							if(iter3->second().size())
							{
								FOREACH(TYPEOF(c_vector<c_shader::s_unifrom_desc>)::iterator , iter , iter3->second())
                                pass->add_uniform(iter->name, iter->type , iter->count ,true);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<c_shader::s_unifrom_desc> >)::iterator , iter4 ,uniforms.find(hash(fname.c_str())));
						if(iter4!=uniforms.end())
						{
							if(iter4->second().size())
							{
								FOREACH(TYPEOF(c_vector<c_shader::s_unifrom_desc>)::iterator , iter , iter4->second())
                                pass->add_uniform(iter->name, iter->type , iter->count , false);
							}
						}
						AUTO(TYPEOF(c_map<u32 , c_vector<c_shader::s_attrib_desc> >)::iterator , iter5 ,attributes.find(hash(vname.c_str())));
						if(iter5!=attributes.end())
						{
							if(iter5->second().size())
							{
								FOREACH(TYPEOF(c_vector<c_shader::s_attrib_desc>)::iterator , iter , iter5->second())
								{
									if(iter->type == L"float")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT1 , iter->name);
									else if(iter->type == L"vec2")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT2 , iter->name);
									else if(iter->type == L"vec3")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT3 , iter->name);
									else if(iter->type == L"vec4")
										pass->add_attribute(iter->semantic , iter->index , VET_FLOAT4 , iter->name);
									else if(iter->type == L"int")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER1 , iter->name);
									else if(iter->type == L"ivec2")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER2 , iter->name);
									else if(iter->type == L"ivec3")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER3 , iter->name);
									else if(iter->type == L"ivec4")
										pass->add_attribute(iter->semantic , iter->index , VET_INTEGER4 , iter->name);
									else assert(0);
								}
							}
						}
						if(xml.set_to_first_child((const utf8*)"renderstate" , 12))
						{
							do
							{
								c_string name = xml.get_attribute_as_string((const utf8*)"name" , 5);
								c_string val  = xml.get_attribute_as_string((const utf8*)"value" , 6);
								if(name == L"depth_enable")
									pass->m_states[RS_DEPTH_ENABLE] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"depth_func")
									pass->m_states[RS_DEPTH_FUNC] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"blend_enable")
									pass->m_states[RS_BLEND_ENABLE] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"blend_op")
									pass->m_states[RS_BLEND_OP] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"blend_op_alpha")
									pass->m_states[RS_BLEND_OP_ALPHA] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"src_factor")
									pass->m_states[RS_SRC_FACTOR] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"dst_factor")
									pass->m_states[RS_DST_FACTOR] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"src_alpha_factor")
									pass->m_states[RS_SRC_ALPHA_FACTOR] = ((c_shader*)m_res)->_str_2_glenum(val);
								else if(name == L"dst_alpha_factor")
									pass->m_states[RS_DST_ALPHA_FACTOR] = ((c_shader*)m_res)->_str_2_glenum(val);
								else assert(0);
							} while (xml.set_to_next_sibling((const utf8*)"renderstate" , 12));
						}
						xml.set_to_parent();
					}while(xml.set_to_next_sibling((const utf8*)"pass" , 5));
				}
				((c_shader*)m_res)->m_teches.insert(id, tech);
			}
			else;
		} while (xml.retrieve());
#if defined(O2D_USE_GL_API)
		c_string prefix = L"#version 110\n";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , ((c_shader*)m_res)->m_vs_prog)
        iter->second() = prefix + iter->second();
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , ((c_shader*)m_res)->m_fs_prog)
        iter->second() = prefix + iter->second();
#elif defined(O2D_USE_GLES_API)
		c_string prefix = L"#version 100\n";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , ((c_shader*)m_res)->m_vs_prog)
        iter->second() = prefix + iter->second();
		prefix << L"precision lowp float";
		FOREACH(TYPEOF(c_map<u32 , c_string>)::iterator , iter , ((c_shader*)m_res)->m_fs_prog)
        iter->second() = prefix + iter->second();
#endif
		return true;
	}
	//--------------------------------------------------------
	void c_shader_loader::unload()
	{
		FOREACH(TYPEOF(c_map<u32 , c_technique*>)::iterator , iter , ((c_shader*)m_res)->m_teches)
        delete iter->second();
		delete m_res;
		m_res = nullptr;
	}
}