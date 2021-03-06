//------------------------------------------------------------------------------
// Copyright 2006 Tom Haines

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "eos/rend/viewers.h"

namespace eos
{
 namespace rend
 {
//------------------------------------------------------------------------------
SimplePinhole::SimplePinhole(nat32 width,nat32 height,real32 focalLength)
{
 Transform t;
  t.t = bs::Vert(0.0,0.0,0.0);
  t.r = math::Quaternion(0.0,bs::Vert(0.0,1.0,0.0));
  t.s = 1.0;
 transform = OpTran(t);
 
 SetParas(width,height,focalLength);
}

void SimplePinhole::SetParas(nat32 w,nat32 h,real32 fl)
{
 width = w;
 height = h;
 focalLength = fl;

 invInt[0][0] = focalLength * real32(width)/35.0; invInt[0][1] = 0.0;          invInt[0][2] = 0.5*real32(width);
 invInt[1][0] = 0.0;                              invInt[1][1] = invInt[0][0]; invInt[1][2] = 0.5*real32(height);
 invInt[2][0] = 0.0;                              invInt[2][1] = 0.0;          invInt[2][2] = 1.0;
 
 math::Mat<3> temp;
 math::Inverse(invInt,temp);
}

bit SimplePinhole::ConstantStart(bs::Vert & out) const
{
 bs::Vert z(0.0,0.0,0.0);
 transform.ToWorld(z,out);
 return true;
}

void SimplePinhole::ViewRay(real32 x,real32 y,bs::Ray & out) const
{
 math::Vect<3> in;
  in[0] = x;
  in[1] = y;
  in[2] = 1.0;
  
 bs::Ray r;
  math::MultVect(invInt,in,r.n);
  r.n.Normalise();
  r.s = bs::Vert(0.0,0.0,0.0);
  
 transform.ToWorld(r,out);
}

//------------------------------------------------------------------------------
 };
};
