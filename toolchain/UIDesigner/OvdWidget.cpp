#include "OvdWidget.h"
#include "uifx.glfx"
#include "frame.glfx"
#include "back.glfx"
#include "Algorithm.h"
#include "Win32Window.h"
OvdWidget::OvdWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	,_tech(NULL)
	, _gbo(NULL)
	, _rectGbo(NULL)
	, _bgLoader(NULL)
	, _ready(false)
	, _drawFrame(false)
	, _drawRect(false)
	, _drawSizeController(false)
	, tex_loader(NULL)
{
	new c_system;
	new c_file_manager;
	new c_device;
	new c_resource_manager;
	new c_ui_manager;	
	new c_input_manager;
	_uiLoader = new c_shader_loader((const unsigned char*)uifx);
	_frameLoader = new c_shader_loader((const unsigned char*)framefx);
	_bgLoader = new c_shader_loader((const unsigned char*)bgfx);
	_gbo = new c_geometry_buffer(RO_LINES);
	_rectGbo = new c_geometry_buffer(RO_LINES);
	_sizeControllerGbo = new c_geometry_buffer(RO_LINES);
	_bgGbo = new c_geometry_buffer(RO_TRIANGLE_STRIP);
}

OvdWidget::~OvdWidget()
{
	delete _bgGbo;
	delete _gbo;
	delete _rectGbo;
	delete _bgLoader;
	delete _frameLoader;
	delete _sizeControllerGbo;
	delete _uiLoader;
	delete c_system::get_singleton_ptr();
	delete c_file_manager::get_singleton_ptr();
	delete c_resource_manager::get_singleton_ptr();
	delete c_device::get_singleton_ptr();
	delete c_ui_manager::get_singleton_ptr();
	delete c_input_manager::get_singleton_ptr();
}

void OvdWidget::init(SCREEN_DPI dpi)
{
	int width, height;
	switch (dpi)
	{
	case SP_XGA:
		width = 800;
		height = 600;
		break;
	case SP_HD:
		width = 800;
		height = 450;
		break;
	case SP_WXGA:
		width = 800;
		height = 500;
		break;
	}
	mdpi = dpi;
	_ready = true;
	c_system::get_singleton_ptr()->initialize();
	c_device::get_singleton_ptr()->initialize(L"GUIEditor" , width , height ,false , NULL , (s32)winId());
	c_device::get_singleton_ptr()->set_raster_state(true, c_color(0.f, 0.f, 0.f, 0.f));
	c_file_manager::get_singleton_ptr()->initialize();
	c_resource_manager::get_singleton_ptr()->initialize(nullptr);
	static_cast<c_shader*>(c_resource_manager::get_singleton_ptr()->fetch_buildin_res(BRI_UI_GLFX , _uiLoader))->active(L"ui_gl");
	c_shader* sha = static_cast<c_shader*>(c_resource_manager::get_singleton_ptr()->fetch_buildin_res(BRI_SPRITE_GLFX , _frameLoader));
	sha->active(L"frame");
	_tech = sha->get_tech(L"frame");
	sha = static_cast<c_shader*>(c_resource_manager::get_singleton_ptr()->fetch_buildin_res(BRI_OUTLINE_GLFX , _bgLoader));
	sha->active(L"bg");
	_bgTech = sha->get_tech(L"bg");
	c_ui_manager::get_singleton_ptr()->initialize();
	c_input_manager::get_singleton_ptr()->initialize();
	c_vector<c_geometry_buffer::s_vertex_element> fmts;
	c_geometry_buffer::s_vertex_element fmt1;
	fmt1.semantic = VES_POSITION;
	fmt1.type = VET_FLOAT2;
	fmt1.index = 0;
	c_geometry_buffer::s_vertex_element fmt2;
	fmt2.semantic = VES_POSITION;
	fmt2.type = VET_FLOAT1;
	fmt2.index = 1;
	fmts.push_back(fmt1);
	fmts.push_back(fmt2);
	//_gbo->bind_vertex_buffer(BU_STREAM_DRAW , NULL , 0 , fmts);
	_rectGbo->bind_vertex_buffer(BU_STREAM_DRAW, NULL, 8*sizeof(float)*3, fmts);
	//_sizeControllerGbo->bind_vertex_buffer(BU_STREAM_DRAW, NULL, 0, fmts);
	fmt1.semantic = VES_POSITION;
	fmt1.type = VET_FLOAT2;
	fmt1.index = 0;
	fmt2.semantic = VES_TEXTURE_COORD;
	fmt2.type = VET_FLOAT2;
	fmt2.index = 0;
	fmts.clear();
	fmts.push_back(fmt1);
	fmts.push_back(fmt2);
	c_stream ss;
	ss.resize(4*4*sizeof(float));
	ss.write_f32(-1);
	ss.write_f32(1);
	ss.write_f32(0);
	ss.write_f32(0);

	ss.write_f32(1);
	ss.write_f32(1);
	ss.write_f32(1);
	ss.write_f32(0);

	ss.write_f32(-1);
	ss.write_f32(-1);
	ss.write_f32(0);
	ss.write_f32(1);

	ss.write_f32(1);
	ss.write_f32(-1);
	ss.write_f32(1);
	ss.write_f32(1);
	_bgGbo->bind_vertex_buffer(BU_STATIC_DRAW, ss.get_pointer(), ss.get_size(), fmts);
	setAttribute(Qt::WA_OpaquePaintEvent , true);
	//setAttribute(Qt::WA_PaintOnScreen , true);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	startTimer(25);
	setFocus();
}

void OvdWidget::close()
{
	_frameLoader->unload();
	_uiLoader->unload();
	c_input_manager::get_singleton_ptr()->shutdown();
	c_ui_manager::get_singleton_ptr()->shutdown();
	c_resource_manager::get_singleton_ptr()->shutdown();
	c_file_manager::get_singleton_ptr()->shutdown();
	c_device::get_singleton_ptr()->shutdown();
	c_system::get_singleton_ptr()->shutdown();
}

void OvdWidget::sendMouseEventToEngine( QMouseEvent* event, bool pressedDown )
{
	//c_mouse* m = c_input_manager::get_singleton_ptr()->get_mouse();
	switch ( event->button() )
	{
	case Qt::LeftButton:
// 		m->set_button(MB_LEFT_BTN);
// 		c_input_manager::get_singleton_ptr()->inject_event();
		c_input_manager::get_singleton_ptr()->inject_event(pressedDown?IE_TOUCH_FINISH:IE_TOUCH_START, c_float2(event->x() , event->y()));
		break;
	case Qt::MidButton:
// 		m->set_button(MB_MIDDLE_BTN);
// 		c_input_manager::get_singleton_ptr()->inject_event(pressedDown?IE_MBUTTONDOWN:IE_MBUTTONUP);
		break;
	case Qt::RightButton:
// 		m->set_button(MB_RIGHT_BTN);
// 		c_input_manager::get_singleton_ptr()->inject_event(pressedDown?IE_RBUTTONDOWN:IE_RBUTTONUP);
		c_input_manager::get_singleton_ptr()->inject_event(pressedDown?IE_TOUCH_FINISH:IE_TOUCH_START, c_float2(event->x() , event->y()));
		break;
	default:
		return;
	}
	//m->set_position(c_float2(event->x() , event->y()));
	//m->set_wheel(0);
}

void OvdWidget::sendKeyEventToEngine( QKeyEvent* event, bool pressedDown )
{
}

void OvdWidget::setWidgetFrame( const QVector<c_widget*>& widgets , c_widget* widget)
{
	if(widgets.empty())
	{
		_drawFrame = false;
	}
	else
	{
		_drawFrame = true;
		c_stream ss;
		size_t sz = widgets.size();
		if(widget)
			sz++;
		ss.resize(sz*8*sizeof(float)*3);
		if(widget)
		{
			c_rect rc0 = widget->get_region();
			ss.write_f32(rc0.lt_pt().x());
			ss.write_f32(rc0.lt_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.rb_pt().x());
			ss.write_f32(rc0.lt_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.rb_pt().x());
			ss.write_f32(rc0.lt_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.rb_pt().x());
			ss.write_f32(rc0.rb_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.rb_pt().x());
			ss.write_f32(rc0.rb_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.lt_pt().x());
			ss.write_f32(rc0.rb_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.lt_pt().x());
			ss.write_f32(rc0.rb_pt().y());
			ss.write_f32(1.f);
			ss.write_f32(rc0.lt_pt().x());
			ss.write_f32(rc0.lt_pt().y());
			ss.write_f32(1.f);
		}
		for(unsigned int i = 0 ; i < widgets.size() ; ++i)
		{
			c_rect rc = widgets[i]->get_region();
			ss.write_f32(rc.lt_pt().x());
			ss.write_f32(rc.lt_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.rb_pt().x());
			ss.write_f32(rc.lt_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.rb_pt().x());
			ss.write_f32(rc.lt_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.rb_pt().x());
			ss.write_f32(rc.rb_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.rb_pt().x());
			ss.write_f32(rc.rb_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.lt_pt().x());
			ss.write_f32(rc.rb_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.lt_pt().x());
			ss.write_f32(rc.rb_pt().y());
			ss.write_f32(0.f);
			ss.write_f32(rc.lt_pt().x());
			ss.write_f32(rc.lt_pt().y());
			ss.write_f32(0.f);
		}
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		c_geometry_buffer::s_vertex_element fmt1;
		fmt1.semantic = VES_POSITION;
		fmt1.type = VET_FLOAT2;
		fmt1.index = 0;
		c_geometry_buffer::s_vertex_element fmt2;
		fmt2.semantic = VES_POSITION;
		fmt2.type = VET_FLOAT1;
		fmt2.index = 1;
		fmts.push_back(fmt1);
		fmts.push_back(fmt2);
		_gbo->bind_vertex_buffer(BU_STREAM_DRAW , ss.get_pointer() , ss.get_size() , fmts);
		//_gbo->uploadvb(0 , ss.get_pointer() , ss.get_size());
	}
}

void OvdWidget::drawRect( const QPoint &p1, const QPoint &p2 )
{
	if(p1 == p2)
	{
		_drawRect = false;
		return;
	}

	c_stream ss;
	ss.resize(8*sizeof(float)*3);

	c_rect rc0(p1.x(), p1.y(), p2.x(), p2.y());
	ss.write_f32(rc0.lt_pt().x());
	ss.write_f32(rc0.lt_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.rb_pt().x());
	ss.write_f32(rc0.lt_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.rb_pt().x());
	ss.write_f32(rc0.lt_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.rb_pt().x());
	ss.write_f32(rc0.rb_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.rb_pt().x());
	ss.write_f32(rc0.rb_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.lt_pt().x());
	ss.write_f32(rc0.rb_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.lt_pt().x());
	ss.write_f32(rc0.rb_pt().y());
	ss.write_f32(0.5f);
	ss.write_f32(rc0.lt_pt().x());
	ss.write_f32(rc0.lt_pt().y());
	ss.write_f32(0.5f);

	void* data(NULL);
	_rectGbo->uploadvb(0 , ss.get_pointer() , ss.get_size());
	_drawRect = true;
}

void OvdWidget::setBackImage( void* mem , int w , int h )
{
	if(!mem&&tex_loader)
	{
		delete tex_loader;
		tex_loader = NULL;
		return;
	}
	tex_loader = new c_tex_loader(false ,w , h);
	c_resource_manager::get_singleton_ptr()->fetch_resource(tex_loader);
	static_cast<c_texture*>(tex_loader->get_res())->upload(0 , 0 , 0 , w, h , mem , w*h*sizeof(u32));
	_bgTech->get_pass(0)->get_sampler(L"image_tex")->tex = (c_texture*)tex_loader->get_res();
}

void OvdWidget::drawSizeControllers( const QVector<SizeController> &rects )
{
	if(rects.empty())
	{
		_drawSizeController = false;
	}
	else
	{
		c_stream ss;
		size_t sz = rects.size();
		ss.resize(sz*8*sizeof(float)*3);
		QVectorIterator<SizeController> iter(rects);
		while(iter.hasNext())
		{
			QRect rect = iter.next().getRect();
			ss.write_f32((float)rect.left());
			ss.write_f32((float)rect.top());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.right());
			ss.write_f32((float)rect.top());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.right());
			ss.write_f32((float)rect.top());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.right());
			ss.write_f32((float)rect.bottom());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.right());
			ss.write_f32((float)rect.bottom());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.left());
			ss.write_f32((float)rect.bottom());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.left());
			ss.write_f32((float)rect.bottom());
			ss.write_f32(0.9f);
			ss.write_f32((float)rect.left());
			ss.write_f32((float)rect.top());
			ss.write_f32(0.9f);
		}
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		c_geometry_buffer::s_vertex_element fmt1;
		fmt1.semantic = VES_POSITION;
		fmt1.type = VET_FLOAT2;
		fmt1.index = 0;
		c_geometry_buffer::s_vertex_element fmt2;
		fmt2.semantic = VES_POSITION;
		fmt2.type = VET_FLOAT1;
		fmt2.index = 1;
		fmts.push_back(fmt1);
		fmts.push_back(fmt2);
		_sizeControllerGbo->bind_vertex_buffer(BU_STREAM_DRAW, ss.get_pointer(), ss.get_size(), fmts);
		_drawSizeController = true;
	}
}

void OvdWidget::updateEngine()
{
	if(_ready)
	{
		c_window* win = c_device::get_singleton_ptr()->get_window();
		if(win)
		{
			m_cur_time = TIME;
			unsigned int dt = clamp((m_cur_time - m_prev_time), 0, 1000);
			m_prev_time = m_cur_time;

			c_device::get_singleton_ptr()->set_framebuffer(c_device::get_singleton_ptr()->get_window()->get_dim());
			win->clear(CM_COLOR|CM_DEPTH);
			if(tex_loader)
				c_device::get_singleton_ptr()->draw(_bgTech , _bgGbo);
			c_ui_manager::get_singleton_ptr()->step(0);
			c_ui_manager::get_singleton_ptr()->draw(0);
			if(_drawFrame)
				c_device::get_singleton_ptr()->draw(_tech , _gbo);
			if(_drawRect)
				c_device::get_singleton_ptr()->draw(_tech , _rectGbo);
			if(_drawSizeController)
				c_device::get_singleton_ptr()->draw(_tech, _sizeControllerGbo);
			win->swap_buffers();
			c_input_manager::get_singleton_ptr()->reset();
		}
	}
}

void OvdWidget::enterEvent( QEvent * event )
{
}

void OvdWidget::timerEvent( QTimerEvent* event )
{
	if(_ready)
		updateEngine();
}

void OvdWidget::paintEvent( QPaintEvent* event )
{
	if(_ready)
		updateEngine();
}

void OvdWidget::resizeEvent( QResizeEvent* event )
{
	if(_ready)
	{
		//c_win32_window* win = (c_win32_window*)c_device::get_singleton_ptr()->get_window();

		int width, height;
		switch (mdpi)
		{
		case SP_XGA:
			width = 800;
			height = 600;
			break;
		case SP_HD:
			width = 800;
			height = 450;
			break;
		case SP_WXGA:
			width = 800;
			height = 500;
			break;
		}
		c_win32_window* win = (c_win32_window*)c_device::get_singleton_ptr()->get_window();
		win->resize(event->size().width(), event->size().height());
		f32* ssz = _tech->get_pass(0)->get_uniform(L"screensz")->var.fpvar;
		ssz[0] = 1.0 / (f32)event->size().width();
		ssz[1] = 1.0 / (f32)event->size().height();
	}
}

void OvdWidget::mousePressEvent( QMouseEvent* event )
{
	if (_ready)
	{
		sendMouseEventToEngine( event, true );
	}
	event->ignore();
}

void OvdWidget::mouseReleaseEvent( QMouseEvent* event )
{
	if (_ready)
	{
		sendMouseEventToEngine( event, false );
	}
	event->ignore();
}

void OvdWidget::mouseMoveEvent( QMouseEvent * event )
{
	if (_ready)
	{
// 		c_mouse* m = c_input_manager::get_singleton_ptr()->get_mouse();
// 		c_input_manager::get_singleton_ptr()->inject_event(IE_MOUSEMOVE);
// 		m->set_position(c_float2(event->x() , event->y()));
// 		m->set_wheel(0);
		c_input_manager::get_singleton_ptr()->inject_event(IE_TOUCH_MOVE , c_float2(event->x() , event->y()));
	}
	event->ignore();
}

void OvdWidget::wheelEvent( QWheelEvent* event )
{
	if (_ready && event->orientation() == Qt::Vertical )
	{
// 		c_mouse* m = c_input_manager::get_singleton_ptr()->get_mouse();
// 		c_input_manager::get_singleton_ptr()->inject_event(IE_MOUSEMOVE);
// 		m->set_position(c_float2(0 , 0));
// 		m->set_wheel(event->delta()/120.f);
	}
	event->ignore();
}

void OvdWidget::keyPressEvent( QKeyEvent* event )
{
	if (_ready)
	{
		sendKeyEventToEngine( event, true );
	}
	event->ignore();
}

void OvdWidget::keyReleaseEvent( QKeyEvent* event )
{
	if (_ready)
	{
		sendKeyEventToEngine( event, false );
	}
	event->ignore();
}