//------------------------------------------------------------------------------
// Copyright 2005 Tom Haines

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

#include "eos/typestring.h"

#include <string.h>

#include "eos/mem/alloc.h"

namespace eos
{
//------------------------------------------------------------------------------
void GlueStr::Fold()
{
 // Calculate length of new string, recording string lengths to save recalculation latter...
 int32 totLen = 0; 
 int32 rhsLen[nMax];
 for (nat32 i=0;i<n;i++)
 {
  rhsLen[i] = strlen(rhs[i]);
  totLen += rhsLen[i];
 }
 
 // If we allready have some ret we need to append to it, otherwise we just start afresh...
  cstr pos;
  if (ret)
  {
   int32 retLen = strlen(ret);
   cstr newRet = new cstrchar[retLen+totLen+1];
   memcpy(newRet,ret,retLen);
   
   delete[] ret;
   ret = newRet;
   pos = ret+retLen;
  }
  else
  {
   ret = new cstrchar[totLen+1];  
   pos = ret;
  }
  
 // Now copy in the data, and null the end of the string...
  for (nat32 i=0;i<n;i++)
  {
   memcpy(pos,rhs[i],rhsLen[i]);
   pos += rhsLen[i];
  }
  *pos = 0;
 
 // And reset n...
  n = 0; 
}

//------------------------------------------------------------------------------
};
