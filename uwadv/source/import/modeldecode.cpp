//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file modeldecode.cpp
/// \brief original uw model decoding
//
#include "import.hpp"
#include "models.hpp"
#include "model_builtin.hpp"
#include "renderer.hpp"
#include "file.hpp"
#include "uamath.hpp"
#include <vector>
#include "tessellator.hpp"

#pragma warning(disable: 4063) // case 'X' is not a valid value for switch of enum 'ua_model_nodecmd'
#pragma warning(disable: 4456) // declaration of 'vertno' hides previous local declaration
#pragma warning(disable: 4701) // potentially uninitialized local variable 'nx' used

/// \brief underworld models executable positions
/// Underworld models are (unfortunately) buried deep inside the executable
/// table was taken from TSSHP, at
/// http://madeira.physiol.ucl.ac.uk/tsshp/sshock.html
struct
{
   Uint32 table_offset;
   Uint32 value;          /* 4 bytes at table_offset */
   Uint32 base_offset;
} ua_model_offset_table[] =
{
   { 0x0004e910, 0x40064ab6, 0x0004e99e },
   { 0x0004ccd0, 0x40064ab6, 0x0004cd5e }, /* same models, different place */
   { 0x0004e370, 0x40064ab6, 0x0004e3fe }, /* ditto (reported Gerd Bitzer) */
   { 0x0004ec70, 0x40064ab6, 0x0004ecfe }, /* uw_demo models */
   { 0x00054cf0, 0x59aa64d4, 0x00054d8a }, /* UW2 */
   { 0x000550e0, 0x59aa64d4, 0x0005517a }, /* another UW2 build            */
};

#define ua_mdl_trace if (dump) printf

/// model name tables
const char* ua_model_name[32] =
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
   "?texture map (0x016f)",
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
enum ua_model_nodecmd
{
   M3_UW_ENDNODE = 0x0000,
   M3_UW_SORT_PLANE = 0x0006,
   M3_UW_SORT_PLANE_ZY = 0x000C,
   M3_UW_SORT_PLANE_XY = 0x000E,
   M3_UW_SORT_PLANE_XZ = 0x0010,
   M3_UW_COLOR_DEF = 0x0014, //???
   M3_UW_FACE_UNK16 = 0x0016, //???
   M3_UW_FACE_UNK40 = 0x0040,
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
   M3_UW_TMAP_VERTICES = 0x00B4,
   M3_UW_FACE_SHADE = 0x00BC,
   M3_UW_FACE_TWOSHADES = 0x00BE,
   M3_UW_VERTEX_DARK = 0x00D4,
   M3_UW_FACE_GOURAUD = 0x00D6,
};

double ua_mdl_read_fixed(Base::File& file)
{
   Sint16 val = static_cast<Sint16>(file.Read16());
   return val / 256.0;
}

Uint16 ua_mdl_read_vertno(Base::File& file)
{
   Uint16 val = file.Read16();
   return val / 8;
}

void ua_mdl_store_vertex(const ua_vector3d& vertex, Uint16 vertno,
   std::vector<ua_vector3d>& vertex_list)
{
   if (vertex_list.size() <= vertno)
      vertex_list.resize(vertno + 1);

   vertex_list[vertno] = vertex;
}

void ua_model_parse_node(Base::File& file, ua_vector3d& origin,
   std::vector<ua_vector3d>vertex_list,
   std::vector<ua_triangle3d_textured>& triangles,
   bool dump)
{
   // parse node until end node
   bool loop = true;

   while (loop)
   {
      // read next command
      ua_model_nodecmd cmd = (ua_model_nodecmd)file.Read16();
      Uint16 refvert, vertno, unk1;
      double vx, vy, vz;
      double nx, ny, nz;
      ua_vector3d refvect;

      ua_mdl_trace(" %04x ", cmd);
      switch (cmd)
      {
         // misc. nodes
      case M3_UW_ENDNODE: // 0000 end node
         ua_mdl_trace("[end]");
         loop = false;
         break;

      case M3_UW_ORIGIN: // 0078 define model center
         vertno = ua_mdl_read_vertno(file);
         origin = vertex_list[vertno];

         vx = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);

         unk1 = file.Read16();
         ua_mdl_trace("[origin] vertno=%u unk1=%04x origin=(%f,%f,%f)",
            vertno, unk1, vx, vy, vz);
         break;

         // vertex definition nodes
      case M3_UW_VERTEX: // 007a define initial vertex
         vx = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = ua_vector3d(vx, vy, vz);
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f)",
            vertno, vx, vy, vz);
         break;

      case M3_UW_VERTICES: // 0082 define initial vertices
      {
         Uint16 nvert = file.Read16();
         vertno = file.Read16();
         for (unsigned int n = 0; n < nvert; n++)
         {
            vx = ua_mdl_read_fixed(file);
            vy = ua_mdl_read_fixed(file);
            vz = ua_mdl_read_fixed(file);

            refvect = ua_vector3d(vx, vy, vz);
            ua_mdl_store_vertex(refvect, Uint16(vertno + n), vertex_list);

            ua_mdl_trace("%s[vertex] vertno=%u vertex=(%f,%f,%f)",
               n == 0 ? "" : "\n      ", vertno + n, vx, vy, vz);
         }
      }
      break;

      case M3_UW_VERTEX_X: // 0086 define vertex offset X
         refvert = ua_mdl_read_vertno(file);
         vx = ua_mdl_read_fixed(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) x from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_Z: // 0088 define vertex offset Z
         refvert = ua_mdl_read_vertno(file);
         vz = ua_mdl_read_fixed(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.z += vz;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) z from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_Y: // 008a define vertex offset Y
         refvert = ua_mdl_read_vertno(file);
         vy = ua_mdl_read_fixed(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.y += vy;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) y from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_XZ: // 0090 define vertex offset X,Z
         vx = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);
         refvert = ua_mdl_read_vertno(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.z += vz;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) xz from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_XY: // 0092 define vertex offset X,Y
         vx = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);
         refvert = ua_mdl_read_vertno(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.y += vy;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) xy from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_YZ: // 0094 define vertex offset Y,Z
         vy = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);
         refvert = ua_mdl_read_vertno(file);
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.y += vy;
         refvect.z += vz;
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,%f) yz from=%u",
            vertno, refvect.x, refvect.y, refvect.z, refvert);
         break;

      case M3_UW_VERTEX_CEIL: // 008c define vertex variable height
         refvert = ua_mdl_read_vertno(file);
         unk1 = file.Read16();
         vertno = ua_mdl_read_vertno(file);

         refvect = vertex_list[refvert];
         refvect.z = 32.0; // todo: ceiling value
         ua_mdl_store_vertex(refvect, vertno, vertex_list);

         ua_mdl_trace("[vertex] vertno=%u vertex=(%f,%f,ceil) ceil from=%u unk1=%04x",
            vertno, refvect.x, refvect.y, refvert, unk1);
         break;

         // face plane checks
      case M3_UW_FACE_PLANE: // 0058 define face plane, arbitrary heading
         unk1 = file.Read16();
         nx = ua_mdl_read_fixed(file);
         vx = ua_mdl_read_fixed(file);
         ny = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);
         nz = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);

         ua_mdl_trace("[planecheck] skip=%04x normal=(%f,%f,%f) dist=(%f,%f,%f)",
            unk1, nx, ny, nz, vx, vy, vz);
         break;

      case M3_UW_FACE_PLANE_X: // 0064 define face plane X
      case M3_UW_FACE_PLANE_Z: // 0066 define face plane Z
      case M3_UW_FACE_PLANE_Y: // 0068 define face plane Y
         unk1 = file.Read16();
         nx = ua_mdl_read_fixed(file);
         vx = ua_mdl_read_fixed(file);

         ua_mdl_trace("[planecheck] skip=%04x normal=(%f,%f,%f) dist=(%f,%f,%f) %c",
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
         nx = ua_mdl_read_fixed(file);
         vx = ua_mdl_read_fixed(file);
         ny = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);

         ua_mdl_trace("[planecheck] skip=%04x ", unk1);
         if (dump)
            switch (cmd)
            {
            case M3_UW_FACE_PLANE_ZY:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f) zy", 0.0, ny, nx, 0.0, vy, vx);
               break;
            case M3_UW_FACE_PLANE_XY:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f) xy", nx, ny, 0.0, vx, vy, 0.0);
               break;
            case M3_UW_FACE_PLANE_XZ:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f) xz", nx, 0.0, ny, vx, 0.0, vy);
               break;
            }
         break;

         // face info nodes
      case M3_UW_FACE_VERTICES: // 007e define face vertices
      {
         Uint16 nvert = file.Read16();
         ua_poly_tessellator tess;

         ua_mdl_trace("[face] nvert=%u vertlist=", nvert);

         for (Uint16 i = 0; i < nvert; i++)
         {
            Uint16 vertno = ua_mdl_read_vertno(file);

            ua_vertex3d vert;
            vert.pos = vertex_list[vertno];
            tess.add_poly_vertex(vert);

            ua_mdl_trace("%u", vertno);
            if (i <= nvert - 1) ua_mdl_trace(" ");
         }

         const std::vector<ua_triangle3d_textured>& tri = tess.tessellate(0x0001);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      case M3_UW_TEXTURE_FACE: // 00a8 define texture-mapped face
      case M3_UW_TMAP_VERTICES: // 00b4 define face vertices with u,v information
      {
         ua_mdl_trace("[face] %s ", cmd == M3_UW_TEXTURE_FACE ? "tex" : "tmap");

         // read texture number
         if (cmd == M3_UW_TEXTURE_FACE)
         {
            unk1 = file.Read16(); // texture number?
            ua_mdl_trace("texnum=%04x ", unk1);
         }

         Uint16 nvert = file.Read16();
         ua_poly_tessellator tess;

         ua_mdl_trace("nvert=%u vertlist=", nvert);

         for (Uint16 i = 0; i < nvert; i++)
         {
            Uint16 vertno = ua_mdl_read_vertno(file);

            double u0 = ua_mdl_read_fixed(file);
            double v0 = ua_mdl_read_fixed(file);

            ua_vertex3d vert;
            vert.pos = vertex_list[vertno];
            vert.u = u0;
            vert.v = v0;
            tess.add_poly_vertex(vert);

            ua_mdl_trace("%u (%f/%f)", vertno, u0, v0);
            if (i <= nvert - 1) ua_mdl_trace(" ");
         }

         const std::vector<ua_triangle3d_textured>& tri = tess.tessellate(0x0002);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      // sort nodes
      case M3_UW_SORT_PLANE: // 0006 define sort node, arbitrary heading
         nx = ua_mdl_read_fixed(file);
         vx = ua_mdl_read_fixed(file);
         // fall-through

      case M3_UW_SORT_PLANE_ZY: // 000C define sort node, ZY plane
      case M3_UW_SORT_PLANE_XY: // 000E define sort node, XY plane
      case M3_UW_SORT_PLANE_XZ: // 0010 define sort node, XZ plane
      {
         ny = ua_mdl_read_fixed(file);
         vy = ua_mdl_read_fixed(file);
         nz = ua_mdl_read_fixed(file);
         vz = ua_mdl_read_fixed(file);

         if (dump)
         {
            ua_mdl_trace("[sort] ");
            switch (cmd)
            {
            case M3_UW_SORT_PLANE:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f)", nx, ny, nz, vx, vy, vz);
               break;
            case M3_UW_SORT_PLANE_ZY:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f)", 0.0, nz, ny, 0.0, vz, vy);
               break;
            case M3_UW_SORT_PLANE_XY:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f)", ny, nz, 0.0, vy, vz, 0.0);
               break;
            case M3_UW_SORT_PLANE_XZ:
               ua_mdl_trace("normal=(%f,%f,%f) dist=(%f,%f,%f)", ny, 0.0, nz, vy, 0.0, vz);
               break;
            }
         }

         Uint32 left = file.Read16();
         left += file.Tell();

         Uint32 right = file.Read16();
         right += file.Tell();

         ua_mdl_trace(" left=%08x right=%08x\n      [sort] start left node\n", left, right);

         long here = file.Tell();

         // parse left nodes
         file.Seek(left, Base::seekBegin);
         ua_model_parse_node(file, origin, vertex_list, triangles, dump);

         ua_mdl_trace("      [sort] end left node/start right node\n");

         // parse right nodes
         file.Seek(right, Base::seekBegin);
         ua_model_parse_node(file, origin, vertex_list, triangles, dump);

         // return to "here"
         file.Seek(here, Base::seekBegin);

         ua_mdl_trace("      [sort] end");
      }
      break;

      // unknown nodes
      case M3_UW_COLOR_DEF: // 0014 ??? colour definition
         refvert = ua_mdl_read_vertno(file);
         unk1 = file.Read16();
         vertno = ua_mdl_read_vertno(file);
         ua_mdl_trace("[shade] color refvert=%u unk1=%04x vertno=%u", refvert, unk1, vertno);
         break;

      case M3_UW_FACE_SHADE: // 00BC define face shade
         unk1 = file.Read16();
         vertno = file.Read16();
         ua_mdl_trace("[shade] shade unk1=%02x unk2=%02x", unk1, vertno);
         break;

      case M3_UW_FACE_TWOSHADES: // 00BE ??? seems to define 2 shades
         vertno = file.Read16();
         unk1 = file.Read16();
         ua_mdl_trace("[shade] twoshade unk1=%02x unk2=%02x ", vertno, unk1);
         break;

      case M3_UW_VERTEX_DARK: // 00D4 define dark vertex face (?)
      {
         Uint16 nvert = file.Read16();
         unk1 = file.Read16();

         ua_mdl_trace("[shade] color nvert=%u, unk1=%04x vertlist=",
            nvert, unk1);

         for (Uint16 n = 0; n < nvert; n++)
         {
            vertno = ua_mdl_read_vertno(file);
            unk1 = file.Read8();

            ua_mdl_trace("%u (%02x) ", vertno, unk1);
         }

         if (nvert & 1)
            file.Read8(); // read alignment padding
      }
      break;

      case M3_UW_FACE_GOURAUD: // 00D6 define gouraud shading
         ua_mdl_trace("[shade] gouraud");
         break;

      case M3_UW_FACE_UNK40: // 0040 ???
         ua_mdl_trace("[shade] unknown");
         break;

      case M3_UW_FACE_SHORT: // 00A0 ??? shorthand face definition
      {
         vertno = ua_mdl_read_vertno(file);
         ua_poly_tessellator tess;

         ua_mdl_trace("[face] shorthand unk1=%u vertlist=", vertno);

         for (Uint16 i = 0; i < 4; i++)
         {
            Uint8 vertno = file.Read8();

            ua_vertex3d vert;
            vert.pos = vertex_list[vertno];
            tess.add_poly_vertex(vert);

            ua_mdl_trace("%u ", vertno);
         }

         const std::vector<ua_triangle3d_textured>& tri = tess.tessellate(0x0003);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());
      }
      break;

      case 0x00d2: // 00D2 ??? shorthand face definition
      {
         vertno = ua_mdl_read_vertno(file);
         ua_poly_tessellator tess;

         ua_mdl_trace("[face] vertno=%u vertlist=", vertno);

         for (Uint16 i = 0; i < 4; i++)
         {
            Uint8 vertno = file.Read8();

            ua_vertex3d vert;
            vert.pos = vertex_list[vertno];
            tess.add_poly_vertex(vert);

            ua_mdl_trace("%u ", vertno);
         }

         const std::vector<ua_triangle3d_textured>& tri = tess.tessellate(0x0004);
         triangles.insert(triangles.begin(), tri.begin(), tri.end());

         ua_mdl_trace("shorthand");
      }
      break;

      case M3_UW_FACE_UNK16: // 0016 ???
      {
         //Uint32 pos = file.Tell();

         Uint16 nvert = ua_mdl_read_vertno(file);
         unk1 = file.Read16();

         for (Uint16 n = 0; n < nvert; n++)
         {
            unk1 = file.Read8();
         }

         if (nvert & 1)
            file.Read8(); // read alignment padding
      }
      break;

      case 0x0012:
         unk1 = file.Read16();
         break;

      default:
         ua_mdl_trace("unknown command at offset 0x%08x\n", file.Tell() - 2);
         return;
      }
      ua_mdl_trace("\n");
   }
}

bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels, bool dump)
{
   Base::File file(filename, Base::modeRead);
   if (!file.IsOpen())
      return false;

   Uint32 base = 0; // models list base address

   // search all offsets for model table begin
   unsigned int max = SDL_TABLESIZE(ua_model_offset_table);
   for (unsigned int i = 0; i < max; i++)
   {
      // check value on file position
      file.Seek(ua_model_offset_table[i].table_offset, Base::seekBegin);
      Uint32 file_value = file.Read32();

      if (file_value == ua_model_offset_table[i].value)
      {
         // found position
         base = ua_model_offset_table[i].base_offset;
         file.Seek(ua_model_offset_table[i].table_offset, Base::seekBegin);

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
      double ex = ua_mdl_read_fixed(file);
      double ey = ua_mdl_read_fixed(file);
      double ez = ua_mdl_read_fixed(file);

      ua_vector3d extents(ex, ez, ey);

      ua_mdl_trace("dumping builtin model %u (%s)\noffset=0x%08x [unk1=0x%04x, extents=(%f,%f,%f) ]\n",
         n, ua_model_name[n], base + offsets[n], unk1, ex, ey, ez);

      ua_model3d_builtin* model = new ua_model3d_builtin;

      // temporary variables
      ua_vector3d origin;
      std::vector<ua_vector3d> vertex_list;

      // parse root node
      ua_model_parse_node(file, origin, vertex_list, model->triangles, dump);

      ua_mdl_trace("\n");

      /*model->*/origin.z -= extents.z / 2.0;
      model->extents = extents;

      // insert model
      ua_model3d_ptr model_ptr(model);
      allmodels.push_back(model_ptr);
   }

   return true;
}
