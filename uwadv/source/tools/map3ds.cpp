/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file map3ds.cpp

   level map to 3ds exporter

*/

// needed includes
#include "common.hpp"
#include "level.hpp"
#include "utils.hpp"
#include "resource/fread_endian.hpp"
#include <set>
#include <algorithm>

#undef HAVE_LIB3DS

#ifdef HAVE_LIB3DS
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/material.h"
#endif

// globals

static std::vector<ua_level> levels;
static ua_settings settings;
static ua_texture_manager texmgr;
static std::set<Uint16> allusedtextures;


// prototypes

// levelmap loader, defined in resource/maploader.cpp
void ua_import_levelmaps(ua_settings &settings, const char *folder,
   std::vector<ua_level> &levels);


// functions

void init_map3ds(const char* uwpath)
{
   // we don't load settings, just set the needed one
   settings.set_gametype(ua_game_uw1);
   std::string uw_path(uwpath);
   settings.set_value(ua_setting_uw_path,uw_path);

   // check if we only have the demo
   FILE *fd = fopen("./data/level13.st","rb");
   if (fd!=NULL)
   {
      settings.set_gametype(ua_game_uw_demo);
      fclose(fd);
   }

   texmgr.init(settings);

   ua_import_levelmaps(settings,"data/",levels);

   ua_mkdir("./levels3ds",0700);
}

void write_level(unsigned int curlevel, ua_level& level)
{
   printf("extracting triangles for level %u\n",curlevel);

   // retrieve all triangles
   std::vector<ua_triangle3d_textured> alltriangles;

   for(unsigned int ypos=0; ypos<64; ypos++)
      for(unsigned int xpos=0; xpos<64; xpos++)
         level.get_tile_triangles(xpos,ypos,alltriangles);

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());

   printf("writing 3ds file ... ");

   // write 3ds file
   char buffer[256];
   sprintf(buffer,"./levels3ds/level%02u.3ds",curlevel,curlevel);

#ifdef HAVE_LIB3DS
   Lib3dsFile* file = lib3ds_file_new();

   std::set<Uint16> usedtextures;

   // insert all meshes
   {
      Uint16 curtexnum = alltriangles[0].texnum;
      unsigned int startidx = 0;

      unsigned int max = alltriangles.size();
      for(unsigned int i=0; i<max; i++)
      {
         if ( alltriangles[i].texnum != curtexnum && signed(i)-signed(startidx) > 0)
         {
            // we reached a new texture number; do a new mesh object
            char buffer2[64];
            sprintf(buffer2,"tri%04x",startidx);
            Lib3dsMesh* mesh = lib3ds_mesh_new(buffer2);

            // material name for all faces
            sprintf(buffer2,"tex%04x",alltriangles[i].texnum);

            // create points, texels and faces list
            lib3ds_mesh_new_point_list(mesh,(i-startidx)*3);
            lib3ds_mesh_new_texel_list(mesh,(i-startidx)*3);
            lib3ds_mesh_new_face_list(mesh,(i-startidx));

            // handle every triangle
            for(unsigned int j=0; j<i-startidx; j++)
            {
               ua_triangle3d_textured tri(alltriangles[j]);

               // point and texels list
               for(int n=0; n<3; n++)
               {
                  Lib3dsPoint pt;

                  pt.pos[0] = tri.points[n].x;
                  pt.pos[1] = tri.points[n].y;
                  pt.pos[2] = tri.points[n].z;

                  mesh->pointL[j*3+n] = pt;

                  // texels
                  mesh->texelL[j*3+n][0] = tri.tex_u[n];
                  mesh->texelL[j*3+n][1] = tri.tex_v[n];
               }

               // faces list
               {
                  Lib3dsFace face;
                  strncpy(face.material,buffer2,64);

                  face.points[0] = j*3+0;
                  face.points[1] = j*3+1;
                  face.points[2] = j*3+2;

                  face.flags = 0;
                  face.smoothing = 0;

                  // calculate normal
                  ua_vector3d line1, line2, normal;
                  line1.set(
                     tri.points[1].x-tri.points[0].x,
                     tri.points[1].y-tri.points[0].y,
                     tri.points[1].z-tri.points[0].z);

                  line2.set(
                     tri.points[2].x-tri.points[0].x,
                     tri.points[2].y-tri.points[0].y,
                     tri.points[2].z-tri.points[0].z);

                  normal.cross(line1,line2);

                  face.normal[0] = normal.x;
                  face.normal[1] = normal.y;
                  face.normal[2] = normal.z;

                  mesh->faceL[j] = face;
               }
            }

            // remember name of used texture
            usedtextures.insert(curtexnum);

            // set begin of new mesh
            startidx = i;
            curtexnum = alltriangles[i].texnum;

            lib3ds_file_insert_mesh(file,mesh);
         }
      }
   }

   // insert all used materials
   {
      std::set<Uint16>::iterator iter,stop;
      iter = usedtextures.begin();
      stop = usedtextures.end();

      for(;iter!=stop; iter++)
      {
         Lib3dsMaterial* mat = lib3ds_material_new();

         // set name for material and first texture map
         sprintf(mat->name,"tex%04x",*iter);
         sprintf(mat->texture1_map.name,"tex%04x.tga",*iter);

         lib3ds_file_insert_material(file,mat);

         allusedtextures.insert(*iter);
      }
   }

   // save and free 3ds file
   lib3ds_file_save(file,buffer);

   lib3ds_file_free(file);
#endif // HAVE_LIB3DS

   printf("done\n");
}

void write_textures()
{
   printf("writing textures ... ");

   std::set<Uint16>::iterator iter,stop;
   iter = allusedtextures.begin();
   stop = allusedtextures.end();

   for(;iter!=stop; iter++)
   {
      // get stock texture
      ua_texture tex;
      tex.init(&texmgr);
      texmgr.stock_to_external(*iter,tex);

      // write tga file with texture
      char buffer2[64];
      sprintf(buffer2,"./levels3ds/tex%04x.tga",*iter);

      FILE *fd = fopen(buffer2,"wb");
      {
         // write header
         fputc(0,fd); // id length
         fputc(0,fd); // color map flag
         fputc(2,fd); // tga type (2=truecolor)
         fwrite16(fd,0); // color map origin
         fwrite16(fd,0); // color map length
         fputc(0,fd); // color map depth
         fwrite16(fd,0); // x origin
         fwrite16(fd,0); // y origin
         fwrite16(fd,tex.get_xres()); // width
         fwrite16(fd,tex.get_yres()); // height
         fputc(32,fd); // bits per pixel
         fputc(0x20,fd); // image descriptor; 0x00 = bottomup
      }

      // write texture texels
      const Uint32* texels = tex.get_texels();

      fwrite(texels,tex.get_xres()*tex.get_yres(),4,fd);

      fclose(fd);

      tex.done();
   }

   printf("done\n");
}


#undef main

int main(int argc, char* argv[])
{
   printf("underworld adventures - level map to 3ds exporter\n\n");

   // init exporter
   init_map3ds("./");

   // write all level maps
   unsigned int max = levels.size();
   for(unsigned int i=0; i<max; i++)
      write_level(i,levels[i]);

   // write textures
   write_textures();

   return 0;
}
