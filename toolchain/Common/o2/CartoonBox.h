/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef CARTOONBOX_H_INCLUDE
#define CARTOONBOX_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_cartoonbox:public c_widget
	{
	private:
		c_geometry_buffer* m_gb;
		c_texture* m_tex;
		c_vector<c_rect> m_frames;
		u32 m_framerate;
		u32 m_last_record;
		u32 m_cur_frame;
		bool m_loop;
		bool m_playing;
	SIGNALS:
		c_signal<void> _sig_play_over;
	public:
		c_cartoonbox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_cartoonbox();
	public:
		inline void set_framerate(u32 rate_) {m_framerate = rate_;}
		inline u32 get_framerate() const {return m_framerate;}
		inline void set_loop(bool flag_) {m_loop = flag_;}
		inline bool is_loop() const{return m_loop;}
		inline bool is_playing() const{return m_playing;}
	public:
		void set_film(const c_string& film_);
		void set_frame(const c_vector<c_rect>& frames_);
		void play();
		void stop();
	protected:
		void _redraw();
		void _update(c_technique* uit_, u32 dt_);
	};
}
#endif