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
/*! \file importwrl.cpp

   \brief vrml97 importer

   a

*/

// needed includes
#include "common.hpp"
#include "models.hpp"
#include "wrllexer.hpp"
#include "core.hpp"


// ua_model3d_wrl methods

void ua_model3d_wrl::import_wrl(ua_game_core_interface* core, SDL_RWops* rwops)
{
   ua_wrl_lexer lexer(rwops);
   int level = 0;
   int res,res2;
   bool ccw = true;

   std::string texture_url;

   do
   {
      res = lexer.yylex();

      switch(res)
      {
      case ua_wt_bracket_open:
         level++;
         break;

      case ua_wt_bracket_close:
         level--;
         break;

      case ua_wt_keyword:
         {
            std::string str(lexer.get_string());

            if (str == "texture")
            {
               // texture url

               lexer.yylex(); // string: ImageTexture
               lexer.yylex(); // bracket open
               lexer.yylex(); // string: url

               lexer.yylex(); // url string
               texture_url.assign(lexer.get_string());

               lexer.yylex(); // bracket close
            }
            else
            if (str == "coord")
            {
               // vertex coordinates

               lexer.yylex(); // keyword: DEF
               lexer.yylex(); // string: <name>-COORD
               lexer.yylex(); // keyword: Coordinate

               lexer.yylex(); // bracket open
               lexer.yylex(); // keyword: point
               lexer.yylex(); // bracket open

               res2 = lexer.yylex();

               // read in all vertex coords
               double vert[3];
               do
               {
                  // read in 3 values
                  for(unsigned int n=0; n<3; n++)
                  {
                     if (res2==ua_wt_float)
                        vert[n] = lexer.get_float();
                     else
                        vert[n] = static_cast<double>(lexer.get_int());

                     res2 = lexer.yylex();
                  }

                  // store coordinates (swap y and z)
                  ua_vector3d vec(vert[0],vert[2],vert[1]);
                  coords.push_back(vec);

               } while(res2 != ua_wt_bracket_close);

               //lexer.yylex(); // bracket close
            }
            else
            if (str == "texCoord")
            {
               // texture u/v coordinates

               lexer.yylex(); // keyword: DEF
               lexer.yylex(); // string: <name>-TEXCOORD
               lexer.yylex(); // keyword: TextureCoordinate

               lexer.yylex(); // bracket open
               lexer.yylex(); // keyword: point
               lexer.yylex(); // bracket open

               res2 = lexer.yylex();

               // read in all vertex coords
               double texel[2];
               do
               {
                  // read in 2 values
                  for(unsigned int n=0; n<2; n++)
                  {
                     if (res2==ua_wt_float)
                        texel[n] = lexer.get_float();
                     else
                        texel[n] = static_cast<double>(lexer.get_int());

                     res2 = lexer.yylex();
                  }

                  // store coordinate
                  ua_vector2d texel(texel[0],texel[1]);
                  texcoords.push_back(texel);

               } while(res2 != ua_wt_bracket_close);

               lexer.yylex(); // bracket close
            }
            else
            if (str == "coordIndex")
            {
               lexer.yylex(); // bracket open
               res2 = lexer.yylex();

               // read in all coord indices
               int indices[4];
               do
               {
                  // read in 4 values
                  for(unsigned int n=0; n<4; n++)
                  {
                     indices[n] = lexer.get_int();
                     res2 = lexer.yylex();
                  }

                  coord_index.push_back(indices[0]);
                  coord_index.push_back(indices[ccw?2:1]);
                  coord_index.push_back(indices[ccw?1:2]);

               } while(res2 != ua_wt_bracket_close);
            }
            else
            if (str == "texCoordIndex")
            {
               lexer.yylex(); // bracket open
               res2 = lexer.yylex();

               // read in all coord indices
               int indices[4];
               do
               {
                  // read in 4 values
                  for(unsigned int n=0; n<4; n++)
                  {
                     indices[n] = lexer.get_int();
                     res2 = lexer.yylex();
                  }

                  texcoord_index.push_back(indices[0]);
                  texcoord_index.push_back(indices[ccw?2:1]);
                  texcoord_index.push_back(indices[ccw?1:2]);

               } while(res2 != ua_wt_bracket_close);
            }
         }
         break;

      case ua_wt_invalid:
         ua_trace("unknown character in .wrl file\n");
         res = -1;
         break;

      default:
         break;
      }

   } while (res > 0);

   // load texture
   if (res==0)
   {
//      core->get_texmgr().load_textures();
//      tex.load();
   }
}

void ua_model3d_wrl::render(ua_vector3d& base)
{
   // render object
   unsigned int max = coord_index.size();
   for(unsigned int i=0; i<max; i+=3)
   {
      //glBegin(GL_LINE_LOOP);
      glBegin(GL_TRIANGLES);

      for(unsigned int n=0; n<3; n++)
      {
         int c_idx = coord_index[i+n];
         int t_idx = texcoord_index[i+n];

         ua_vector3d vec3d(coords[c_idx]);
         ua_vector2d vec2d(texcoords[t_idx]);

         vec3d *= 0.008;

         glTexCoord2d(vec2d.x,vec2d.y);
         glVertex3d(vec3d.x+base.x,vec3d.y+base.y,vec3d.z+base.z);
      }

      glEnd();
   }
}
