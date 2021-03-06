#ifndef EOS_REND_VIEWERS_H
#define EOS_REND_VIEWERS_H
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


/// \file viewers.h
/// Provides various camera models.

#include "eos/types.h"
#include "eos/rend/renderer.h"

namespace eos
{
 namespace rend
 {
//------------------------------------------------------------------------------
/// A very simple pinhole camera model with square pixels, its only parameters
/// are the size of the image and the 35mm equivalent focal length in mm.
/// You can off course also provide a transformation to move it from its 
/// default (0,0,0) spot looking down the z axis in the -ve direction.
/// The 35mm measurement is made along the x axis only, the y axis size has
/// no effect here.
class EOS_CLASS SimplePinhole : public Viewer
{
 public:
  /// &nbsp;
   SimplePinhole(nat32 width,nat32 height,real32 focalLength = 50.0);

  /// &nbsp;
   ~SimplePinhole() {}

  
  /// The camera is at (0,0,0) pointing down the -ve Z axis in the local space
  /// of the given transform.
   void SetTransform(const OpTran & tran) {transform = tran;}
  
  /// The focal length is in 35mm equivalent terms, in mm.
  /// (i.e. 35-50mm is normal, 28 and less wideangle, 100 and more telephoto.)
   void SetParas(nat32 width,nat32 height,real32 focalLength = 50.0);


  /// &nbsp;
   nat32 Width() const {return width;}

  /// &nbsp;
   nat32 Height() const {return height;}


  /// &nbsp;
   bit ConstantStart(bs::Vert & out) const;

  /// &nbsp;
   void ViewRay(real32 x,real32 y,bs::Ray & out) const;


  /// &nbsp;
   cstrconst TypeString() const {return "eos::rend::SimplePinhole";}


 protected:
  nat32 width;
  nat32 height;
  real32 focalLength;

  OpTran transform;
  
  // An inverse intrinsic matrix - given a homogenous screen coordinate
  // multiplied by this you get the unnormalised ray direction.
   math::Mat<3> invInt;
};

//------------------------------------------------------------------------------
 };
};
#endif
