/**
    Copyright (C) powturbo 2015-2016
    GPL v2 License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    - email    : powturbo [AT] gmail.com
    - github   : https://github.com/powturbo
    - homepage : https://sites.google.com/site/powturbo/
    - twitter  : https://twitter.com/powturbo

    TurboRLE - "Most efficient and fastest Run Length Encoding"
**/
#include <stdint.h>
//------------------------------------- Variable Byte -----------------------------------------------------
#define vbputa(_op_, _x_, _act_) {\
       if(likely(_x_ < (1<< 7))) {		   			  *_op_++ = _x_ << 1; 			 		      		_act_;}\
  else if(likely(_x_ < (1<<14))) { *(unsigned short *)_op_ = _x_ << 2 | 0x01; _op_ += 2; 		      	_act_;}\
  else if(likely(_x_ < (1<<21))) { *(unsigned short *)_op_ = _x_ << 3 | 0x03; _op_ += 2; *_op_++ = _x_ >> 13; _act_;}\
  else if(likely(_x_ < (1<<28))) { *(unsigned       *)_op_ = _x_ << 4 | 0x07; _op_ += 4; 		      	_act_;}\
  else { 		           *(unsigned       *)_op_ = _x_ << 4 | 0x0f; _op_ += 4; *_op_++ = _x_ >> 28; 	_act_;}\
}
 
#define vbgeta(_ip_, _x_, _act_) do { _x_ = *_ip_;\
       if(!(_x_ & (1<<0))) { _x_			     >>= 1; 		                      _ip_++;    _act_;}\
  else if(!(_x_ & (1<<1))) { _x_ = (*(unsigned short *)_ip_) >>  2;		              _ip_ += 2; _act_;}\
  else if(!(_x_ & (1<<2))) { _x_ = (*(unsigned short *)_ip_) >>  3 | *(_ip_+2) << 13; _ip_ += 3; _act_;}\
  else if(!(_x_ & (1<<3))) { _x_ = (*(unsigned       *)_ip_) >>  4; 		      	  _ip_ += 4; _act_;}\
  else 			   	       { _x_ = (*(unsigned       *)_ip_) >>  4 | *(_ip_+4) << 28; _ip_ += 5; _act_;}\
} while(0)

#define vbput(_op_, _x_) { unsigned _x = _x_; vbputa(_op_, _x, ;); }
#define vbget(_ip_, _x_) vbgeta(_ip_, _x_, ;);

#define vbxput(_op_, _x_) { *_op_++ = _x_; if(unlikely((_x_) >= 0xff)) { unsigned _xi = (_x_) - 0xff; _op_[-1] = 0xff; vbput(_op_, _xi); } }
#define vbxget(_ip_, _x_) { _x_ = *_ip_++; if(unlikely( _x_ == 0xff)) { vbget(_ip_,_x_); _x_+=0xff; } }

#define vbzput(_op_, _x_, _m_, _emap_) do { if(unlikely((_x_) < _m_)) *_op_++ = _emap_[_x_]; else { unsigned _xi = (_x_) - _m_; *_op_++ = _emap_[_m_]; vbput(_op_, _xi); } } while(0)
#define vbzget(_ip_, _x_, _m_, _e_) { _x_ = _e_; if(unlikely(_x_ == _m_)) { vbget(_ip_,_x_); _x_+=_m_; } }

