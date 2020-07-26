//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2020 Underworld Adventures Team
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
/// \file VrmlImporter.cpp
/// \brief VRML97 importer
//
#include "pch.hpp"
#include "VrmlImporter.hpp"
#include "Model3DVrml.hpp"
#include "vrml/WrlLexer.hpp"

std::shared_ptr<Model3DVrml> Import::VrmlImporter::ImportWrl(
   SDL_RWops* rwops, std::string relativePath)
{
   auto model = std::make_shared<Model3DVrml>();

   WrlLexer lexer{ rwops };
   int level = 0;
   int result, result2;
   bool ccw = true;

   Vector3d translate;
   Vector3d rotateAxis;
   double rotateAngle = 0.0;
   Vector3d scale;

   scale.Set(1.0, 1.0, 1.0);
   rotateAxis.Set(1.0, 0.0, 0.0);

   std::string textureUrl;

   do
   {
      result = lexer.yylex();

      switch (result)
      {
      case tokenBracketOpen:
         level++;
         break;

      case tokenBracketClose:
         level--;
         break;

      case tokenKeyword:
      {
         std::string str{ lexer.GetString() };

         if (str == "texture")
         {
            // texture url

            lexer.yylex(); // string: ImageTexture
            lexer.yylex(); // bracket open
            lexer.yylex(); // string: url

            lexer.yylex(); // url string
            textureUrl.assign(lexer.GetString());

            // remove enclosing " chars
            textureUrl.erase(0, 1);
            textureUrl.erase(textureUrl.size() - 1);

            lexer.yylex(); // bracket close
         }
         else if (str == "coord")
         {
            // vertex coordinates

            lexer.yylex(); // keyword: DEF
            lexer.yylex(); // string: <name>-COORD
            lexer.yylex(); // keyword: Coordinate

            lexer.yylex(); // bracket open
            lexer.yylex(); // keyword: point
            lexer.yylex(); // bracket open

            result2 = lexer.yylex();

            // read in all vertex coords
            double vert[3];
            do
            {
               // read in 3 values
               for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++)
               {
                  if (result2 == tokenFloat)
                     vert[vertexIndex] = lexer.GetFloat();
                  else
                     vert[vertexIndex] = static_cast<double>(lexer.GetInt());

                  result2 = lexer.yylex();
               }

               // store coordinates (swap y and z)
               Vector3d vec{ vert[0],vert[2],vert[1] };
               model->m_coords.push_back(vec);

            } while (result2 != tokenBracketClose);

            //lexer.yylex(); // bracket close
         }
         else if (str == "texCoord")
         {
            // texture u/v coordinates

            lexer.yylex(); // keyword: DEF
            lexer.yylex(); // string: <name>-TEXCOORD
            lexer.yylex(); // keyword: TextureCoordinate

            lexer.yylex(); // bracket open
            lexer.yylex(); // keyword: point
            lexer.yylex(); // bracket open

            result2 = lexer.yylex();

            // read in all vertex coords
            double texel[2];
            do
            {
               // read in 2 values
               for (unsigned int texelIndex = 0; texelIndex < 2; texelIndex++)
               {
                  if (result2 == tokenFloat)
                     texel[texelIndex] = lexer.GetFloat();
                  else
                     texel[texelIndex] = static_cast<double>(lexer.GetInt());

                  result2 = lexer.yylex();
               }

               // store coordinate
               Vector2d texel2d{ texel[0], texel[1] };
               model->m_texCoords.push_back(texel2d);

            } while (result2 != tokenBracketClose);

            lexer.yylex(); // bracket close
         }
         else if (str == "coordIndex")
         {
            lexer.yylex(); // bracket open
            result2 = lexer.yylex();

            // read in all coord indices
            int indices[4];
            do
            {
               // read in 4 values
               for (unsigned int coordIndex = 0; coordIndex < 4; coordIndex++)
               {
                  indices[coordIndex] = lexer.GetInt();
                  result2 = lexer.yylex();
               }

               model->m_coordIndex.push_back(indices[0]);
               model->m_coordIndex.push_back(indices[ccw ? 2 : 1]);
               model->m_coordIndex.push_back(indices[ccw ? 1 : 2]);

            } while (result2 != tokenBracketClose);
         }
         else if (str == "texCoordIndex")
         {
            lexer.yylex(); // bracket open
            result2 = lexer.yylex();

            // read in all coord indices
            int indices[4];
            do
            {
               // read in 4 values
               for (unsigned int texCoordIndex = 0; texCoordIndex < 4; texCoordIndex++)
               {
                  indices[texCoordIndex] = lexer.GetInt();
                  result2 = lexer.yylex();
               }

               model->m_texCoordIndex.push_back(indices[0]);
               model->m_texCoordIndex.push_back(indices[ccw ? 2 : 1]);
               model->m_texCoordIndex.push_back(indices[ccw ? 1 : 2]);

            } while (result2 != tokenBracketClose);
         }
         else if (str == "translation")
         {
            double vec[3];

            // read in 3 values
            for (unsigned int vectorIndex = 0; vectorIndex < 3; vectorIndex++)
            {
               result2 = lexer.yylex();

               if (result2 == tokenFloat)
                  vec[vectorIndex] = lexer.GetFloat();
               else
                  vec[vectorIndex] = static_cast<double>(lexer.GetInt());
            }

            translate.Set(vec[0], vec[2], vec[1]);
         }
         else if (str == "rotation")
         {
            double vec[4];

            // read in 3 axis values and 1 angle value
            for (unsigned int rotationIndex = 0; rotationIndex < 4; rotationIndex++)
            {
               result2 = lexer.yylex();

               if (result2 == tokenFloat)
                  vec[rotationIndex] = lexer.GetFloat();
               else
                  vec[rotationIndex] = static_cast<double>(lexer.GetInt());
            }

            rotateAxis.Set(vec[0], vec[2], vec[1]);
            rotateAngle = Rad2deg(vec[3]);
         }
         else if (str == "scale")
         {
            double vec[3];

            // read in 3 values
            for (unsigned int scaleIndex = 0; scaleIndex < 3; scaleIndex++)
            {
               result2 = lexer.yylex();

               if (result2 == tokenFloat)
                  vec[scaleIndex] = lexer.GetFloat();
               else
                  vec[scaleIndex] = static_cast<double>(lexer.GetInt());
            }

            scale.Set(vec[0], vec[2], vec[1]);
         }
      }
      break;

      case tokenInvalid:
         UaTrace("failed loading model: unknown character in %s\n",
            relativePath.c_str());
         result = -1;
         break;

      default:
         break;
      }

   } while (result > 0);

   // put pixels and texels into vertex buffers
   for (size_t coordIndex = 0; coordIndex < model->m_coords.size(); coordIndex++)
   {
      model->m_coords[coordIndex] *= scale;
      model->m_coords[coordIndex].Rotate(rotateAxis, rotateAngle);
      model->m_coords[coordIndex] += translate;
   }

   // load texture
   if (result == 0)
   {
      model->m_texture.Init(1);

      // construct texture name
      std::string::size_type pos = relativePath.find_last_of("\\/");

      if (pos == std::string::npos) pos = 0;
      else pos++;

      relativePath.erase(pos);
      relativePath.append(textureUrl);

      // load texture
      Base::SDL_RWopsPtr textureRwops =
         m_resourceManager.GetResourceFile(relativePath);

      model->m_texture.Load(textureRwops);
      model->m_texture.Upload();
   }

   return model;
}
