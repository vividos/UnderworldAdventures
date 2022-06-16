//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file CreateCharacterScreen.cpp
/// \brief character creation screen
///
/// \todo reimplement the whole screen, using own button window class, etc.
/// also use unicode mode to properly capture keys
/// use "relative" indexing for lua C call parameters
/// put global constants as static members into class
//
#include "pch.hpp"
#include "CreateCharacterScreen.hpp"
#include "OriginalIngameScreen.hpp"
#include "Audio.hpp"
#include "Renderer.hpp"
#include "GameStrings.hpp"
#include "Underworld.hpp"
#include "Import.hpp"
#include "ImageManager.hpp"
#include "script/LuaScripting.hpp"
#include <sstream>

extern "C"
{
#include "lua.h"
}

/// time needed to fade in/out screen and music
const double c_fadeTime = 0.5;

/// maximum number of buttons in a group (shown on-screen at once)
const unsigned char c_maxNumButtons = 10;

/// script actions
enum CreateCharLuaAction
{
   actionEnd = 0,                ///< ends the character creation screen (no params)
   actionSetInitValue = 1,       ///< sets init values (param1=stringblock, param2=buttongroup x-coord)
   actionSetUIButtonGroup = 2,   ///< sets the specified button group (param1=heading, param2=buttontype, param3=buttontable)
   actionSetUIText = 3,          ///< sets the specified text at the specified location (param1=stringno, param2=x-coord, param3=y-coord)
   actionSetUICustomText = 4,    ///< sets the specified custom text at the specified location (param1=text, param2=x-coord, param3=y-coord, param4=alignment)
   actionSetUINumber = 5,        ///< sets the specifed number at the specified location
   actionSetUIImage = 6,         ///< sets the specified image at the specified location
   actionUIClear = 7,            ///< clears all screen elements (not the background)
   actionUIUpdate = 8,           ///< updates the screen after a SetUIxxx/UIClear action (no params)
   actionSetPlayerName = 9,      ///< does what the name suggests (param1=name)
   actionSetPlayerAttribute = 10, ///< does what the name suggests (param1=attribute, param2=value)
   actionSetPlayerSkill = 11     ///< does what the name suggests (param1=skill, param2=value)
};

/// custom type for identifying buttons/text
enum CreateCharValue
{
   ccvNone = 0
};

/// custom type for identifying buttons/text
enum CreateCharButtonType
{
   buttonText = 0,   ///< standard button with text from string table
   buttonImage = 1,  ///< square button with image
   buttonInput = 2,  ///< input area
   buttonTimer = 3,  ///< virtual button acting as a countdown timer
};

enum TextAlignTypes
{
   alignLeft = 0,
   alignCenter = 1,
   alignRight = 2,
};

enum EndActionType
{
   endActionCancel = 0,    ///< end screen and go back to start menu
   endActionNewGame = 1,   ///< end screen and go to the in-game view with a new game
};

CreateCharacterScreen::CreateCharacterScreen(IGame& game)
   :ImageScreen(game, 0, c_fadeTime)
{
}

/// \todo replace pplayer with reference to player object
void CreateCharacterScreen::Init()
{
   ImageScreen::Init();

   UaTrace("character creation screen started\n");

   // setup screen
   m_game.GetRenderer().SetupForUserInterface();

   // get a pointer to to current player
   m_player = &(m_game.GetUnderworld().GetPlayer());

   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

   if (!isUw2)
   {
      m_game.GetImageManager().Load(m_backgroundImage, "data/chargen.byt", 0, 3, imageByt);
   }
   else
   {
      m_game.GetImageManager().LoadFromArk(m_backgroundImage, "data/byt.ark", 1, 0);
   }

   m_game.GetImageManager().LoadList(m_buttonImages, "chrbtns", 0, 0, 3);

   // init text
   m_font.Load(m_game.GetResourceManager(), fontCharacterGeneration);

   GetImage() = m_backgroundImage;
   UpdateImage();

   // init mouse cursor
   m_mouseCursor.Init(m_game);
   m_mouseCursor.Show(true);

   RegisterWindow(&m_mouseCursor);

   // intial variable values
   m_newGame = false;
   m_selectedButton = 0;
   m_previousButton = 0;
   m_countDownTickCount = 0;
   m_isButtonDown = false;
   m_stringBlock = 2;
   m_buttonGroupPosX = 240;
   m_buttonGroupCaptionStringNumber = 0;
   m_buttonGroupButtonType = 0;
   m_buttonGroupButtonCount = 0;
   m_buttonGroupNormalButtonImage = 0;
   m_buttonGroupHighlightButtonImage = 0;

   m_buttonGroupButtonStringNumbers.resize(c_maxNumButtons);
   m_buttonImageIndices.resize(5);

   // init lua scripting
   InitLuaScript();
}

void CreateCharacterScreen::Destroy()
{
   ImageScreen::Destroy();

   UaTrace("character creation screen ended\n\n");
}

bool CreateCharacterScreen::ProcessEvent(SDL_Event& event)
{
   ImageScreen::ProcessEvent(event);

   switch (event.type)
   {
   case SDL_KEYDOWN:
      // don't handle any keystroke if Alt or Ctrl is down
      if (((event.key.keysym.mod & KMOD_ALT) > 0) || ((event.key.keysym.mod & KMOD_CTRL) > 0))
         return false;

      // handle key presses
      if ((m_buttonGroupButtonType == buttonInput) &&
         (((event.key.keysym.sym >= SDLK_a) && (event.key.keysym.sym <= SDLK_z)) ||
            ((event.key.keysym.sym >= SDLK_0) && (event.key.keysym.sym <= SDLK_9)) ||
            (event.key.keysym.sym == SDLK_BACKSPACE) ||
            (event.key.keysym.sym == SDLK_SPACE) ||
            (event.key.keysym.sym == SDLK_MINUS) ||
            (event.key.keysym.sym == SDLK_UNDERSCORE)))
      {
         char c = event.key.keysym.sym;
         if (((event.key.keysym.mod & KMOD_SHIFT) > 0) && ((c >= 'a') && (c <= 'z')))
            c -= 32;
         HandleInputCharacter(c);
         DrawButtonGroup();
         UpdateImage();
      }

      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         m_lua.Cancel();
         break;

      case SDLK_PAGEUP:
      case SDLK_HOME:
         m_selectedButton = 0;
         break;

      case SDLK_PAGEDOWN:
      case SDLK_END:
         m_selectedButton = m_buttonGroupButtonCount - 1;
         break;

      case SDLK_UP:
         // select the button above, if possible
         if (m_selectedButton == -1) m_selectedButton = 0;
         if (m_selectedButton > 0) m_selectedButton--;
         break;

      case SDLK_DOWN:
         // select the button below, if possible
         if (m_selectedButton + 1 < static_cast<int>(m_buttonGroupButtonCount))
            m_selectedButton++;
         break;

      case SDLK_RIGHT:
         // select the button to the right, if possible
         if (m_selectedButton + m_buttonGroupButtonsPerColumn < static_cast<int>(m_buttonGroupButtonCount))
            m_selectedButton += m_buttonGroupButtonsPerColumn;
         break;

      case SDLK_LEFT:
         // select the button to the right, if possible
         if (m_selectedButton - m_buttonGroupButtonsPerColumn >= 0)
            m_selectedButton -= m_buttonGroupButtonsPerColumn;
         break;

      case SDLK_KP_ENTER:
      case SDLK_RETURN:
         // simulate clicking on that area
         PressButton(m_selectedButton);
         break;

      default: break;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      // select the area where the mouse button is pressed
      m_isButtonDown = true;
      {
         int ret = GetButtonOver();
         if (ret >= 0)
            m_selectedButton = ret;
      }
      break;

   case SDL_MOUSEMOTION:
   {
      if (m_isButtonDown)
      {
         int ret = GetButtonOver();
         if (ret >= 0)
            m_selectedButton = ret;
      }
   }
   break;

   case SDL_MOUSEBUTTONUP:
      m_isButtonDown = false;
      {
         // determine if user released the mouse button over the same area
         int ret = GetButtonOver();
         if ((ret >= 0) && (ret == m_selectedButton))
            PressButton(m_selectedButton);
      }
      break;

   default:
      break;
   }

   if (m_selectedButton != m_previousButton)
   {
      DrawButtonGroup();
      m_previousButton = m_selectedButton;
      UpdateImage();
   }

   return false;
}

void CreateCharacterScreen::Tick()
{
   ImageScreen::Tick();

   if ((m_buttonGroupButtonType == buttonTimer) &&
      (++m_countDownTickCount >= (GetTickRate() * m_countDownTime)))
   {
      m_countDownTickCount = 0;
      PressButton(0);
   }
}

void CreateCharacterScreen::OnFadeOutEnded()
{
   if (m_newGame)
   {
      // load initial game
      Import::LoadUnderworld(
         m_game.GetSettings(),
         m_game.GetResourceManager(),
         m_game.GetUnderworld());

      // init new game
      m_game.GetScripting().InitNewGame();

      // start original game
      m_game.ReplaceScreen(new OriginalIngameScreen(m_game), false);
   }
   else
      m_game.RemoveScreen();
}

void CreateCharacterScreen::InitLuaScript()
{
   m_lua.SetActionHandler(std::bind(&CreateCharacterScreen::DoAction, this));

   bool hasError = false;

   // load lua interface script for constants
   if (!LuaScripting::LoadScript(m_lua, m_game.GetSettings(), m_game.GetResourceManager(), "uw1/scripts/uwinterface"))
      hasError = true;

   // load lua cutscene script
   if (!LuaScripting::LoadScript(m_lua, m_game.GetSettings(), m_game.GetResourceManager(), "uw1/scripts/createchar"))
      hasError = true;

   // starts character creation
   if (!m_lua.Start())
      hasError = true;

   if (hasError)
      StartFadeout();
}

void CreateCharacterScreen::HandleInputCharacter(char c)
{
   size_t i = m_inputText.size();
   if (c == 8)
   {
      if (i > 0)
         m_inputText.erase(i - 1);
      return;
   }

   if (i >= 31)
      return;
   m_inputText += c;
}

void CreateCharacterScreen::DoAction()
{
   lua_State* L = m_lua.GetLuaState();

   int n = lua_gettop(L);
   unsigned int action = (n < 1) ? 0 : static_cast<unsigned int>(lua_tonumber(L, 1));

   IndexedImage& img = GetImage();

   switch (static_cast<CreateCharLuaAction>(action))
   {
   case actionEnd:
      m_newGame = (n > 1) && (static_cast<unsigned int>(lua_tonumber(L, 2)) == 1);

      if (m_newGame)
      {
         // fade out music
         m_game.GetAudioManager().FadeoutMusic(static_cast<int>(c_fadeTime * 1000));
      }

      StartFadeout();

      //UaTrace("end request by char. creation script\n");
      break;

   case actionSetInitValue:
   {
      if (n < 5) break;
      m_stringBlock = static_cast<unsigned int>(lua_tonumber(L, 2));
      m_buttonGroupPosX = static_cast<unsigned int>(lua_tonumber(L, 3));
      m_normalTextColor = static_cast<unsigned int>(lua_tonumber(L, 4));
      m_highlightTextColor = static_cast<unsigned int>(lua_tonumber(L, 5));

      // set different highlight color when features are enabled
      if (m_game.GetSettings().GetBool(Base::settingUwadvFeatures))
         m_highlightTextColor = 162; // orange, palette #3

      size_t ic = lua_rawlen(L, 6);
      if (ic > 5) ic = 5;
      for (size_t i = 0; i < ic; i++)
      {
         lua_rawgeti(L, 6, i + 1);
         m_buttonImageIndices[i] = static_cast<unsigned int>(lua_tonumber(L, 7));
         lua_pop(L, 1);
      }
      //UaTrace("init values set by char. creation script\n");
      break;
   }

   case actionSetUIButtonGroup:
   {
      if (n < 3) break;
      m_buttonGroupCaptionStringNumber = static_cast<unsigned int>(lua_tonumber(L, 2));
      m_buttonGroupButtonType = static_cast<unsigned int>(lua_tonumber(L, 3));
      if (m_buttonGroupButtonType == buttonTimer)
      {
         m_countDownTime = m_buttonGroupCaptionStringNumber / 1000.0;
         m_countDownTickCount = 0;
         m_buttonGroupCaptionStringNumber = 0;
         m_buttonGroupButtonCount = m_buttonGroupButtonsPerColumn = 1;
      }
      else
      {
         m_buttonGroupButtonCount = lua_rawlen(L, 4);
         if ((m_buttonGroupButtonType == buttonInput) && (m_buttonGroupButtonCount > 0))
         {
            m_inputText.erase();
            m_buttonGroupButtonCount = 1;
         }
         for (int i = 0; i < static_cast<int>(m_buttonGroupButtonCount); i++)
         {
            lua_rawgeti(L, 4, i + 1);
            m_buttonGroupButtonStringNumbers[i] = static_cast<unsigned int>(lua_tonumber(L, 5));
            lua_pop(L, 1);
         }

         switch (m_buttonGroupButtonType)
         {
         case buttonText:
            m_buttonGroupNormalButtonImage = m_buttonImageIndices[0];
            m_buttonGroupHighlightButtonImage = m_buttonImageIndices[1];
            break;
         case buttonImage:
            m_buttonGroupNormalButtonImage = m_buttonImageIndices[2];
            m_buttonGroupHighlightButtonImage = m_buttonImageIndices[3];
            break;
         case buttonInput:
            m_buttonGroupNormalButtonImage = m_buttonImageIndices[4];
            break;
         }
         m_buttonGroupButtonsPerColumn = (m_buttonGroupButtonCount > 8) ? m_buttonGroupButtonCount / 2 : m_buttonGroupButtonCount;
      }
      m_selectedButton = m_previousButton = 0;
      DrawButtonGroup();
      //UaTrace("new buttongroup set by char. creation script, caption nr: %d, %d buttons\n", m_buttonGroupCaptionStringNumber, m_buttonGroupButtonCount);
      break;
   }

   case actionSetUIText:
   {
      if (n < 5) break;
      DrawText(static_cast<unsigned int>(lua_tonumber(L, 2)),
         static_cast<unsigned int>(lua_tonumber(L, 3)),
         static_cast<unsigned int>(lua_tonumber(L, 4)),
         static_cast<unsigned int>(lua_tonumber(L, 5)),
         m_normalTextColor,
         (n > 5) ? static_cast<unsigned int>(lua_tonumber(L, 6)) : unsigned(-1));
      break;
   }

   case actionSetUICustomText:
   {
      if (n < 5) break;
      DrawText(lua_tostring(L, 2),
         static_cast<unsigned int>(lua_tonumber(L, 3)),
         static_cast<unsigned int>(lua_tonumber(L, 4)),
         static_cast<unsigned int>(lua_tonumber(L, 5)));
      break;
   }

   case actionSetUINumber:
   {
      if (n < 4) break;
      DrawNumber(static_cast<unsigned int>(lua_tonumber(L, 2)),
         static_cast<unsigned int>(lua_tonumber(L, 3)),
         static_cast<unsigned int>(lua_tonumber(L, 4)));
      break;
   }

   case actionSetUIImage:
   {
      if (n < 4) break;
      IndexedImage& cimg = m_buttonImages[static_cast<unsigned int>(lua_tonumber(L, 2))];
      unsigned int destx, desty;
      destx = static_cast<unsigned int>(lua_tonumber(L, 3)) - cimg.GetXRes() / 2;
      desty = static_cast<unsigned int>(lua_tonumber(L, 4)) - cimg.GetYRes() / 2;
      img.PasteRect(cimg, 0, 0, cimg.GetXRes(), cimg.GetYRes(), destx, desty, true);
      break;
   }

   case actionUIClear:
      img.PasteImage(m_backgroundImage, 0, 0);
      //UaTrace("buffered screen cleared by char. creation script\n");
      break;

   case actionUIUpdate:
      UpdateImage();
      //UaTrace("screen updated by char. creation script\n");
      break;

   case actionSetPlayerName:
      if (n < 2) break;
      m_player->SetName(lua_tostring(L, 2));
      //UaTrace("player name set to \"%s\" by char. creation script\n", lua_tostring(L,2));
      break;

   case actionSetPlayerAttribute:
      if (n < 3) break;
      m_player->SetAttribute(static_cast<Underworld::PlayerAttribute>(static_cast<unsigned int>(lua_tonumber(L, 2))),
         static_cast<unsigned int>(lua_tonumber(L, 3)));
      //UaTrace("player attribute set\n");
      break;

   case actionSetPlayerSkill:
      if (n < 3) break;
      m_player->SetSkill(static_cast<Underworld::PlayerSkill>(static_cast<unsigned int>(lua_tonumber(L, 2))),
         static_cast<unsigned int>(lua_tonumber(L, 3)));
      //UaTrace("player skill set\n");
      break;

   default:
      //UaTrace("unknown action (#%d) requested by by char. creation script\n", action);
      break;
   }
}

unsigned int CreateCharacterScreen::DrawText(const std::string& str, int x, int y, int xalign, unsigned char color)
{
   // set default text color
   if (color == 0)
      color = m_normalTextColor;

   IndexedImage textImage;
   textImage.Clear(0);

   unsigned int textlength = m_font.CalcLength(str);
   m_font.CreateString(textImage, str, color);

   if (xalign != 0)
   {
      // adjust horizontal alignment
      if (xalign == 1)
         x -= textlength / 2;
      else if (xalign == 2)
         x -= textlength;
   }

   if (textImage.GetXRes() > 0)
   {
      GetImage().PasteRect(textImage, 0, 0, textImage.GetXRes(), textImage.GetYRes(), x, y, true);
   }

   return textlength;
}

unsigned int CreateCharacterScreen::DrawNumber(unsigned int num, int x, int y, unsigned char color)
{
   std::ostringstream buffer;
   buffer << num;
   return DrawText(buffer.str(), x, y, 2, color);
}

unsigned int CreateCharacterScreen::DrawText(int stringNumber, int x, int y, int xalign, unsigned char color, int customStringBlock)
{
   std::string text =
      m_game.GetGameStrings().GetString(
         customStringBlock > -1 ? customStringBlock : m_stringBlock,
         stringNumber);

   return !text.empty() ? DrawText(text, x, y, xalign, color) : 0;
}

void CreateCharacterScreen::DrawButton(int buttontype, bool highlight, int stringNumber, int xc, int y)
{
   IndexedImage button = m_buttonImages[m_buttonGroupNormalButtonImage];
   int x = xc - button.GetXRes() / 2;

   GetImage().PasteImage(button, x, y);

   if (buttontype == buttonText)
      DrawText(stringNumber, xc, y + 3, 1, (highlight ? m_highlightTextColor : m_normalTextColor));
   else if (buttontype == buttonInput)
   {
      unsigned int labelwidth = DrawText(stringNumber, x + 4, y + 3, 0, m_normalTextColor);
      unsigned int maxnamewidth = button.GetXRes() - labelwidth - 7;
      size_t ip = m_inputText.size();
      while ((m_font.CalcLength(m_inputText) > maxnamewidth) && (ip > 0))
         m_inputText.erase(ip--);
      DrawText(m_inputText, x + labelwidth + 4, y + 3, 0, m_highlightTextColor);
   }
   else if (buttontype == buttonImage)
   {
      button = m_buttonImages[stringNumber];
      GetImage().PasteRect(button, 0, 0, button.GetXRes(), button.GetYRes(), x, y, true);
   }
   if (highlight && (buttontype != buttonInput))
   {
      button = m_buttonImages[m_buttonGroupHighlightButtonImage];
      GetImage().PasteRect(button, 0, 0, button.GetXRes(), button.GetYRes(), x, y, true);
   }
}

void CreateCharacterScreen::DrawButtonGroup()
{
   if (m_buttonGroupButtonType == buttonTimer)
      return;

   IndexedImage& button = m_buttonImages[m_buttonGroupNormalButtonImage];
   int inity = m_buttonGroupButtonsPerColumn * button.GetYRes() + ((m_buttonGroupButtonsPerColumn - 1) * 5);
   if (m_buttonGroupCaptionStringNumber != ccvNone)
      inity += 15;
   inity = (200 - inity) / 2;
   if (m_buttonGroupCaptionStringNumber != ccvNone)
   {
      DrawText(m_buttonGroupCaptionStringNumber, m_buttonGroupPosX, inity, 1);
      inity += 15;
   }

   int y = inity;
   int x = m_buttonGroupPosX;
   int iex = m_buttonGroupButtonsPerColumn;
   if (iex != static_cast<int>(m_buttonGroupButtonCount))
      x -= button.GetXRes() / 2 + 3;
   else
      iex = -1;
   for (int i = 0; i < static_cast<int>(m_buttonGroupButtonCount); i++)
   {
      DrawButton(m_buttonGroupButtonType, (m_selectedButton == i), m_buttonGroupButtonStringNumbers[i], x, y);
      if (i == iex - 1)
      {
         y = inity;
         x += button.GetXRes() + 6;
      }
      else
         y += button.GetYRes() + 5;
   }
}

int CreateCharacterScreen::GetButtonOver()
{
   if (m_buttonGroupButtonType == buttonTimer)
      return -1;

   // convert to 320x200 screen coordinates
   int xpos, ypos;
   SDL_GetMouseState(&xpos, &ypos);
   MapWindowPosition(xpos, ypos);

   // determine column
   int columns = m_buttonGroupButtonCount / m_buttonGroupButtonsPerColumn;
   int btnsize = m_buttonImages[m_buttonGroupNormalButtonImage].GetXRes();
   int bgsize = columns * btnsize + (columns - 1) * 6;
   xpos -= m_buttonGroupPosX - (bgsize / 2);
   if ((xpos < 0) || (xpos > bgsize) || (xpos % (btnsize + 6) > btnsize))
      return -1;
   int coffset = xpos / (btnsize + 6) * m_buttonGroupButtonsPerColumn;

   // determine button in column
   btnsize = m_buttonImages[m_buttonGroupNormalButtonImage].GetYRes();
   bgsize = m_buttonGroupButtonsPerColumn * btnsize + ((m_buttonGroupButtonsPerColumn - 1) * 5);
   int mv = (m_buttonGroupCaptionStringNumber != ccvNone) ? 15 : 0;
   ypos -= (200 - (bgsize + mv)) / 2 + mv;
   return (ypos >= 0) && (ypos <= bgsize) && (ypos % (btnsize + 5) <= btnsize) ?
      ypos / (btnsize + 5) + coffset : -1;
}

void CreateCharacterScreen::PressButton(int button)
{
   //UaTrace("character creation button %d pressed\n", button);

   lua_State* L = m_lua.GetLuaState();

   // call "cchar_buttonclick(button)"
   lua_getglobal(L, "cchar_buttonclick");
   lua_pushnumber(L, button);

   bool ret;
   if (m_buttonGroupButtonType == buttonInput)
   {
      lua_pushstring(L, m_inputText.c_str());
      ret = m_lua.CheckedCall(2, 0);
   }
   else
      ret = m_lua.CheckedCall(1, 0);

   if (!ret)
      StartFadeout();
}
