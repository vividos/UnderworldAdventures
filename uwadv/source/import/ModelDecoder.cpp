//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file ModelDecoder.cpp
/// \brief original uw model decoding
//
#include "pch.hpp"
#include "Model3D.hpp"
#include "Model3DBuiltIn.hpp"
#include "Renderer.hpp"
#include "File.hpp"
#include "Math.hpp"
#include <vector>
#include "PolygonTessellator.hpp"

#pragma warning(disable: 4063) // case 'X' is not a valid value for switch of enum 'ModelNodeCommand'
#pragma warning(disable: 4456) // declaration of 'vertno' hides previous local declaration
#pragma warning(disable: 4701) // potentially uninitialized local variable 'nx' used

/// \brief underworld models executable positions
/// Underworld models are (unfortunately) buried deep inside the executable
/// table was taken from TSSHP, at
/// http://madeira.physiol.ucl.ac.uk/tsshp/sshock.html
struct
{
   Uint32 table_offset;
   Uint32 value;          ///< 4 bytes at table_offset
   Uint32 base_offset;
} g_model_offset_table[] =
{
   { 0x0004e910, 0x40064ab6, 0x0004e99e },
   { 0x0004ccd0, 0x40064ab6, 0x0004cd5e }, // same models, different place
   { 0x0004e370, 0x40064ab6, 0x0004e3fe }, // ditto (reported Gerd Bitzer)
   { 0x0004ec70, 0x40064ab6, 0x0004ecfe }, // uw_demo models
   { 0x00054cf0, 0x59aa64d4, 0x00054d8a }, // UW2
   { 0x000550e0, 0x59aa64d4, 0x0005517a }, // another UW2 build
};

static std::map<Uint16, Uint8> s_colorOffsetToPaletteIndexMapping =
{
   // uw1
   { 0x2920, 0x63 }, // gray
   { 0x2922, 0x30 }, // blue
   { 0x2924, 0x50 }, // yellow
   { 0x2926, 0 }, // black/transparent
   { 0x292a, 0 },
   { 0x292c, 0 }, // only used in 00be node in model 1 door frame
   { 0x292e, 0 }, // only used in 00be node in model 1 door frame
   { 0x2936, 0 }, // only used in bridge

   // uw2
   { 0x2680, 0x63 },
   { 0x2682, 0x30 },
   { 0x2684, 0x50 },
   { 0x2686, 0 },
   { 0x268a, 0 },
   { 0x268c, 0 }, // only used in 00be node in model 1 door frame
   { 0x268e, 0 }, // only used in 00be node in model 1 door frame
   { 0x2696, 0 }, // only used in bridge

   // blackrock gem colors
   { 0x2698, 0 },
   { 0x269a, 0 },
   { 0x269c, 0 },
   { 0x269e, 0 },
   { 0x26a0, 0 },
};

#define UaModelTrace if (dump) printf

/// model name tables
const char* c_modelNames[32] =
{
   "-",
   "door frame",
   "bridge",
   "bench",
   "Lotus Turbo Esprit (no, really!)",
   "small boulder",
   "medium boulder",
   "large boulder",
   "arrow",
   "beam",
   "pillar",
   "shrine",
   "?",
   "painting [uw2]",
   "?",
   "?",
   "texture map (8-way lever)",
   "texture map (8-way switch)",
   "texture map (writing)",
   "gravestone",
   "texture map (0x016e)",
   "-",
   "texture map (0x016f)",
   "moongate",
   "table",
   "chest",
   "nightstand",
   "barrel",
   "chair",
   "bed [uw2]",
   "blackrock gem [uw2]",
   "shelf [uw2]"
};

/// model node commands
enum ModelNodeCommand
{
   M3_UW_ENDNODE = 0x0000,
   M3_UW_SORT_PLANE = 0x0006,
   M3_UW_SORT_PLANE_ZY = 0x000C,
   M3_UW_SORT_PLANE_XY = 0x000E,
   M3_UW_SORT_PLANE_XZ = 0x0010,
   M3_UW_UNKNOWN_12 = 0x0012,
   M3_UW_COLOR_DEF = 0x0014,
   M3_UW_FACE_UNK16 = 0x0016,
   M3_UW_FACE_UNK2E = 0x002E, // only uw2
   M3_UW_FACE_UNK40 = 0x0040,
   M3_UW_FACE_UNK44 = 0x0044, // only uw2
   M3_UW_TRANSLATE = 0x004a,
   M3_UW_FACE_PLANE = 0x0058,
   M3_UW_FACE_PLANE_ZY = 0x005E,
   M3_UW_FACE_PLANE_XY = 0x0060,
   M3_UW_FACE_PLANE_XZ = 0x0062,
   M3_UW_FACE_PLANE_X = 0x0064,
   M3_UW_FACE_PLANE_Z = 0x0066,
   M3_UW_FACE_PLANE_Y = 0x0068,
   M3_UW_ORIGIN = 0x0078,
   M3_UW_VERTEX = 0x007A,
   M3_UW_FACE_VERTICES = 0x007E,
   M3_UW_VERTICES = 0x0082,
   M3_UW_VERTEX_X = 0x0086,
   M3_UW_VERTEX_Z = 0x0088,
   M3_UW_VERTEX_Y = 0x008A,
   M3_UW_VERTEX_CEIL = 0x008C,
   M3_UW_VERTEX_XZ = 0x0090,
   M3_UW_VERTEX_XY = 0x0092,
   M3_UW_VERTEX_YZ = 0x0094,
   M3_UW_FACE_SHORT = 0x00A0,
   M3_UW_TEXTURE_FACE = 0x00A8,
   M3_UW_UNKNOWN_B2 = 0x00B2, // uw2 only
   M3_UW_TMAP_VERTICES = 0x00B4,
   M3_UW_SUBMODEL = 0x00BA,
   M3_UW_FACE_SHADE = 0x00BC,
   M3_UW_FACE_TWOSHADES = 0x00BE,
   M3_UW_TMAP_VERTICES2 = 0x00CE,
   M3_UW_FACE_SHORT2 = 0x00D2, // uw2 only
   M3_UW_VERTEX_DARK = 0x00D4,
   M3_UW_FACE_GOURAUD = 0x00D6,
};

// shorthand texture coordinates for M3_UW_FACE_SHORT and M3_UW_FACE_SHORT2
// nodes
static const double shorthand_texcoords[8] =
{
   0.0, 0.0,
   1.0, 0.0,
   1.0, 1.0,
   0.0, 1.0,
};

/// reads a 16-bit int as 8.8 fixed-point double value
double ModelReadFixed(Base::File& file)
{
   Sint16 val = static_cast<Sint16>(file.Read16());
   return val / 256.0;
}

/// reads a vertex number, ignoring the first 3 bits
Uint16 ModelReadVertexNumber(Base::File& file)
{
   Uint16 val = file.Read16();
   return val >> 3;
}

/// reads a 16-bit int as 0.16 fixed-point double texture coordinate
double ModelReadTextureCoord(Base::File& file)
{
   Uint16 val = static_cast<Uint16>(file.Read16());
   return val / 65535.0;
}

/// Takes a color offset that points into the uw's executable's segment at the
/// position where color indices are stored, maps it to the palette index and
/// returns it
Uint8 MapColorOffsetToPaletteIndex(Uint16 colorOffset)
{
   UaAssertMsg(
      s_colorOffsetToPaletteIndexMapping.find(colorOffset) != s_colorOffsetToPaletteIndexMapping.end(),
      "color offset must be mapped to a palette index");

   return s_colorOffsetToPaletteIndexMapping[colorOffset];
}

/// stores a vertex in the vertex list, resizing it when necessary
void ModelStoreVertex(const Vector3d& vertex, Uint16 vertno,
   std::vector<Vector3d>& vertex_list)
{
   if (vertex_list.size() <= vertno)
      vertex_list.resize(vertno + 1);

   vertex_list[vertno] = vertex;
}

void ModelParseNode(Base::File& file, Vector3d& origin,
   std::vector<Vector3d>& vertex_list,
   std::vector<Triangle3dTextured>& triangles,
   Uint8 palIndex,
   bool dump)
{
   // parse node until end node
   bool loop = true;

   Uint16 textureNumber = 0;
   bool flatShaded = false;

   while (loop)
   {
      // read next command
      ModelNodeCommand cmd = (ModelNodeCommand)file.Read16();
      Uint16 refvert, vertno, unk1;

      Uint16 colorOffset;

      double vx, vy, vz;
      double nx, ny, nz;
      Vector3d refvect;

      UaModelTrace(" %04x ", cmd);
      switch (cmd)
      {
         // misc. nodes
      case M3_UW_ENDNODE: // 0000 end node
         UaModelTrace("[end]");
         loop = false;
         break;

      case M3_UW_ORIGIN: // 0078 define model center
         vertno = ModelReadVertexNumber(file);
         origin = vertex_list[vertno];

         vx = ModelReadFixed(file);
         vy = ModelReadFixed(file);
         vz = ModelReadFixed(file);

         unk1 = file.Read16(); // always 0
         UaModelTrace("[origin] vertno=%u unk1=%04x origin=(%f,%f,%f)",
            vertno, unk1, vx, vy, vz);
         break;

      case M3_UW_TRANSLATE: // 004a translate
         vx = ModelReadFixed(file);
         vz = ModelReadFixed(file); // z and y swapped
         vy = ModelReadFixed(file);
         UaModelTrace("[translate] delta=(%f,%f,%f)",
            vx, vy, vz);
         break;

      case M3_UW_SUBMODEL: // 00ba submodel
      {
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);

         UaModelTrace("[submodel] start, color=%04x (%02x) ", colorOffset, palIndex);
         unk1 = file.Read16();

         long here = file.Tell();
         long offset = here - ((-unk1) & 0xffff);
         UaModelTrace(" reloffset=%04x, submodel at 0x%08x\n", unk1, (Uint32)offset);

         // parse submodel nodes
         file.Seek(offset, Base::seekBegin);
         ModelParseNode(file, origin, vertex_list, triangles, palIndex, dump);

         // return to "here"
         file.Seek(here, Base::seekBegin);

         UaModelTrace("      [submodel] end");
      }
      break;

      // vertex definition nodes
      case M3_UW_VERTEX: // 007a define initial vertex
         vx = ModelReadFixed(file);
         vy = ModelReadFixed(file);
         vz = ModelReadFixed(file);
         vertno = ModelReadVertexNumber(file);

         refvect = Vector3d(vx, vy, vz);
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f)",
            vertno, vx, vy, vz);
         break;

      case M3_UW_VERTICES: // 0082 define initial vertices
      {
         Uint16 nvert = file.Read16();
         vertno = file.Read16();
         for (unsigned int n = 0; n < nvert; n++)
         {
            vx = ModelReadFixed(file);
            vy = ModelReadFixed(file);
            vz = ModelReadFixed(file);

            refvect = Vector3d(vx, vy, vz);
            ModelStoreVertex(refvect, Uint16(vertno + n), vertex_list);

            UaModelTrace("%s[vertex] vertno=%u vertex=(%f,%f,%f)",
               n == 0 ? "" : "\n      ", vertno + n, vx, vy, vz);
         }
      }
      break;

      case M3_UW_VERTEX_X: // 0086 define vertex offset X
         refvert = ModelReadVertexNumber(file);
         vx = ModelReadFixed(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.x += vx;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-x from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_Z: // 0088 define vertex offset Z
         refvert = ModelReadVertexNumber(file);
         vz = ModelReadFixed(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.z += vz;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-z from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_Y: // 008a define vertex offset Y
         refvert = ModelReadVertexNumber(file);
         vy = ModelReadFixed(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.y += vy;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-y from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_XZ: // 0090 define vertex offset X,Z
         vx = ModelReadFixed(file);
         vz = ModelReadFixed(file);
         refvert = ModelReadVertexNumber(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.z += vz;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-xz from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_XY: // 0092 define vertex offset X,Y
         vx = ModelReadFixed(file);
         vy = ModelReadFixed(file);
         refvert = ModelReadVertexNumber(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.y += vy;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-xy from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_YZ: // 0094 define vertex offset Y,Z
         vy = ModelReadFixed(file);
         vz = ModelReadFixed(file);
         refvert = ModelReadVertexNumber(file);
         vertno = ModelReadVertexNumber(file);

         UaAssertMsg(refvert < vertex_list.size(), "vertex list index must be valid");

         refvect = vertex_list[refvert];
         refvect.y += vy;
         refvect.z += vz;
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,%f) offset-yz from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_CEIL: // 008c define vertex variable height
         refvert = ModelReadVertexNumber(file);
         unk1 = file.Read16();
         vertno = ModelReadVertexNumber(file);

         refvect = vertex_list[refvert];
         refvect.z = 32.0; // use fixed value for ceiling height
         ModelStoreVertex(refvect, vertno, vertex_list);

         UaModelTrace("[vertex] vertno=%u vertex=(%f,%f,ceil) to-ceil from=%u unk1=%04x",
            vertno, refvect.x, refvect.y, refvert, unk1);
         break;

         // face plane checks
      case M3_UW_FACE_PLANE: // 0058 define face plane, arbitrary heading
         unk1 = file.Read16();
         nx = ModelReadFixed(file);
         vx = ModelReadFixed(file);
         ny = ModelReadFixed(file);
         vy = ModelReadFixed(file);
         nz = ModelReadFixed(file);
         vz = ModelReadFixed(file);

         UaModelTrace("[planecheck] skip=%04x normal=(%f,%f,%f) dist=(%f,%f,%f)",
            unk1, nx, ny, nz, vx, vy, vz);
         break;

      case M3_UW_FACE_PLANE_X: // 0064 define face plane X
      case M3_UW_FACE_PLANE_Z: // 0066 define face plane Z
      case M3_UW_FACE_PLANE_Y: // 0068 define face plane Y
         unk1 = file.Read16();
         nx = ModelReadFixed(file);
         vx = ModelReadFixed(file);

         UaModelTrace("[planecheck] skip=%04x normal=(%f,%f,%f) dist=(%f,%f,%f) %c",
            unk1,
            cmd == M3_UW_FACE_PLANE_X ? nx : 0.0,
            cmd == M3_UW_FACE_PLANE_Y ? nx : 0.0,
            cmd == M3_UW_FACE_PLANE_Z ? nx : 0.0,
            cmd == M3_UW_FACE_PLANE_X ? vx : 0.0,
            cmd == M3_UW_FACE_PLANE_Y ? vx : 0.0,
            cmd == M3_UW_FACE_PLANE_Z ? vx : 0.0,

            cmd == M3_UW_FACE_PLANE_X ? 'x' : cmd == M3_UW_FACE_PLANE_Y ? 'y' : 'z'
         );
         break;

      case M3_UW_FACE_PLANE_ZY: // 005e define face plane Z/Y
      case M3_UW_FACE_PLANE_XY: // 0060 define face plane X/Y
      case M3_UW_FACE_PLANE_XZ: // 0062 define face plane X/Z
         unk1 = file.Read16();
         nx = ModelReadFixed(file);
         vx = ModelReadFixed(file);
         ny = ModelReadFixed(file);
         vy = ModelReadFixed(file);

         UaModelTrace("[planecheck] skip=%04x ", unk1);
         if (dump)
            switch (cmd)
            {
            case M3_UW_FACE_PLANE_ZY:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f) zy", 0.0, ny, nx, 0.0, vy, vx);
               break;
            case M3_UW_FACE_PLANE_XY:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f) xy", nx, ny, 0.0, vx, vy, 0.0);
               break;
            case M3_UW_FACE_PLANE_XZ:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f) xz", nx, 0.0, ny, vx, 0.0, vy);
               break;
            }
         break;

         // face definition nodes
      case M3_UW_FACE_VERTICES: // 007e define face vertices
      {
         Uint16 nvert = file.Read16();
         PolygonTessellator tess;

         UaModelTrace("[face] nvert=%u vertlist=", nvert);

         for (Uint16 i = 0; i < nvert; i++)
         {
            Uint16 vertno = ModelReadVertexNumber(file);

            Vertex3d vert;
            vert.pos = vertex_list[vertno];
            tess.AddPolygonVertex(vert);

            UaModelTrace("%u", vertno);
            if (i < nvert - 1) UaModelTrace(" ");
         }

         const std::vector<Triangle3dTextured>& tri = tess.Tessellate(textureNumber, palIndex, flatShaded);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      case M3_UW_TEXTURE_FACE: // 00a8 define texture-mapped face
      case M3_UW_TMAP_VERTICES: // 00b4 define face vertices with u,v information
      case M3_UW_TMAP_VERTICES2: // 00ce same as 00b4
      {
         UaModelTrace("[face] %s ", cmd == M3_UW_TEXTURE_FACE ? "tex" : "tmap");

         // read texture number
         if (cmd == M3_UW_TEXTURE_FACE)
         {
            unk1 = file.Read16();
            UaModelTrace("texnum=%04x ", unk1);
         }
         else
            unk1 = 0;

         Uint16 nvert = file.Read16();
         PolygonTessellator tess;

         UaModelTrace("nvert=%u vertlist=", nvert);

         for (Uint16 i = 0; i < nvert; i++)
         {
            Uint16 vertno = ModelReadVertexNumber(file);

            double u0 = ModelReadTextureCoord(file);
            double v0 = ModelReadTextureCoord(file);

            Vertex3d vert;
            vert.pos = vertex_list[vertno];
            vert.u = u0;
            vert.v = v0;
            tess.AddPolygonVertex(vert);

            UaModelTrace("%u (%f/%f)", vertno, u0, v0);
            if (i < nvert - 1) UaModelTrace(" ");
         }

         const std::vector<Triangle3dTextured>& tri = tess.Tessellate(textureNumber, palIndex, flatShaded);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      case M3_UW_FACE_SHORT: // 00A0 shorthand face definition
      case M3_UW_FACE_SHORT2: // 00D2 shorthand face definition [uw2]
      {
         UaModelTrace("[face] shorthand ");

         if (cmd == M3_UW_FACE_SHORT)
         {
            unk1 = file.Read16();
            UaModelTrace("texnum=%04x ", unk1);
         }

         PolygonTessellator tess;

         UaModelTrace("vertlist=");

         for (Uint16 i = 0; i < 4; i++)
         {
            Uint8 vertno = file.Read8();

            Vertex3d vert;
            vert.pos = vertex_list[vertno];
            vert.u = shorthand_texcoords[i * 2];
            vert.v = shorthand_texcoords[i * 2 + 1];
            tess.AddPolygonVertex(vert);

            UaModelTrace("%u", vertno);
            if (i < 3) UaModelTrace(" ");
         }

         const std::vector<Triangle3dTextured>& tri = tess.Tessellate(textureNumber, palIndex, flatShaded);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      // sort nodes
      case M3_UW_SORT_PLANE: // 0006 define sort node, arbitrary heading
         nx = ModelReadFixed(file);
         vx = ModelReadFixed(file);
         // fall-through

      case M3_UW_SORT_PLANE_ZY: // 000C define sort node, ZY plane
      case M3_UW_SORT_PLANE_XY: // 000E define sort node, XY plane
      case M3_UW_SORT_PLANE_XZ: // 0010 define sort node, XZ plane
      {
         ny = ModelReadFixed(file);
         vy = ModelReadFixed(file);
         nz = ModelReadFixed(file);
         vz = ModelReadFixed(file);

         if (dump)
         {
            UaModelTrace("[sort] ");
            switch (cmd)
            {
            case M3_UW_SORT_PLANE:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f)", nx, ny, nz, vx, vy, vz);
               break;
            case M3_UW_SORT_PLANE_ZY:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f)", 0.0, nz, ny, 0.0, vz, vy);
               break;
            case M3_UW_SORT_PLANE_XY:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f)", ny, nz, 0.0, vy, vz, 0.0);
               break;
            case M3_UW_SORT_PLANE_XZ:
               UaModelTrace("normal=(%f,%f,%f) dist=(%f,%f,%f)", ny, 0.0, nz, vy, 0.0, vz);
               break;
            }
         }

         Uint32 left = file.Read16();
         left += file.Tell();

         Uint32 right = file.Read16();
         right += file.Tell();

         UaModelTrace(" left=%08x right=%08x\n      [sort] start left node\n", left, right);

         long here = file.Tell();

         // parse left nodes
         file.Seek(left, Base::seekBegin);
         ModelParseNode(file, origin, vertex_list, triangles, palIndex, dump);

         UaModelTrace("      [sort] end left node/start right node\n");

         // parse right nodes
         file.Seek(right, Base::seekBegin);
         ModelParseNode(file, origin, vertex_list, triangles, palIndex, dump);

         // return to "here"
         file.Seek(here, Base::seekBegin);

         UaModelTrace("      [sort] end");
      }
      break;

      // shading/colour nodes
      case M3_UW_COLOR_DEF: // 0014 colour definition
         refvert = ModelReadVertexNumber(file);
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);
         vertno = ModelReadVertexNumber(file);

         UaModelTrace("[shade] color refvert=%u color=%04x (%02x) vertno=%u", refvert, colorOffset, palIndex, vertno);
         break;

      case M3_UW_FACE_SHADE: // 00BC define face shade
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);
         unk1 = file.Read16();

         UaModelTrace("[shade] face color=%04x (%02x) unk1=%02x", colorOffset, palIndex, unk1);
         break;

      case M3_UW_FACE_TWOSHADES: // 00BE ??? seems to define 2 colors
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);
         UaModelTrace("[shade] twoshades color1=%04x (%02x) ", colorOffset, palIndex);

         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);
         UaModelTrace("color2=%04x (%02x) ", colorOffset, palIndex);
         break;

      case M3_UW_VERTEX_DARK: // 00D4 define vertex gouraud shading values
      {
         Uint16 nvert = file.Read16();
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);

         UaModelTrace("[shade] gouraud nvert=%u, color=%04x (%02x) vertlist=",
            nvert, colorOffset, palIndex);

         for (Uint16 n = 0; n < nvert; n++)
         {
            vertno = ModelReadVertexNumber(file);
            unk1 = file.Read8();

            UaModelTrace("%u (%02x)", vertno, unk1);
            if (n < nvert - 1) UaModelTrace(" ");
         }

         if (nvert & 1)
            file.Read8(); // read alignment padding
      }
      break;

      case M3_UW_FACE_GOURAUD: // 00D6 define gouraud shading
         UaModelTrace("[shade] gouraud on");
         break;

      case M3_UW_FACE_UNK40: // 0040 ??? flat
         UaModelTrace("[shade] unknown");
         break;

         // unknown nodes
      case M3_UW_FACE_UNK16: // 0016 ??? unknown
         vertno = ModelReadVertexNumber(file);
         colorOffset = file.Read16();
         palIndex = MapColorOffsetToPaletteIndex(colorOffset);
         unk1 = file.Read16();

         UaModelTrace("[unkn] 0016 vertno=%02x color=%04x (%02x) unk1=%04x", vertno, colorOffset, palIndex, unk1);
         break;

      case M3_UW_UNKNOWN_12: // 0012 ??? unknown
         unk1 = file.Read16();
         UaModelTrace("[unkn] 0012 unk1=%04x", unk1);
         break;

      case M3_UW_FACE_UNK2E: // 002e ??? unknown [uw2]
         unk1 = file.Read16();
         UaModelTrace("[unkn] 002e unk1=%02x", unk1);
         break;

      case M3_UW_FACE_UNK44: // 0044 ??? unknown [uw2]
         UaModelTrace("[unkn] 0044");
         break;

      case M3_UW_UNKNOWN_B2: // 00b2 ??? unknown [uw2]
         unk1 = file.Read16();
         UaModelTrace("[unkn] 00b2 unk1=%02x", unk1);
         break;

      default:
         UaModelTrace("unknown command at offset 0x%08lx\n", file.Tell() - 2);
         return;
      }
      UaModelTrace("\n");
   }
}

bool DecodeBuiltInModels(const char* filename,
   std::vector<Model3DPtr>& allModels, bool dump, bool isUw2)
{
   Base::File file(filename, Base::modeRead);
   if (!file.IsOpen())
      return false;

   Uint32 base = 0; // models list base address

   // search all offsets for model table begin
   unsigned int max = SDL_TABLESIZE(g_model_offset_table);
   for (unsigned int i = 0; i < max; i++)
   {
      // check value on file position
      file.Seek(g_model_offset_table[i].table_offset, Base::seekBegin);
      Uint32 file_value = file.Read32();

      if (file_value == g_model_offset_table[i].value)
      {
         // found position
         base = g_model_offset_table[i].base_offset;
         file.Seek(g_model_offset_table[i].table_offset, Base::seekBegin);

         UaTrace("found models in %s at 0x%08x\n", filename, base);
         break;
      }
   }

   if (base == 0)
   {
      UaTrace("didn't find models in file %s!\n", filename);
      return false; // didn't find list
   }

   // read in offsets
   Uint16 offsets[32];

   for (unsigned int j = 0; j < 32; j++)
      offsets[j] = file.Read16();

   // parse all models
   for (unsigned int n = 0; n < 32; n++)
   {
      // seek to model
      file.Seek(base + offsets[n], Base::seekBegin);

      // read header
      Uint32 unk1 = file.Read32();

      // extents
      double ex = ModelReadFixed(file);
      double ey = ModelReadFixed(file);
      double ez = ModelReadFixed(file);

      Vector3d extents(ex, ez, ey);

      UaModelTrace("dumping builtin model %u (%s)\nreloffset=%04x offset=0x%08x [unk1=0x%04x, extents=(%f,%f,%f)]\n",
         n, c_modelNames[n], offsets[n], base + offsets[n], unk1, ex, ey, ez);

      Model3DBuiltIn* model = new Model3DBuiltIn;

      // temporary variables
      Vector3d origin;
      std::vector<Vector3d> vertex_list;
      Uint8 palIndex = 0;

      // parse root node
      ModelParseNode(file, origin, vertex_list, model->m_triangles, palIndex, dump);

      UaModelTrace("\n");

      origin.z -= extents.z / 2.0;
      model->m_extents = extents;

      allModels.push_back(Model3DPtr{ model });
   }

   return true;
}
