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

#include "eos/fit/disp_norm.h"

#include "eos/file/csv.h"
#include "eos/file/stereo_helpers.h"
#include "eos/math/gaussian_mix.h"

namespace eos
{
 namespace fit
 {
//------------------------------------------------------------------------------
DispNorm::DispNorm()
:dsc(null<stereo::DSC*>()),dscMult(1.0),
range(20),sdCount(2.0),minK(2.5),maxK(10.0),
minSd(0.1),maxSd(10.0),maxIters(1000)
{}

DispNorm::~DispNorm()
{}

void DispNorm::Set(const svt::Field<real32> & d,const stereo::DSC & ds,real32 dscM)
{
 disp = d;
 dsc = &ds;
 dscMult = dscM;
}

void DispNorm::SetMask(const svt::Field<bit> & m)
{
 mask = m;
}

void DispNorm::SetRange(nat32 r,real32 sdC)
{
 range = r;
 sdCount = sdC;
}

void DispNorm::SetClampK(real32 miK,real32 maK)
{
 minK = miK;
 maxK = maK;
}

void DispNorm::SetClamp(real32 minS,real32 maxS)
{
 minSd = minS;
 maxSd = maxS;
}

void DispNorm::SetMaxIters(nat32 mi)
{
 maxIters = mi;
}

void DispNorm::Run(time::Progress * prog)
{
 prog->Push();

 out.Resize(disp.Size(0),disp.Size(1));
 
 ds::Array<real32> buf(range*2+1);
 
 for (int32 y=0;y<int32(out.Height());y++)
 {
  prog->Report(y,out.Height());
  prog->Push();
  for (int32 x=0;x<int32(out.Width());x++)
  {
   prog->Report(x,out.Width());
   
   if (mask.Valid()&&(mask.Get(x,y)==false))
   {
    out.Get(x,y) = 0.0;
   }
   else
   {
    real32 var = math::Sqr(maxSd); // Variance obtained so far.
    real32 mean = disp.Get(x,y);

    int32 minDisp = math::Clamp<int32>(int32(math::Round(mean))-int32(range),
                                       -x,int32(dsc->WidthRight())-x);
    int32 maxDisp = math::Clamp<int32>(int32(math::Round(mean))+int32(range),
                                       -x,int32(dsc->WidthRight())-x);
    
    // Cache the disparity weights to save repeated calculation...
     for (int32 d=minDisp;d<=maxDisp;d++)
     {
      buf[d-minDisp] = math::Exp(-dsc->Cost(x,x+d,y) * dscMult);
     }
     
    // Do iterative re-weighting till convergance...
     for (nat32 iter=0;iter<maxIters;iter++)
     {
      real32 newVar = 0.0;
      real32 newVarW = 0.0;
     
      real32 iSigma = 1.0/math::Clamp(math::Sqrt(var),minK,maxK);
      for (int32 d=minDisp;d<=maxDisp;d++)
      {
       real32 delta = math::Abs(real32(d) - mean);
       if ((delta*iSigma<sdCount)&&(!math::IsZero(delta)))
       {
        real32 weight = (1.0 - math::Cube(1.0 - math::Sqr(delta*iSigma/sdCount)))/math::Sqr(delta*iSigma);
        weight *= buf[d-minDisp];
       
        newVar += weight * math::Sqr(delta);
        newVarW += 0.5*buf[d-minDisp];//weight;
       }
      }
     
      if (!math::IsZero(newVarW)) newVar /= newVarW;
      newVar = math::Clamp(newVar,math::Sqr(minSd),math::Sqr(maxSd));
      bit done = math::Abs(newVar-var) < (var*1e-3);
      var = newVar;
      if (done) break;
     }
   
    // Store...
     out.Get(x,y) = math::Sqrt(var);
   }
  }
  prog->Pop();
 }

 prog->Pop();
}

void DispNorm::Get(svt::Field<real32> & sd)
{
 for (nat32 y=0;y<out.Height();y++)
 {
  for (nat32 x=0;x<out.Width();x++) sd.Get(x,y) = out.Get(x,y);
 }
}

real32 DispNorm::GetSd(nat32 x,nat32 y)
{
 return out.Get(x,y);
}

//------------------------------------------------------------------------------
LaplaceDispNorm::LaplaceDispNorm()
:dsc(null<stereo::DSC*>()),dscMult(1.0),
sd(7.0),sdMult(3.0),minSd(0.1),maxSd(16.0)
{}

LaplaceDispNorm::~LaplaceDispNorm()
{}

void LaplaceDispNorm::Set(const svt::Field<real32> & d,const stereo::DSC & ds,real32 dscM)
{
 disp = d;
 dsc = &ds;
 dscMult = dscM;
}

void LaplaceDispNorm::SetMask(const svt::Field<bit> & m)
{
 mask = m;
}

void LaplaceDispNorm::SetParam(real32 s,real32 mi,real32 ma,real32 sM)
{
 sd = s;
 minSd = mi;
 maxSd = ma;
 sdMult = sM;
}

void LaplaceDispNorm::Run(time::Progress * prog)
{
 prog->Push();

 out.Resize(disp.Size(0),disp.Size(1));
 
 for (int32 y=0;y<int32(out.Height());y++)
 {
  prog->Report(y,out.Height());
  prog->Push();
  for (int32 x=0;x<int32(out.Width());x++)
  {
   prog->Report(x,out.Width());
   
   if (mask.Valid()&&(mask.Get(x,y)==false))
   {
    out.Get(x,y) = 0.0;
   }
   else
   {
    // Initialise the 3 dsc derived costs around and at the selected
    // disparity. A divisor is needed for each...
     real32 costNeg = 0.0, costNegDiv = 0.0;
     real32 cost    = 0.0, costDiv = 0.0;
     real32 costPos = 0.0, costPosDiv = 0.0;
     
    // Calculate the range of disparity values to consider...
     real32 mean = disp.Get(x,y);
     int32 minDisp = math::Clamp<int32>(int32(math::RoundDown(mean-1.0-sd*sdMult)),
                                        -x,int32(dsc->WidthRight())-x);
     int32 maxDisp = math::Clamp<int32>(int32(math::RoundUp(mean+1.0+sd*sdMult)),
                                        -x,int32(dsc->WidthRight())-x);
    
    // Calculate the costs - Gaussian blur of the disparities for the
    // given range...
     for (int32 d=minDisp;d<=maxDisp;d++)
     {
      real32 c = dsc->Cost(x,x+d,y) * dscMult;
      
      real32 weightNeg = math::UnNormGaussian<real32>(sd,mean-1.0-real32(d));
      real32 weight    = math::UnNormGaussian<real32>(sd,mean-real32(d));
      real32 weightPos = math::UnNormGaussian<real32>(sd,mean+1.0-real32(d));
      
      costNeg += weightNeg*c; costNegDiv += weightNeg;
      cost    += weight*c;    costDiv    += weight;
      costPos += weightPos*c; costPosDiv += weightPos;
     }

    // Normalise the final cost, and use central differences to
    // calculate the second differentials...
     costNeg /= costNegDiv;
     cost    /= costDiv;
     costPos /= costPosDiv;
     
     real32 dd = costNeg + costPos - 2.0 * cost;

    // Convert the second differential to a standard deviation, clamp it and store...
     out.Get(x,y) = math::Sqrt(1.0/math::Clamp<real32>(dd,1.0/math::Sqr(maxSd),1.0/math::Sqr(minSd)));
     if (!math::IsFinite(out.Get(x,y))) out.Get(x,y) = maxSd;
   }
  }
  prog->Pop();
 }

 prog->Pop();
}

void LaplaceDispNorm::Get(svt::Field<real32> & sd)
{
 for (nat32 y=0;y<out.Height();y++)
 {
  for (nat32 x=0;x<out.Width();x++) sd.Get(x,y) = out.Get(x,y);
 }
}

real32 LaplaceDispNorm::GetSd(nat32 x,nat32 y)
{
 return out.Get(x,y);
}

//------------------------------------------------------------------------------
 };
};
