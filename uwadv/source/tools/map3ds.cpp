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
#include "renderer.hpp"
#include "resource/fread_endian.hpp"
#include <set>
#include <algorithm>

#define HAVE_LIB3DS

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

// create a big 3ds file with all levels in one file?
static bool one_map = false;

// create separate meshes for ceiling and water/lava
static bool sep_meshes = false;

#ifdef HAVE_LIB3DS
Lib3dsFile* file;
#endif


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

   // import all levelmaps
   ua_import_levelmaps(settings,"data/",levels);

   ua_mkdir("./levels3ds",0700);
}

void write_level(unsigned int curlevel, ua_level& level)
{
   printf("extracting triangles for level %u\n",curlevel);

   // retrieve all triangles
   std::vector<ua_triangle3d_textured> alltriangles,alltriangles_sep;

   for(unsigned int ypos=0; ypos<64; ypos++)
      for(unsigned int xpos=0; xpos<64; xpos++)
         ua_renderer::get_tile_triangles(level,xpos,ypos,alltriangles);

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());

   // separate meshes
   if (sep_meshes)
   {
      printf("separating meshes ... ");

      for(int n = alltriangles.size()-1; n>=0; n--)
      {
         ua_triangle3d_textured& tri = alltriangles[n];

         if ((tri.points[0].z > 31.9 && tri.points[0].z > 31.9 && tri.points[0].z > 31.9) ||
             (tri.texnum == 0x0119) || // lava
             (tri.texnum == 0x0110) || // water
             (tri.texnum == 0x0111) || // water
             (tri.texnum == 0x0120) || // water
             (tri.texnum == 0x0121) || // water
             (tri.texnum == 0x0122)    // water
            )
         {
            alltriangles_sep.push_back(tri);
            alltriangles.erase(alltriangles.begin()+n);
         }
      }

      printf("done\n");
   }

   if (one_map)
      printf("collecting triangles ...");
   else
      printf("writing 3ds file ... ");

   // write 3ds file
   char buffer[256];
   sprintf(buffer,"./levels3ds/level%02u.3ds",curlevel);

#ifdef HAVE_LIB3DS

   if (!one_map)
   {
      // create new 3ds file
      file = lib3ds_file_new();
   }

   std::set<Uint16> usedtextures;

   // insert all meshes
   unsigned int max_a = sep_meshes ? 2 : 1 ;
   for(unsigned int a=0; a<max_a; a++)
   {
      unsigned int max = a==0 ? alltriangles.size() : alltriangles_sep.size();

      char buffer2[64];

      sprintf(buffer2,"level%u%s",curlevel,a==0?"":"_sep");
      Lib3dsMesh* mesh = lib3ds_mesh_new(buffer2);

      // create points, texels and faces list
      lib3ds_mesh_new_point_list(mesh,max*3);
      lib3ds_mesh_new_texel_list(mesh,max*3);
      lib3ds_mesh_new_face_list(mesh,max);

      // process every triangle
      for(unsigned int j=0; j<max; j++)
      {
         ua_triangle3d_textured& tri =
            a==0 ? alltriangles[j] : alltriangles_sep[j];

         // point and texels list
         for(int n=0; n<3; n++)
         {
            Lib3dsPoint pt;

            pt.pos[0] = tri.points[n].x;
            pt.pos[1] = tri.points[n].y;
            pt.pos[2] = tri.points[n].z*0.125;

            if (one_map)
               pt.pos[2] -= 4.0*curlevel;

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

            face.flags = 7;
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

            // set face texture name
            sprintf(mesh->faceL[j].material,"tex%04x",tri.texnum);
         }

         // remember name of used texture
         usedtextures.insert(tri.texnum);
      }

      // insert new mesh
      lib3ds_file_insert_mesh(file,mesh);
   }

   // insert all used materials
   {
      std::set<Uint16>::iterator iter,stop;
      iter = usedtextures.begin();
      stop = usedtextures.end();

      for(;iter!=stop; iter++)
      {
         if (!one_map)
         {
            Lib3dsMaterial* mat = lib3ds_material_new();

            // set name for material and first texture map
            sprintf(mat->name,"tex%04x",*iter);
            sprintf(mat->texture1_map.name,"tex%04x.tga",*iter);
            mat->texture1_map.flags = 0;

            lib3ds_file_insert_material(file,mat);
         }
         allusedtextures.insert(*iter);
      }
   }

   if (!one_map)
   {
      // save and free 3ds file
      lib3ds_file_save(file,buffer);

      lib3ds_file_free(file);
      file = NULL;
   }

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

      // swap red and blue color byte of texture
      std::vector<Uint32> texels;
      {
         const Uint32* oldtexels = tex.get_texels();
         Uint32 temp = 0;
         Uint8 tempred = 0;

         unsigned int max = tex.get_xres()*tex.get_yres();
         for(unsigned int i=0; i<max; i++)
         {
            temp = oldtexels[i];

            tempred = temp & 0xff;       // save red
            temp &= 0xffffff00;          // mask out old red
            temp |= ((temp >> 16)&0xff); // move blue
            temp &= 0xff00ffff;          // mask out old blue
            temp |= Uint32(tempred)<<16; // add back red

            texels.push_back(temp);
         }
      }

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
      fwrite(&texels[0],tex.get_xres()*tex.get_yres(),4,fd);

      fclose(fd);

      tex.done();
   }

   printf("done\n");
}


#undef main

int main(int argc, char* argv[])
{
   printf("underworld adventures - level map to 3ds exporter\n\n");

   if (argc==2 && strcmp(argv[1],"sepmesh")==0)
      sep_meshes = true;

   // init exporter
   init_map3ds("./");

   if (one_map)
      file = lib3ds_file_new();

   // write all level maps
   unsigned int max = levels.size();
   for(unsigned int i=0; i<max; i++)
      write_level(i,levels[i]);

   // insert all used materials
   if (one_map)
   {
      std::set<Uint16>::iterator iter,stop;
      iter = allusedtextures.begin();
      stop = allusedtextures.end();

      for(;iter!=stop; iter++)
      {
         Lib3dsMaterial* mat = lib3ds_material_new();

         // set name for material and first texture map
         sprintf(mat->name,"tex%04x",*iter);
         sprintf(mat->texture1_map.name,"tex%04x.tga",*iter);

         lib3ds_file_insert_material(file,mat);
      }

      printf("writing 3ds file ... ");

      // save and free 3ds file
      lib3ds_file_save(file,"./levels3ds/all_levels.3ds");
      lib3ds_file_free(file);

      printf("done!\n");
   }

   // write textures
   write_textures();

   return 0;
}

// fake function to prevent linking with all the uwadv stuff

#include "files.hpp"

SDL_RWops* ua_files_manager::get_uadata_file(const char *filename)
{
   return SDL_RWFromFile(filename,"rb");
}

unsigned int ua_player::get_attr(ua_player_attributes) const { return 0; }
void ua_underworld::change_level(unsigned int){}
ua_level& ua_underworld::get_current_level(){ return levels[0]; }
void ua_conv_globals::import(char const *,bool){}
