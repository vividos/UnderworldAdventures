//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file mapdisp.cpp
/// \brief level map display program
/// \details
/// the program's executable must be placed in the folder where the uw.exe
/// lives. uw_demo works, too. the executable needs the SDL.dll, placed into
/// the same dir as itself.
///
/// the user can rotate the map by dragging, holding the left mouse button.
/// when dragging with the right mouse button, the map is moved on the screen.
/// with the cursor up and down keys the distance to the viewpoint can be
/// adjusted. finally, page-up and page-down loads another map level.
//
#include <SDL2/SDL.h>
#include "Level.hpp"
#include "LevelList.hpp"
#include "File.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "ImageManager.hpp"
#include "LevelList.hpp"
#include "LevelImporter.hpp"
#include "MainGameLoop.hpp"
#include "RenderOptions.hpp"
#include "RenderWindow.hpp"
#include "Viewport.hpp"
#include "UnderworldRenderer.hpp"
#include <vector>
#include <algorithm>

/// map display class
class MapDisplay :
   public MainGameLoop,
   public IGame
{
public:
   /// ctor
   MapDisplay()
      :MainGameLoop("Underworld Adventures: Map Display", true),
      m_renderWindow(800, 600, "Underworld Adventures: Map Display", false),
      m_viewport(m_renderWindow),
      m_resourceManager(m_settings),
      m_imageManager(m_resourceManager),
      m_currentLevel(0),
      m_showAllMaps(false),
      m_cameraPos(80.0, -64.0, 2000.0),
      m_panAngle(70.0),
      m_rotateAngle(90.0),
      m_leftButtonDown(false)
   {
      m_viewport.SetViewport3D(0, 0, 800, 600);
   }

   /// parses command line arguments
   void ParseArgs(int argc, char* argv[])
   {
      // check if we have "allmaps" param
      if (argc > 1 && strcmp(argv[1], "allmaps") == 0)
         m_showAllMaps = true;
   }

   /// runs the map display application
   bool Run();

private:
   /// initializes map display
   bool Init();

   /// loads all levels
   void LoadLevels();

   /// prepares a single level for rendering
   void PrepareLevel(size_t levelIndex);

   /// renders current level
   void RenderLevel();

   // MainGameLoop virtual methods

   virtual void UpdateCaption(const char* windowTitle) override
   {
      m_renderWindow.SetWindowTitle(windowTitle);
   }

   virtual void OnTick(bool& resetTickTimer);

   virtual void OnRender();

   virtual void OnEvent(SDL_Event& event);

   /// handles key down event
   void HandleKeyDown(struct SDL_Keysym& keysym);

   /// creates a screenshot of the current scene
   void CreateScreenshot(bool largeResolution);

   /// writes a .tga files from given RGBA data
   static void WriteTgaFile(const char* filename, unsigned int xres, unsigned int yres, Uint32* data);

private:
   /// settings
   Base::Settings m_settings;

   /// resource manager
   Base::ResourceManager m_resourceManager;

   /// image manager
   ImageManager m_imageManager;

   /// list of levels
   Underworld::LevelList m_levelList;

   /// render options for mapdisp
   RenderOptions m_renderOptions;

   /// render window
   RenderWindow m_renderWindow;

   /// render viewport
   Viewport m_viewport;

   /// underworld renderer
   std::unique_ptr<UnderworldRenderer> m_renderer;

   /// stores currently displayed level
   size_t m_currentLevel;

   /// indicates if all maps should be shown at once
   bool m_showAllMaps;

   /// current camera position
   Vector3d m_cameraPos;

   double m_moveDirectionX;
   double m_moveDirectionY;

   double m_panAngle;
   double m_rotateAngle;
   bool m_leftButtonDown;

private:
   // IGame virtual methods

   virtual double GetTickRate() const override
   {
      return 20.0;
   }

   virtual bool PauseGame(bool pause) override
   {
      return false;
   }

   virtual Base::Settings& GetSettings() override
   {
      return m_settings;
   }

   virtual Base::ResourceManager& GetResourceManager() override
   {
      return m_resourceManager;
   }

   virtual Base::SavegamesManager& GetSavegamesManager() override
   {
      throw std::runtime_error("object not available");
   }

   virtual IScripting& GetScripting() override
   {
      throw std::runtime_error("object not available");
   }

   virtual IDebugServer& GetDebugger() override
   {
      throw std::runtime_error("object not available");
   }

   virtual GameStrings& GetGameStrings() override
   {
      throw std::runtime_error("object not available");
   }

   virtual Underworld::Underworld& GetUnderworld() override
   {
      throw std::runtime_error("object not available");
   }

   virtual Underworld::GameLogic& GetGameLogic() override
   {
      throw std::runtime_error("object not available");
   }

   virtual void InitGame() override
   {
   }

   virtual void DoneGame() override
   {
   }

   virtual Audio::AudioManager& GetAudioManager() override
   {
      throw std::runtime_error("object not available");
   }

   virtual ImageManager& GetImageManager() override
   {
      return m_imageManager;
   }

   virtual Renderer& GetRenderer() override
   {
      throw std::runtime_error("object not available");
   }

   virtual RenderWindow& GetRenderWindow() override
   {
      return m_renderWindow;
   }

   virtual Viewport& GetViewport() override
   {
      throw std::runtime_error("object not available");
   }

   virtual Physics::PhysicsModel& GetPhysicsModel() override
   {
      throw std::runtime_error("object not available");
   }

   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) override
   {
   }

   virtual void RemoveScreen() override
   {
   }

   virtual unsigned int GetScreenXRes() override
   {
      return 800;
   }

   virtual unsigned int GetScreenYRes() override
   {
      return 600;
   }
};

bool MapDisplay::Run()
{
   if (!Init())
      return false;

   MainGameLoop::RunGameLoop(20);

   SDL_Quit();

   return true;
}

bool MapDisplay::Init()
{
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // enable z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // give some rendering hints
   glHint(GL_FOG_HINT, GL_DONT_CARE);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

   m_renderOptions.m_renderVisibleTilesUsingOctree = false;
   m_renderOptions.m_useFog = false;

   LoadLevels();

   return true;
}

void MapDisplay::LoadLevels()
{
   m_settings.SetValue(Base::settingUnderworldPath, "./");

   m_resourceManager.DetectGameType(m_settings);

   m_imageManager.Init();

   Import::LevelImporter importer{ m_resourceManager };
   importer.LoadLevels(m_settings, m_levelList);

   m_renderer = std::make_unique<UnderworldRenderer>(*this);

   PrepareLevel(0);
}

void MapDisplay::PrepareLevel(size_t levelIndex)
{
   UaTrace("preparing level %u...\n", levelIndex);
   m_currentLevel = levelIndex;

   m_renderer->PrepareLevel(m_levelList.GetLevel(levelIndex));
}

void MapDisplay::OnTick(bool& resetTickTimer)
{
   Vector3d dirX{ 1.0, 0.0, 0.0 };
   dirX.RotateZ(-(-m_rotateAngle + 90.0));
   dirX.RotateX(-(m_panAngle + 270.0));

   dirX *= m_moveDirectionX;

   Vector3d dirY{ 0.0, 0.0, -1.0 };
   dirY.RotateX(-(m_panAngle + 270.0));
   dirY.RotateZ(-m_rotateAngle + 90.0);

   dirY *= m_moveDirectionY;

   Vector3d dir = dirX + dirY;

   m_cameraPos += dir;

   if (dir.Length() > 1e-12)
      UaTrace("dir = (%3.2f; %3.2f; %3.2f) pos = (%3.2f; %3.2f; %3.2f)\n",
         dir.x, dir.y, dir.z,
         m_cameraPos.x, m_cameraPos.y, m_cameraPos.z
      );
}

void MapDisplay::OnRender()
{
   RenderLevel();

   m_renderWindow.SwapBuffers();
}

void MapDisplay::RenderLevel()
{
   m_viewport.SetupCamera3D(90.0, 256.0);

   glEnable(GL_DEPTH_TEST);

   // clear color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   const Underworld::Level& level = m_levelList.GetLevel(m_currentLevel);
   if (level.GetTilemap().IsUsed())
   {
      m_renderer->Render(
         m_renderOptions,
         level,
         m_cameraPos,
         m_panAngle,
         m_rotateAngle,
         90.0);
   }

   // draw ground square
   glBegin(GL_QUADS);
   glColor3ub(32, 32, 32);
   glVertex3d(0, 0, -0.1);
   glVertex3d(64, 0, -0.1);
   glVertex3d(64, 64, -0.1);
   glVertex3d(0, 64, -0.1);
   glEnd();
}

void MapDisplay::OnEvent(SDL_Event& event)
{
   switch (event.type)
   {
   case SDL_KEYDOWN:
      HandleKeyDown(event.key.keysym);
      break;

   case SDL_KEYUP:
      m_moveDirectionX = 0.0;
      m_moveDirectionY = 0.0;
      break;

   case SDL_MOUSEBUTTONDOWN:
   {
      // check which mouse button was pressed
      Uint8 state = SDL_GetRelativeMouseState(NULL, NULL);
      if (SDL_BUTTON(state) == SDL_BUTTON_LEFT)
         m_leftButtonDown = true;
   }
   break;

   case SDL_MOUSEBUTTONUP:
      m_leftButtonDown = false;
      break;

   case SDL_MOUSEMOTION:
   {
      if (m_leftButtonDown)
      {
         // adjust values according to mouse movement
         int x, y;
         SDL_GetRelativeMouseState(&x, &y);

         m_rotateAngle += x * 0.2;
         m_panAngle += y * -0.2;
      }
   }
   break;
   }
}

void MapDisplay::HandleKeyDown(SDL_Keysym& keysym)
{
   switch (keysym.sym)
   {
   case SDLK_ESCAPE:
      QuitLoop();
      break;

   case SDLK_PAGEUP:
      if (!m_showAllMaps && m_currentLevel > 0)
         PrepareLevel(m_currentLevel - 1);
      break;

   case SDLK_PAGEDOWN:
      if (!m_showAllMaps && m_currentLevel < m_levelList.GetNumLevels() - 1)
         PrepareLevel(m_currentLevel + 1);
      break;

   case SDLK_w:
   case SDLK_UP:
      m_moveDirectionY = 1.0;
      break;

   case SDLK_s:
   case SDLK_x:
   case SDLK_DOWN:
      m_moveDirectionY = -1.0;
      break;

   case SDLK_d:
   case SDLK_RIGHT:
      m_moveDirectionX = 1.0;
      break;

   case SDLK_a:
   case SDLK_LEFT:
      m_moveDirectionX = -1.0;
      break;

   case SDLK_PRINTSCREEN:
      // create large screenshot when shift key is also down
      CreateScreenshot((keysym.mod & KMOD_SHIFT) != 0);
      break;

   default:
      break;
   }
}


#if 0
void load_all_levels()
{
   texmgr.reset();

   unsigned int maxlevels = levels.size();

   alltriangles.clear();
   alltriangles.reserve(12000 * maxlevels);

   for (unsigned int i = 0; i < maxlevels; i++)
   {
      {
         // set new caption
         char buffer[256];
         sprintf(buffer, "Underworld Adventures: Map Display; loading level %u ...", i);

         SDL_WM_SetCaption(buffer, NULL);
      }

      // prepare all used textures
      const std::vector<Uint16>& used_textures =
         levels[i].get_used_textures();

      unsigned int max = used_textures.size();
      for (unsigned int n = 0; n < max; n++)
         texmgr.prepare(used_textures[n]);

      ua_geometry_provider prov(levels[i]);

      // collect all triangles
      for (unsigned int x = 0; x < 64; x++)
         for (unsigned int y = 0; y < 64; y++)
            prov.get_tile_triangles(x, y, alltriangles);

      // translate all triangles up
      unsigned int maxtri = alltriangles.size();
      for (unsigned int t = 0; t < maxtri; t++)
      {
         for (unsigned int k = 0; k < 3; k++)
            alltriangles[t].vertices[k].pos.z += 32.0;
      }
   }

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());
}

void init_mapdisp()
{
   if (m_showAllMaps)
   {
      // load all level maps
      load_all_levels();
      zpos = 64.0;
      midz = -16.0;
   }
   else
   {
      // load first level
      load_level(0);
   }
}
#endif

void MapDisplay::CreateScreenshot(bool big)
{
   unsigned int xres = 800;
   unsigned int yres = 600;

   if (big)
   {
      // store projection matrix
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);

      unsigned int tilex = 4;
      unsigned int tiley = 4;

      std::vector<Uint32> image;
      image.resize(xres * yres * tilex * tiley);

      double left, right, bottom, top;

      for (unsigned int x = 0; x < tilex; x++)
      {
         for (int y = tiley - 1; y >= 0; y--)
         {
            {
               // set new caption
               char buffer[256];
               snprintf(buffer, sizeof(buffer),
                  "Underworld Adventures: Map Display; capturing image %u x %u",
                  x + 1,
                  tiley - y);

               m_renderWindow.SetWindowTitle(buffer);
            }

            left = -0.25 + x / double(tilex * 2);
            right = -0.25 + (x + 1) / double(tilex * 2);
            bottom = -0.25 + y / double(tiley * 2);
            top = -0.25 + (y + 1) / double(tiley * 2);

            left *= double(xres) / yres;
            right *= double(xres) / yres;
            left *= double(tilex) / tiley;
            right *= double(tilex) / tiley;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glFrustum(left, right, bottom, top, 0.25, 256.0);
            glMatrixMode(GL_MODELVIEW);

            RenderLevel();

            // store scanlines
            glReadBuffer(GL_BACK);

            // read in all scanlines
            for (int n = yres - 1; n >= 0; n--)
            {
               unsigned int pos =
                  (x * xres) +
                  (n * xres * tilex) +
                  ((y)*xres * yres * tilex)
                  ;

               glReadPixels(0, n, xres, 1, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
                  &image[pos]);
            }

            // show captured image
            m_renderWindow.SwapBuffers();
         }
      }

      m_renderWindow.SetWindowTitle("Underworld Adventures: Map Display; saving captured image ...");

      // saving tga file
      WriteTgaFile("mapdisp-shot.tga", xres * tilex, yres * tiley, image.data());

      // restore projection matrix
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
   }
   else
   {
      // normal capture
      std::vector<Uint32> image;
      image.resize(xres * yres);

      RenderLevel();

      // store scanlines
      glReadBuffer(GL_BACK);

      // read in all scanlines
      for (int n = yres - 1; n >= 0; n--)
      {
         unsigned int pos = n * xres;

         glReadPixels(0, n, xres, 1, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
            &image[pos]);
      }

      m_renderWindow.SetWindowTitle("Underworld Adventures: Map Display; saving captured image ...");

      // saving tga file
      WriteTgaFile("mapdisp-shot.tga", xres, yres, image.data());
   }
}

void MapDisplay::WriteTgaFile(const char* filename, unsigned int xres, unsigned int yres, Uint32* data)
{
   // write tga header
   Base::File file{ filename, Base::modeWrite };

   // write header
   file.Write8(0); // id length
   file.Write8(0); // color map flag
   file.Write8(2); // tga type (2=truecolor)
   file.Write16(0); // color map origin
   file.Write16(0); // color map length
   file.Write8(0); // color map depth
   file.Write16(0); // x origin
   file.Write16(0); // y origin
   file.Write16(xres); // width
   file.Write16(yres); // height
   file.Write8(32); // bits per pixel
   file.Write8(0x20); // image descriptor; 0x00 = bottomup

   for (int line = yres - 1; line >= 0; line--)
      file.WriteBuffer(reinterpret_cast<Uint8*>(&data[line * xres]), xres * 4);

   file.Close();
}

#if 0
void setup_opengl(int width, int height)
{
   float ratio = float(width) / height;

   // set up viewport
   glViewport(0, 0, width, height);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // clear color
   glClearColor(0, 0, 0, 0);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(90.0, ratio, 0.25, 256.0);
   //   glOrtho(-32.0*ratio,32.0*ratio, -32.0,32.0, 0.25, 256.0);

      // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
}
#endif

/// mapdisp main function
int main(int argc, char* argv[])
{
   MapDisplay mapDisplay;
   mapDisplay.ParseArgs(argc, argv);

   return mapDisplay.Run() ? 0 : 1;
}
