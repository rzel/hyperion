#ifndef EOS_FIT_DISP_FISH_H
#define EOS_FIT_DISP_FISH_H
//------------------------------------------------------------------------------
// Copyright 2008 Tom Haines

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


/// \namespace eos::fit
/// Provides tools for 'fitting' to data sets.

/// \file disp_fish.h
/// Provides an algorithm for fitting Fisher distributions to disparity maps, 
/// given the DSC.

#include "eos/types.h"
#include "eos/time/progress.h"
#include "eos/bs/geo3d.h"
#include "eos/cam/files.h"
#include "eos/ds/arrays2d.h"
#include "eos/svt/field.h"
#include "eos/stereo/dsi.h"
#include "eos/math/stats_dir.h"

namespace eos
{
 namespace fit
 {
//------------------------------------------------------------------------------
/// A standard algorithm class that is given a disparity map and a DSC as well 
/// as parameters - it will then fit a Fisher distribution to each pixel which 
/// will be output as a vector indicating both direction and concentration.
/// This is not a fast algorithm.
class EOS_CLASS DispFish
{
 public:
  /// &nbsp;
   DispFish();
   
  /// &nbsp;
   ~DispFish();


  /// Sets the disparity map and DSC to be used. Must be called before run.
  /// Optional set a multiplier for the dsc costs, to emphasis differences.
   void Set(const svt::Field<real32> & disp,const stereo::DSC & dsc,real32 dscMult = 1.0);
   
  /// Sets the mask, optional - will set masked areas to a concentration of 0.
   void SetMask(const svt::Field<bit> & mask);

  /// Sets the parameters to convert to depth - a cam::CameraPair
   void SetPair(const cam::CameraPair & pair);

  /// Sets the search range for calculating concentration - this is the slow bit.
  /// Note that the scaling is nuts here, requiring the handling of (range*2+1)^3 values,
  /// i.e. the algorithm is O(width*height*(range*2+1)^3) - a very big number.
  /// Default is 3
   void SetRange(nat32 range);
   
  /// Sets the bias, which biases it towards higher concentrations, 0 for no
  /// bias, which is the default.
   void SetBias(real32 bias);
   
  /// Sets the clamping range for the concentration, defaults to 1 to 24.
   void SetClamp(real32 minCon,real32 maxCon);


  /// Runs the algorithm.
   void Run(time::Progress * prog = null<time::Progress*>());
   
   
  /// Extracts the results.
   void Get(svt::Field<bs::Vert> & fish);

  /// Extracts the results.
   void Get(svt::Field<math::Fisher> & fish);
   
  /// Extracts a result.
   bs::Vert & GetFish(nat32 x,nat32 y);


  /// &nbsp;
   inline cstrconst TypeString() const {return "eos::fit::DispFish";}


 private:
  // Input...
   svt::Field<real32> disp;
   svt::Field<bit> mask;
   const stereo::DSC * dsc;
   real32 dscMult;
   cam::CameraPair pair;
   nat32 range;
   real32 minCon;
   real32 maxCon;
   real32 bias;
   
  // Output...
   ds::Array2D<bs::Vert> out;
   
  // Runtime...
   // Structure for storing information for each disparity considered for each pixel...
    struct Pixel
    {
     bs::Vert pos;
     real32 weight;
     
     static inline cstrconst TypeString() {return "eos::fit::DispFish::Pixel";}
    };
    
   // Structure for storing a r-contribution and weight, used during estimate
   // of concentration...
    struct Rcont
    {
     real32 r;
     real32 weight;
     
     bit operator < (const Rcont & rhs) const {return this->r<rhs.r;}
    };
};

//------------------------------------------------------------------------------
 };
};
#endif
