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
/// \file CreateCharacterScreen.hpp
/// \brief create character screen
/// \details shows the screen where the user can create a player character for a
/// new game.
//
#pragma once

#include "ImageScreen.hpp"
#include "ImageQuad.hpp"
#include "Font.hpp"
#include "MouseCursor.hpp"
#include "script/CreateCharacterLuaScripting.hpp"

namespace Underworld
{
   class Player;
}

/// \brief create character screen class
class CreateCharacterScreen : public ImageScreen
{
public:
   /// ctor
   CreateCharacterScreen(IGame& game);

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

   // registered lua C functions

   /// performs an action given by the script
   static int cchar_do_action(lua_State* L);

private:
   /// loads and initializes Lua script
   void InitLuaScript();

   /// performs an action given by the script
   void DoAction();

   /// does a button press
   void PressButton(int button);

   /// determines clicked button in current group, -1 if none
   int GetButtonOver();

   /// draws text at a coordinate (xalign: 0=left, 1=center, 2=right), returns width of text in pixels
   unsigned int DrawText(const std::string& str, int x, int y, int xalign = 0, unsigned char color = 0);
   /// draws a text with given text id and string block
   unsigned int DrawText(int strnum, int x, int y, int xalign = 0, unsigned char color = 0, int custstrblock = -1);

   /// draw a number at a coordinate, number is always right-aligned, returns width of text in pixels
   unsigned int DrawNumber(unsigned int num, int x, int y, unsigned char color = 0);

   /// draws a button at a coordinate (xc is horizontal center, y vertical top)
   void DrawButton(int buttontype, bool highlight, int strnum, int xc, int y);

   /// draws a buttongroup
   void DrawButtonGroup();

   /// HandleInputCharacter
   void HandleInputCharacter(char c);

   // virtual methods from ImageScreen

   /// called when fadeout has ended
   virtual void OnFadeOutEnded() override;

private:
   /// lua scripting interface
   CreateCharacterLuaScripting m_lua;

   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// string block for button and label text
   unsigned int strblock = 2;

   /// button and text font
   Font m_font;

   /// start a new game
   bool m_newGame = false;

   /// the player
   Underworld::Player* m_player;

   /// indicates if the mouse button is down
   bool m_isButtonDown = false;

   /// background image
   IndexedImage m_backgroundImage;

   /// image list with buttons
   std::vector<IndexedImage> m_buttonImages;

   /// number of selected button, or -1 if none
   int m_selectedButton = -1;

   /// number of previous button
   int m_previousButton = -1;

   /// input text
   std::string m_inputText;

   /// button images
   std::vector<unsigned char> m_buttonImageIndices;

   /// x-coordinate for center of buttongroup
   unsigned int m_buttonGroupPosX = 240;

   /// palette color index of normal text
   unsigned char m_normalTextColor = 0;

   /// palette color index of highlighted text
   unsigned char m_highlightTextColor = 0;

   /// string number of caption in current button group (0 if none)
   unsigned int m_buttonGroupCaptionStringNumber = 0;

   /// button type of current button group
   unsigned int m_buttonGroupButtonType = 0;

   /// normal button image for current button group
   unsigned char m_buttonGroupNormalButtonImage = 0;

   /// highlighted button image for current button group
   unsigned char m_buttonGroupHighlightButtonImage = 0;

   /// number of buttons in current group
   unsigned int m_buttonGroupButtonCount = 0;

   /// number of buttons per column in current group
   int m_buttonGroupButtonsPerColumn = 1;

   /// array of string numbers for buttons in current group
   std::vector<unsigned int> m_buttonGroupButtonStringNumbers;

   /// countdown time in seconds for timer buttons
   double m_countDownTime = 1.0;

   /// tickcount for countdown timer
   unsigned int m_countDownTickCount = 0;
};
