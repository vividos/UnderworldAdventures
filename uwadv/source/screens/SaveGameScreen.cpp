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
/// \file SaveGameScreen.cpp
/// \brief save game screen implementation
/// \todo add scroll buttons
//
#include "pch.hpp"
#include "SaveGameScreen.hpp"
#include "OriginalIngameScreen.hpp"
#include "Renderer.hpp"
#include "Underworld.hpp"
#include "GameStrings.hpp"
#include "Audio.hpp"
#include "ImageManager.hpp"
#include <sstream>

const unsigned int SaveGameButton::c_buttonWidth = 55;

const double SaveGameScreen::s_fadeTime = 0.5;

/// Draws edges into an image.
/// \param img image to draw to
/// \param xpos x start position
/// \param ypos y start position
/// \param width width of the edged area to draw
/// \param height height of the edged area
/// \param inner indicates if the edge is a sunken edge (true)
void DrawImageEdges(IndexedImage& img, unsigned int xpos, unsigned int ypos,
   unsigned int width, unsigned int height, bool inner)
{
   Uint8 colorLeft = 165, colorRight = 170;
   unsigned int edgeWidth = 1;

   // left edge
   img.FillRect(xpos, ypos + 1, edgeWidth, height - 2,
      inner ? colorLeft : colorRight);
   // top edge
   img.FillRect(xpos + edgeWidth, ypos, width - edgeWidth * 2, 1,
      inner ? colorLeft : colorRight);

   // right edge
   img.FillRect(xpos + width - edgeWidth, ypos + 1, edgeWidth, height - 2,
      inner ? colorRight : colorLeft);
   // bottom edge
   img.FillRect(xpos + edgeWidth, ypos + height - 1, width - edgeWidth * 2, 1,
      inner ? colorRight : colorLeft);
}

void SaveGameButton::Init(SaveGameScreen* screen,
   IGame& game, unsigned int xpos, unsigned int ypos,
   const char* text, SaveGameButtonId buttonId)
{
   std::string buttonText(text);

   m_screen = screen;
   m_buttonId = buttonId;
   m_isLeftButtonDown = m_isRightButtonDown = false;

   Font m_normalFont;

   if (game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
   {
      // uw_demo only
      m_buttonImages.resize(3);
      m_buttonImages[0].Create(c_buttonWidth, 16);
      m_buttonImages[0].SetPalette(game.GetImageManager().GetPalette(3));

      m_buttonImages[2] = m_buttonImages[1] = m_buttonImages[0];

      m_buttonImages[0].FillRect(0, 0, c_buttonWidth, 16, 29);
      DrawImageEdges(m_buttonImages[1], 0, 0, c_buttonWidth, 16, false);
      DrawImageEdges(m_buttonImages[2], 0, 0, c_buttonWidth, 16, true);

      m_normalFont.Load(game.GetResourceManager(), fontButtons);

      Base::String::Uppercase(buttonText);
   }
   else
   {
      // load buttons (unpressed/pressed)
      game.GetImageManager().LoadList(m_buttonImages, "chrbtns", 0, 3, 3);

      // shorten button images according to button width
      IndexedImage& button0 = m_buttonImages[0];
      button0.PasteRect(button0, 66, 0, 1, 16, c_buttonWidth - 1, 0);

      IndexedImage& button1 = m_buttonImages[1];
      button1.PasteRect(button1, 66, 0, 1, 16, c_buttonWidth - 1, 0);

      IndexedImage& button2 = m_buttonImages[2];
      button2.PasteRect(button2, 66, 0, 1, 16, c_buttonWidth - 1, 0);

      m_normalFont.Load(game.GetResourceManager(), fontCharacterGeneration);
   }

   {
      // paste text onto buttons
      IndexedImage tempImage;

      // normal
      m_normalFont.CreateString(tempImage, buttonText.c_str(), 162);

      int text_xpos = (signed(c_buttonWidth) - signed(tempImage.GetXRes())) / 2;
      if (text_xpos < 0)
         text_xpos = 0;
      unsigned int text_ypos = (16 - tempImage.GetYRes()) / 2;

      m_buttonImages[1].PasteImage(tempImage, text_xpos, text_ypos, true);

      // highlighted
      m_normalFont.CreateString(tempImage, buttonText.c_str(), 73);

      text_xpos = (signed(c_buttonWidth) - signed(tempImage.GetXRes())) / 2;
      if (text_xpos < 0)
         text_xpos = 0;

      m_buttonImages[2].PasteImage(tempImage, text_xpos, text_ypos, true);
   }

   // create window
   m_image.Create(c_buttonWidth, m_buttonImages[0].GetYRes());
   m_image.SetPalette(game.GetImageManager().GetPalette(3));

   ImageQuad::Init(game, xpos, ypos);

   UpdateButton(false);
}

bool SaveGameButton::ProcessEvent(SDL_Event& event)
{
   // check if mouse left button area
   if (event.type == SDL_MOUSEMOTION && (m_isLeftButtonDown || m_isRightButtonDown))
   {
      unsigned int xpos = 0, ypos = 0;
      CalcMousePosition(event, xpos, ypos);

      UpdateButton(IsInWindow(xpos, ypos));
   }

   return Window::ProcessEvent(event);
}

void SaveGameButton::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   // update button states
   if (buttonClicked)
   {
      if (leftButton) m_isLeftButtonDown = buttonDown;
      else m_isRightButtonDown = buttonDown;
   }

   // update button images
   if (buttonClicked)
      UpdateButton(buttonDown);

   // send "press button" event
   if (buttonClicked && !buttonDown)
      m_screen->PressButton(m_buttonId);
}

void SaveGameButton::UpdateButton(bool buttonStatePressed)
{
   // paste base button
   IndexedImage& button0 = m_buttonImages[0];
   m_image.PasteImage(button0, 0, 0);

   // base border depending on selection
   IndexedImage& button = m_buttonImages[buttonStatePressed ? 1 : 2];

   m_image.PasteImage(button, 0, 0, true);

   Update();
}

void SaveGamesList::Init(SaveGameScreen* screen,
   IGame& game, unsigned int xpos, unsigned int ypos,
   bool showNewSavegameEntry)
{
   m_savegamesManager = &game.GetSavegamesManager();
   m_screen = screen;
   m_showNewSavegameEntry = showNewSavegameEntry;

   m_firstListItemIndex = 0;
   m_selectedSavegameItemIndex = -1;

   // load font
   m_normalFont.Load(game.GetResourceManager(), fontNormal);

   // setup image
   ImageQuad::Init(game, xpos, ypos);

   GetImage().Create(119, 126);
   GetImage().Clear(142);

   Update();
}

void SaveGamesList::UpdateList()
{
   GetImage().Clear(142);

   unsigned int charHeight = m_normalFont.GetCharHeight();

   // show savegames, but only as much entries as fit
   unsigned int numItems = m_savegamesManager->GetSavegamesCount() + (m_showNewSavegameEntry ? 0 : 1);

   unsigned int maxItems = unsigned(GetImage().GetYRes() / charHeight);

   if (numItems - m_firstListItemIndex > maxItems)
      numItems = m_firstListItemIndex + 17;

   for (unsigned int i = m_firstListItemIndex; i < numItems; i++)
   {
      bool selected = int(i) == m_selectedSavegameItemIndex;

      std::string desc;
      if (!m_showNewSavegameEntry && i == numItems - 1)
      {
         // last entry: new game slot
         /// \todo move to strings
         desc.assign("new savegame slot");
      }
      else
      {
         // get savegame info
         Base::SavegameInfo info;
         m_savegamesManager->GetSavegameInfo(i, info);

         std::ostringstream buffer;
         buffer << i + 1 << ". ";

         desc.assign(buffer.str().c_str());
         desc.append(info.m_title);
      }

      // string too long for list field?
      if (m_normalFont.CalcLength(desc.c_str()) > GetImage().GetXRes() - 2)
      {
         desc.append("...");
         std::string::size_type pos = desc.size() - 4;

         do
         {
            // delete one char
            desc.erase(pos, 1);
            pos--;

         } while (pos > 0 && m_normalFont.CalcLength(desc.c_str()) > GetImage().GetXRes() - 2);
      }

      // when selected, fill background
      if (selected)
         GetImage().FillRect(0, i*(charHeight + 1) + 2,
            GetImage().GetXRes(), charHeight, 162);

      // paste image
      IndexedImage tempImage;
      m_normalFont.CreateString(tempImage, desc.c_str(), selected ? 73 : 162);

      GetImage().PasteImage(tempImage, 2, i*(charHeight + 1) + 2, true);
   }

   Update();

   m_screen->UpdateInfo();
}

void SaveGamesList::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && buttonDown)
   {
      // determine
      unsigned int item = (mouseY - m_windowYPos - 2) / (m_normalFont.GetCharHeight() + 1);
      item += m_firstListItemIndex;

      int lastSelectedIndex = m_selectedSavegameItemIndex;

      unsigned int numItems = m_savegamesManager->GetSavegamesCount() + (m_showNewSavegameEntry ? 0 : 1);

      // check if user selected an empty slot
      if (item < numItems)
         m_selectedSavegameItemIndex = item;
      else
      {
         // only deselect when clicked on empty slot
         if (buttonDown)
            m_selectedSavegameItemIndex = -1;
      }

      // update when needed
      if (lastSelectedIndex != m_selectedSavegameItemIndex)
         UpdateList();
   }
}

SaveGameScreen::SaveGameScreen(IGame& game,
   bool calledFromStartMenu, bool disableSaveButton)
   :Screen(game),
   m_calledFromStartMenu(calledFromStartMenu),
   m_disableSaveButton(disableSaveButton)
{
}

void SaveGameScreen::Init()
{
   Screen::Init();

   UaTrace("save game screen started\n");

   m_game.GetRenderer().SetupCamera2D();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   m_showPreview = false;
   m_isEditingDescription = false;
   m_pressedButton = saveGameButtonNone;

   if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
   {
      // we only have the uw_demo, and no character screen
      m_buttonFont.Load(m_game.GetResourceManager(), fontNormal);

      m_game.GetImageManager().LoadList(m_facesImages, "bodies", 0, 0);

      // init background image
      IndexedImage& img = m_backgroundImage.GetImage();
      img.Create(320, 200);
      img.SetPalette(m_game.GetImageManager().GetPalette(3));

      img.Clear(142);
      DrawImageEdges(img, 2, 2, 320 - 4, 200 - 4, true);

      // savegame list edge
      DrawImageEdges(img, 12, 7, 134, 138, true);

      // info area
      DrawImageEdges(img, 172, 7, 134, 27, true);

      DrawImageEdges(img, 172, 42, 63, 77, true);
      DrawImageEdges(img, 243, 42, 63, 77, true);

      // preview image
      DrawImageEdges(img, 172, 127, 134, 63, true);

      m_backgroundImage.Init(m_game, 0, 0);
      m_backgroundImage.Update();

      RegisterWindow(&m_backgroundImage);
   }
   else
   {
      // normal uw1 game
      m_buttonFont.Load(m_game.GetResourceManager(), fontCharacterGeneration);

      m_game.GetImageManager().LoadList(m_facesImages, "chrbtns", 17, 0, 3);

      // scan for savegames
      m_game.GetSavegamesManager().Rescan();

      // load background image
      IndexedImage temp_back;
      m_game.GetImageManager().
         Load(temp_back, "data/chargen.byt", 0, 3, imageByt);

      // prepare background image
      IndexedImage& img = m_backgroundImage.GetImage();

      img.Create(320, 200);
      img.SetPalette(m_game.GetImageManager().GetPalette(3));
      img.PasteRect(temp_back, 160, 0, 160, 200, 0, 0);

      // add frame
      img.PasteRect(temp_back, 11, 124, 138, 6, 10, 4);
      img.PasteRect(temp_back, 10, 130, 6, 56, 9, 10);
      img.PasteRect(temp_back, 10, 130, 6, 56, 9, 66);
      img.PasteRect(temp_back, 10, 130, 6, 19, 9, 122);

      img.PasteRect(temp_back, 144, 130, 6, 56, 143, 10);
      img.PasteRect(temp_back, 144, 130, 6, 56, 143, 66);
      img.PasteRect(temp_back, 144, 130, 6, 19, 143, 122);

      img.PasteRect(temp_back, 10, 186, 140, 6, 9, 141);

      img.PasteRect(temp_back, 0, 0, 160, 200, 159, 0);

      m_backgroundImage.Init(m_game, 0, 0);
      m_backgroundImage.Update();

      RegisterWindow(&m_backgroundImage);
   }

   // init buttons
   m_loadButton.Init(this, m_game, 17, 155, "Load", saveGameButtonLoad);
   m_saveButton.Init(this, m_game, 17, 177, "Save", saveGameButtonSave);
   m_refreshButton.Init(this, m_game, 84, 155, "Refresh", saveGameButtonRefresh);
   m_exitButton.Init(this, m_game, 84, 177, "Exit", saveGameButtonExit);

   if (!m_disableSaveButton)
      RegisterWindow(&m_saveButton);
   RegisterWindow(&m_loadButton);
   RegisterWindow(&m_refreshButton);
   RegisterWindow(&m_exitButton);

   // init info area
   m_infoAreaImage.GetImage().Create(128, 105);
   m_infoAreaImage.GetImage().SetPalette(m_backgroundImage.GetImage().GetPalette());
   m_infoAreaImage.Init(m_game, 160 + 16, 8);

   // init savegames list
   m_savegamesList.Init(this, m_game, 19, 13, m_disableSaveButton);
   m_savegamesList.UpdateList();
   RegisterWindow(&m_savegamesList);

   // init mouse cursor
   m_mouseCursor.Init(m_game, 10);
   m_mouseCursor.Show(true);

   RegisterWindow(&m_mouseCursor);

   // init fadein
   m_fader.Init(true, m_game.GetTickRate(), s_fadeTime);
   m_fadeState = 0;

   // update info area
   UpdateInfo();
}

void SaveGameScreen::Destroy()
{
   m_previewImageTexture.Done();

   UaTrace("leaving save game screen\n\n");
}

bool SaveGameScreen::ProcessEvent(SDL_Event& event)
{
   if (Screen::ProcessEvent(event))
      return true;

   if (m_isEditingDescription)
      if (m_textEdit.ProcessEvent(event))
         return true;

   // user event?
   if (m_isEditingDescription && event.type == SDL_USEREVENT &&
      (event.user.code == gameEventTexteditFinished ||
         event.user.code == gameEventTexteditAborted))
   {
      // end edit mode
      m_isEditingDescription = false;

      m_textEdit.Done();

      if (event.user.code == gameEventTexteditFinished)
         SaveGameToDisk();

      // refresh list
      m_savegamesList.UpdateList();
   }

   switch (event.type)
   {
   case SDL_KEYDOWN: // key was pressed
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         // simulate press on exit button
         if (m_fadeState == 1)
         {
            PressButton(saveGameButtonExit);
         }
         break;

      default:
         break;
      }
      break;

   default:
      break;
   }

   return true;
}

void SaveGameScreen::Draw()
{
   glClear(GL_COLOR_BUFFER_BIT);

   // determine brightness of images
   {
      Uint8 light = m_fader.GetFadeValue();
      glColor3ub(light, light, light);
   }

   // draw registered windows
   Screen::Draw();

   // render edit field when needed
   if (m_isEditingDescription)
      m_textEdit.Draw();

   // render savegame preview image
   if (m_showPreview)
   {
      glEnable(GL_BLEND);
      m_infoAreaImage.Draw();
      glDisable(GL_BLEND);

      m_previewImageTexture.Use();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      double u, v;
      u = m_previewImageTexture.GetTexU();
      v = m_previewImageTexture.GetTexV();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v);   glVertex2i(200, 200 - 133 - 50);
      glTexCoord2d(u, v);   glVertex2i(200 + 80, 200 - 133 - 50);
      glTexCoord2d(u, 0.0); glVertex2i(200 + 80, 200 - 133);
      glTexCoord2d(0.0, 0.0); glVertex2i(200, 200 - 133);
      glEnd();
   }
}

void SaveGameScreen::Tick()
{
   if ((m_fadeState == 0 || m_fadeState == 2) && m_fader.Tick())
   {
      m_fadeState++;

      if (m_fadeState == 3)
      {
         // faded out, do some action
         switch (m_pressedButton)
         {
         case saveGameButtonLoad:
         {
            UaTrace("loading saved game, filename %s\n",
               m_game.GetSavegamesManager().GetSavegameFilename(
                  m_savegamesList.GetSelectedSavegame()).c_str());

            // clear screen; loading takes a while
            m_game.GetRenderer().Clear();

            // load savegame
            Base::Savegame sg = m_game.GetSavegamesManager().GetSavegameFromFile(
               m_game.GetSavegamesManager().GetSavegameFilename(
                  m_savegamesList.GetSelectedSavegame()).c_str());
            m_game.GetUnderworld().Load(sg);

            // next screen
            if (m_calledFromStartMenu)
               m_game.ReplaceScreen(new OriginalIngameScreen(m_game), false);
            else
               m_game.RemoveScreen();
            break;
         }

         case saveGameButtonExit:
            m_game.RemoveScreen();
            break;

         default:
            break;
         }
      }
   }
}

void SaveGameScreen::PressButton(SaveGameButtonId buttonId)
{
   if (m_fadeState != 1)
      return; // don't allow button presses in fadein/fadeout

   m_pressedButton = buttonId;

   switch (buttonId)
   {
      // load button
   case saveGameButtonLoad:
   {
      // check if user tries to load the "new slot" entry
      if (m_savegamesList.GetSelectedSavegame() == -1 ||
         static_cast<unsigned int>(m_savegamesList.GetSelectedSavegame()) >=
         m_game.GetSavegamesManager().GetSavegamesCount())
         break;

      // fade out and do action
      m_fadeState = 2;
      m_fader.Init(false, m_game.GetTickRate(), s_fadeTime);
      m_game.GetAudioManager().FadeoutMusic(static_cast<int>(s_fadeTime * 1000));
   }
   break;

   // refresh button
   case saveGameButtonRefresh:
   {
      // refresh list
      m_game.GetSavegamesManager().Rescan();
      m_savegamesList.UpdateList();
   }
   break;

   // save button
   case saveGameButtonSave:
      if (!m_disableSaveButton)
         AskForSavegameDescription();
      break;

      // exit button
   case saveGameButtonExit:
      m_fadeState = 2;
      m_fader.Init(false, m_game.GetTickRate(), s_fadeTime);
      break;
   }
}

void SaveGameScreen::UpdateInfo()
{
   int selectedSavegameItemIndex = m_savegamesList.GetSelectedSavegame();

   IndexedImage& infoImage = m_infoAreaImage.GetImage();

   // restore original image
   infoImage.Clear(0);

   if (selectedSavegameItemIndex >= 0 &&
      static_cast<unsigned int>(selectedSavegameItemIndex) < m_game.GetSavegamesManager().GetSavegamesCount())
   {
      // get savegame infos
      Base::SavegameInfo info;
      m_game.GetSavegamesManager().GetSavegameInfo(selectedSavegameItemIndex, info);

      // show infos
      IndexedImage tempImage;
      unsigned int textColor = 73;

      // player name
      unsigned int width = m_buttonFont.CalcLength(info.m_title.c_str());
      unsigned int xpos = width > 128 ? 0 : (128 - width) / 2;
      m_buttonFont.CreateString(tempImage, info.m_title.c_str(), 162);
      infoImage.PasteImage(tempImage, xpos, 2, true);
      tempImage.Clear();

      // gender
      std::string text(m_game.GetGameStrings().GetString(2, info.m_gender + 9));
      m_buttonFont.CreateString(tempImage, text.c_str(), textColor);
      infoImage.PasteImage(tempImage, 2, 13, true);
      tempImage.Clear();

      // profession
      text = m_game.GetGameStrings().GetString(2, info.m_profession + 23);
      m_buttonFont.CreateString(tempImage, text.c_str(), textColor);
      infoImage.PasteImage(tempImage, 125 - tempImage.GetXRes(), 13, true);

      // basic attributes
      for (unsigned int i = 0; i < 4; i++)
      {
         // text
         text = m_game.GetGameStrings().GetString(2, i + 17);
         m_buttonFont.CreateString(tempImage, text.c_str(), textColor);
         infoImage.PasteImage(tempImage, 75, 42 + i * 17, true);

         // number
         unsigned int val = 0;
         switch (i)
         {
         case 0: val = info.m_strength; break;
         case 1: val = info.m_dexterity; break;
         case 2: val = info.m_intelligence; break;
         case 3: val = info.m_vitality; break;
         }

         std::ostringstream buffer;
         buffer << val;

         m_buttonFont.CreateString(tempImage, buffer.str().c_str(), textColor);
         infoImage.PasteImage(tempImage, 110, 42 + i * 17, true);
      }

      // appearance
      unsigned int app = info.m_appearance + (info.m_gender == 0 ? 0 : 5);
      infoImage.PasteImage(m_facesImages[app], 13, 39, true);

      /*
      #ifdef HAVE_DEBUG
            // savegame filename
            text = sgmgr->GetSavegameFilename(selectedSavegameItemIndex);
            std::string::size_type pos = text.find_last_of("\\/");
            if (pos != std::string::npos)
               text.erase(0,pos+1);

            m_normalFont.CreateString(temp,text.c_str(),162);
            img_back2.PasteImage(temp,5,192,true);
      #endif
      */
      // do preview image
      m_previewImageTexture.Init();
      m_previewImageTexture.Convert(info.m_imageXRes, info.m_imageYRes, &info.m_imageRGBA[0]);
      m_previewImageTexture.Upload();

      m_showPreview = true;
   }
   else
   {
      // empty info pane, no preview
      m_showPreview = false;
   }

   m_infoAreaImage.Update();
}

void SaveGameScreen::AskForSavegameDescription()
{
   // ask for a savegame name
   std::string savegameName;

   Base::SavegamesManager& sgmgr = m_game.GetSavegamesManager();

   // select new slot when no savegame was selected
   int selectedSavegameItemIndex = m_savegamesList.GetSelectedSavegame();
   if (selectedSavegameItemIndex != -1 && static_cast<unsigned int>(selectedSavegameItemIndex) < sgmgr.GetSavegamesCount())
   {
      // retrieve savegame title
      Base::SavegameInfo info;
      sgmgr.GetSavegameInfo(selectedSavegameItemIndex, info);
      savegameName = info.m_title;
   }
   else
      selectedSavegameItemIndex = sgmgr.GetSavegamesCount();

   /// \todo change
   unsigned int height = 6;
   unsigned int ypos = selectedSavegameItemIndex * (height + 1) + 13 + 2;

   // init text edit window
   m_textEdit.Init(m_game, 19, ypos, 119, 162, 1, 73,
      "", savegameName.c_str(), false);

   m_isEditingDescription = true;
}

void SaveGameScreen::SaveGameToDisk()
{
   // set up savegame info
   Base::SavegameInfo info;
   info.m_title = m_textEdit.GetText();
   info.m_gamePrefix = m_game.GetSettings().GetString(Base::settingGamePrefix);
   m_game.GetUnderworld().GetPlayer().FillSavegamePlayerInfos(info);

   Base::SavegamesManager& sgmgr = m_game.GetSavegamesManager();

   // saving over existing game?
   int selectedSavegameItemIndex = m_savegamesList.GetSelectedSavegame();
   if (selectedSavegameItemIndex != -1 && static_cast<unsigned int>(selectedSavegameItemIndex) < sgmgr.GetSavegamesCount())
   {
      UaTrace("saving game over old savegame slot, filename %s\n",
         sgmgr.GetSavegameFilename(selectedSavegameItemIndex).c_str());

      // saving over selected game
      Base::Savegame sg = sgmgr.SaveSavegame(info, selectedSavegameItemIndex);
      m_game.GetUnderworld().Save(sg);
   }
   else
   {
      UaTrace("saving game to new savegame slot\n");

      // saving to new slot
      Base::Savegame sg = sgmgr.SaveSavegame(info);
      m_game.GetUnderworld().Save(sg);
   }

   sgmgr.Rescan();
}
