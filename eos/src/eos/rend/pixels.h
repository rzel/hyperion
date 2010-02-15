#ifndef EOS_REND_PIXELS_H
#define EOS_REND_PIXELS_H
//------------------------------------------------------------------------------
// Copyright 2006 Tom Haines

/// \file pixels.h
/// Provides an interface to something you can draw on.

#include "eos/types.h"
#include "eos/bs/colours.h"
#include "eos/bs/geo2d.h"
#include "eos/svt/field.h"

namespace eos
{
 namespace rend
 {
//------------------------------------------------------------------------------
/// A very basic interface that is implimented by things that can be drawn on.
/// Allows for a basic set of primatives to be drawn. An implimentor must clip
/// correctly.
class EOS_CLASS Pixels : public Deletable
{
 public:
  /// &nbsp;
   ~Pixels() {}

  /// &nbsp;
   virtual nat32 Width() = 0;
   
  /// &nbsp;
   virtual nat32 Height() = 0;

 
  /// Draws a single pixel.
   virtual void Point(const bs::Pos & pos,const bs::ColourRGB & col) = 0;
   
  /// Draws a line.
   virtual void Line(const bs::Pos & start,const bs::Pos & end,const bs::ColourRGB & col) = 0;

  /// Draws the contents of a rectangle, including the boundary line.
   virtual void Rectangle(const bs::Rect & rect,const bs::ColourRGB & col) = 0;
      
  /// Draws the line arround a rectangle. To draw a rectangle with a border
  /// line simply call Rectangle then Border.
   virtual void Border(const bs::Rect & rect,const bs::ColourRGB & col) = 0;
   
  /// Draws an image, represented by a svt::Field<bs::ColRGB>.
  /// Takes the rect chunk from image and renders it at position pos in this.
  /// Clips in regards to both this and the given image.
   virtual void Image(const bs::Rect & rect,const bs::Pos & pos,const svt::Field<bs::ColRGB> & image) = 0;
};

//------------------------------------------------------------------------------
 };
};
#endif
