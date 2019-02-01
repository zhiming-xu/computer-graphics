// Original file Copyright CMU462 Fall 2015: 
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#ifndef CGL_SVGPARSER_H
#define CGL_SVGPARSER_H

#include "svg.h"

namespace CGL { 

class SVGParser {
 public:

  static int load( const char* filename, SVG* svg );
  static int save( const char* filename, const SVG* svg );
 
 private:
  
  // parse a svg file
  static void parseSVG       ( XMLElement* xml, SVG* svg );

  // parse shared properties of svg elements
  static void parseElement   ( XMLElement* xml, SVGElement* element );

  // parse a common texture file
  static void parseTexture   ( XMLElement* xml );
  
  // parse type specific properties
  static void parsePoint     ( XMLElement* xml, Point*    point       );
  static void parseLine      ( XMLElement* xml, Line*     line        );
  static void parsePolyline  ( XMLElement* xml, Polyline* polyline    );
  static void parseRect      ( XMLElement* xml, Rect*     rect        );
  static void parsePolygon   ( XMLElement* xml, Polygon*  polygon     );
  static void parseImage     ( XMLElement* xml, Image*    image       );
  static void parseGroup     ( XMLElement* xml, Group*    group       );

  static void parseColorTri  ( XMLElement* xml, ColorTri* ctri       );
  static void parseTexTri    ( XMLElement* xml, TexTri*   ttri       );

  static SVG *curr_svg;
  static std::string dir;

}; // class SVGParser

}

#endif // CGL_SVGPARSER_H