/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file modeldecode.cpp

   \brief original model decoding

*/

// needed includes
#include "common.hpp"
#include "fread_endian.hpp"
#include "uamath.hpp"
#include <vector>


// tables

//! underworld models executable positions
/*! Underworld models are (unfortunately) buried deep inside the executable
    table was taken from TSSHP, at
    http://madeira.physiol.ucl.ac.uk/tsshp/sshock.html
*/
struct
{
  Uint32 table_offset;
  Uint32 value;          /* 4 bytes at table_offset */
  Uint32 base_offset;
} ua_model_offset_table [] =
{
   { 0x0004e910, 0x40064ab6, 0x0004e99e },
   { 0x0004ccd0, 0x40064ab6, 0x0004cd5e }, /* same models, different place */
   { 0x0004e370, 0x40064ab6, 0x0004e3fe }, /* ditto (reported Gerd Bitzer) */
   { 0x00054cf0, 0x59aa64d4, 0x00054d8a },
   { 0x000550e0, 0x59aa64d4, 0x0005517a }, /* another UW2 build            */
};


// enums

//! model node commands
enum ua_model_nodecmd
{
   M3_UW_ENDNODE =       0x0000,
   M3_UW_SORT_PLANE =    0x0006,
   M3_UW_SORT_PLANE_ZY = 0x000C,
   M3_UW_SORT_PLANE_XY = 0x000E,
   M3_UW_SORT_PLANE_XZ = 0x0010,
   M3_UW_COLOR_DEF =     0x0014, //???
   M3_UW_FACE_UNK16 =    0x0016, //???
   M3_UW_FACE_UNK40 =    0x0040,
   M3_UW_FACE_PLANE =    0x0058,
   M3_UW_FACE_PLANE_ZY = 0x005E,
   M3_UW_FACE_PLANE_XY = 0x0060,
   M3_UW_FACE_PLANE_XZ = 0x0062,
   M3_UW_FACE_PLANE_X =  0x0064,
   M3_UW_FACE_PLANE_Z =  0x0066,
   M3_UW_FACE_PLANE_Y =  0x0068,
   M3_UW_ORIGIN =        0x0078,
   M3_UW_VERTEX =        0x007A,
   M3_UW_FACE_VERTICES = 0x007E,
   M3_UW_VERTICES =      0x0082,
   M3_UW_VERTEX_X =      0x0086,
   M3_UW_VERTEX_Z =      0x0088,
   M3_UW_VERTEX_Y =      0x008A,
   M3_UW_VERTEX_CEIL =   0x008C,
   M3_UW_VERTEX_XZ =     0x0090,
   M3_UW_VERTEX_XY =     0x0092,
   M3_UW_VERTEX_YZ =     0x0094,
   M3_UW_FACE_SHORT =    0x00A0,
   M3_UW_TEXTURE_FACE  = 0x00A8,
   M3_UW_TMAP_VERTICES = 0x00B4,
   M3_UW_FACE_SHADE =    0x00BC,
   M3_UW_FACE_TWOSHADES= 0x00BE,
   M3_UW_VERTEX_DARK =   0x00D4,
   M3_UW_FACE_GOURAUD =  0x00D6,
};


// helper functions

double ua_mdl_read_fixed(FILE* fd)
{
   Sint16 val = static_cast<Sint16>(fread16(fd));
   return val / 16.0;
}

Uint16 ua_mdl_read_vertno(FILE* fd)
{
   Uint16 val = fread16(fd);
   return val / 8;
}

void ua_mdl_store_vertex(const ua_vector3d& vertex, Uint16 vertno,
   std::vector<ua_vector3d>& vertex_list)
{
   if (vertex_list.size()<=vertno)
      vertex_list.resize(vertno+1);

   vertex_list[vertno] = vertex;
}


// global functions

void ua_model_parse_node(FILE* fd, std::vector<ua_vector3d>& vertex_list)
{
   // parse node until end node
   bool loop = true;

   while(loop)
   {
      // read next command
      ua_model_nodecmd cmd = (ua_model_nodecmd)fread16(fd);
      Uint16 refvert, vertno, unk1;
      double vx, vy, vz;
      ua_vector3d refvect;

      switch(cmd)
      {
         // misc. nodes
      case M3_UW_ENDNODE: // 0000 end node
         loop = false;
         break;

      case M3_UW_ORIGIN: // 0078 define model center
         vertno = ua_mdl_read_vertno(fd);
         vx = ua_mdl_read_fixed(fd);
         vy = ua_mdl_read_fixed(fd);
         vz = ua_mdl_read_fixed(fd);
         unk1 = fread16(fd);
         break;

         // vertex definition nodes
      case M3_UW_VERTEX: // 007a define initial vertex
         vx = ua_mdl_read_fixed(fd);
         vy = ua_mdl_read_fixed(fd);
         vz = ua_mdl_read_fixed(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = ua_vector3d(vx,vy,vz);
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTICES: // 0082 define initial vertices
         {
            Uint16 nvert = fread16(fd);
            vertno = fread16(fd);
            for(unsigned int n=0; n<nvert; n++)
            {
               vx = ua_mdl_read_fixed(fd);
               vy = ua_mdl_read_fixed(fd);
               vz = ua_mdl_read_fixed(fd);

               refvect = ua_vector3d(vx,vy,vz);
               ua_mdl_store_vertex(refvect,vertno+n, vertex_list);
            }
         }
         break;

      case M3_UW_VERTEX_X: // 0086 define vertex offset X
         refvert = ua_mdl_read_vertno(fd);
         vx = ua_mdl_read_fixed(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_Z: // 0088 define vertex offset Z
         refvert = ua_mdl_read_vertno(fd);
         vz = ua_mdl_read_fixed(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.z += vz;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_Y: // 008a define vertex offset Y
         refvert = ua_mdl_read_vertno(fd);
         vy = ua_mdl_read_fixed(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.y += vy;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_XZ: // 0090 define vertex offset X,Z
         vx = ua_mdl_read_fixed(fd);
         vz = ua_mdl_read_fixed(fd);
         refvert = ua_mdl_read_vertno(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.z += vz;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_XY: // 0092 define vertex offset X,Y
         vx = ua_mdl_read_fixed(fd);
         vy = ua_mdl_read_fixed(fd);
         refvert = ua_mdl_read_vertno(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.x += vx;
         refvect.y += vy;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_YZ: // 0094 define vertex offset Y,Z
         vy = ua_mdl_read_fixed(fd);
         vz = ua_mdl_read_fixed(fd);
         refvert = ua_mdl_read_vertno(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.y += vy;
         refvect.z += vz;
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

      case M3_UW_VERTEX_CEIL: // 008c define vertex variable height
         refvert = ua_mdl_read_vertno(fd);
         unk1 = fread16(fd);
         vertno = ua_mdl_read_vertno(fd);

         refvect = vertex_list[refvert];
         refvect.z = 32.0; // todo: ceiling value
         ua_mdl_store_vertex(refvect,vertno, vertex_list);
         break;

         // face plane checks
      case M3_UW_FACE_PLANE: // 0058 define face plane, arbitrary heading
         fread16(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         break;

      case M3_UW_FACE_PLANE_X: // 0064 define face plane X
      case M3_UW_FACE_PLANE_Z: // 0066 define face plane Z
      case M3_UW_FACE_PLANE_Y: // 0068 define face plane Y
         fread16(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         break;

      case M3_UW_FACE_PLANE_ZY: // 005e define face plane Z/Y
      case M3_UW_FACE_PLANE_XY: // 0060 define face plane X/Y
      case M3_UW_FACE_PLANE_XZ: // 0062 define face plane X/Z
         fread16(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         break;

         // face info nodes
      case M3_UW_FACE_VERTICES: // 007e define face vertices
         {
            Uint16 nvert = fread16(fd);
            for(Uint16 n=0; n<nvert; n++)
            {
               vertno = ua_mdl_read_vertno(fd);
            }
         }
         break;

      case M3_UW_TEXTURE_FACE: // 00a8 define texture-mapped face
      case M3_UW_TMAP_VERTICES: // 00b4 define face vertices with u,v information
         {
            // read texture number
            if (cmd==M3_UW_TEXTURE_FACE)
               fread16(fd);

            Uint16 nvert = fread16(fd);

            for(Uint16 n=0; n<nvert; n++)
            {
               vertno = ua_mdl_read_vertno(fd);
               double u = ua_mdl_read_fixed(fd);
               double v = ua_mdl_read_fixed(fd);
            }
         }
         break;

         // sort nodes
      case M3_UW_SORT_PLANE: // 0006 define sort node, arbitrary heading
         ua_mdl_read_fixed(fd);
         ua_mdl_read_fixed(fd);
         // fall-through

      case M3_UW_SORT_PLANE_ZY: // 000C define sort node, ZY plane
      case M3_UW_SORT_PLANE_XY: // 000E define sort node, XY plane
      case M3_UW_SORT_PLANE_XZ: // 0010 define sort node, XZ plane
         {
            ua_mdl_read_fixed(fd);
            ua_mdl_read_fixed(fd);
            ua_mdl_read_fixed(fd);
            ua_mdl_read_fixed(fd);

            Uint32 left = fread16(fd);
            left += ftell(fd);

            Uint32 right = fread16(fd);
            right += ftell(fd);

            long here = ftell(fd);

            // parse left nodes
            fseek(fd,left,SEEK_SET);
            ua_model_parse_node(fd,vertex_list);

            // parse right nodes
            fseek(fd,right,SEEK_SET);
            ua_model_parse_node(fd,vertex_list);

            // return to "here"
            fseek(fd,here,SEEK_SET);
         }
         break;

         // unknown nodes
      case M3_UW_COLOR_DEF: // 0014 ??? colour definition
         refvert = ua_mdl_read_vertno(fd);
         unk1 = fread16(fd);
         vertno = ua_mdl_read_vertno(fd);
         break;

      case M3_UW_FACE_SHADE: // 00BC define face shade
         unk1 = fread16(fd);
         unk1 = fread16(fd);
         break;

      case M3_UW_FACE_TWOSHADES: // 00BE ??? seems to define 2 shades
         unk1 = fread16(fd);
         unk1 = fread16(fd);
         break;

      case M3_UW_VERTEX_DARK: // 00D4 define dark vertex face (?)
         {
            Uint16 nvert = fread16(fd);
            unk1 = fread16(fd);

            for(Uint16 n=0; n<nvert; n++)
            {
               vertno = ua_mdl_read_vertno(fd);
               unk1 = fgetc(fd);
            }

            if (nvert & 1)
               fgetc(fd); // read alignment padding
         }
         break;

      case M3_UW_FACE_GOURAUD: // 00D6 define gouraud shading
         break;

      case M3_UW_FACE_UNK40: // 0040 ???
         break;

      case M3_UW_FACE_SHORT: // 00A0 ??? shorthand face definition
         vertno = ua_mdl_read_vertno(fd);
         unk1 = fgetc(fd);
         unk1 = fgetc(fd);
         unk1 = fgetc(fd);
         unk1 = fgetc(fd);
         break;

      case M3_UW_FACE_UNK16: // 0016 ???
         {
            Uint32 pos = ftell(fd);

            Uint16 nvert = ua_mdl_read_vertno(fd);
            unk1 = fread16(fd);

            for(Uint16 n=0; n<nvert; n++)
            {
               unk1 = fgetc(fd);
            }

            if (nvert & 1)
               fgetc(fd); // read alignment padding
         }
         break;

      case 0x0012:
         unk1 = fread16(fd);
         break;

      default:
         ua_trace("unknown command: %04x at offset 0x%08x\n",cmd,ftell(fd)-2);
         return;
      }
   }
}

bool ua_model_decode_all(const char* filename)
{
   // open file
   FILE* fd = fopen(filename,"rb");
   if (fd == NULL)
      return false;

   Uint32 base = 0; // models list base address

   // search all offsets for model table begin
   unsigned int max = SDL_TABLESIZE(ua_model_offset_table);
   for(unsigned int i=0; i<max; i++)
   {
      // check value on file position
      fseek(fd,ua_model_offset_table[i].table_offset,SEEK_SET);
      Uint32 file_value = fread32(fd);

      if (file_value == ua_model_offset_table[i].value)
      {
         // found position
         base = ua_model_offset_table[i].base_offset;
         fseek(fd,ua_model_offset_table[i].table_offset,SEEK_SET);

         ua_trace("found models in %s at 0x%08x\n",filename,base);
         break;
      }
   }

   if (base == 0)
   {
      ua_trace("didn't find models in file %s!\n",filename);
      return false; // didn't find list
   }

   // read in offsets
   Uint16 offsets[32];

   for(unsigned int j=0; j<32; j++)
      offsets[j] = fread16(fd);

   // parse all models
   for(unsigned int n=0; n<32; n++)
   {
      // seek to model
      fseek(fd,base + offsets[n],SEEK_SET);

      // read header
      Uint32 unk1 = fread32(fd);

      // extents
      double ex = ua_mdl_read_fixed(fd);
      double ey = ua_mdl_read_fixed(fd);
      double ez = ua_mdl_read_fixed(fd);

      ua_trace(" loading model %u, offset=0x%08x {unk1=0x%04x, e=(%3.2f, %3.2f, %3.2f) }\n",
         n,base + offsets[n],unk1,ex,ey,ez);

      // parse root node
      std::vector<ua_vector3d> vertex_list;
      ua_model_parse_node(fd,vertex_list);
   }

   fclose(fd);

   return true;
}


#ifdef DEV_DO_DEBUGGING

int main()
{
//   ua_model_decode_all("e:\\uw1\\uw.exe");
   ua_model_decode_all("d:\\projekte\\uwadv\\uw1\\uw.exe");
   return 0;
}

#endif
