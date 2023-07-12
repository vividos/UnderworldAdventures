//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2005,2019,2022,2023 Underworld Adventures Team
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
/// \file TraceOnlyUserInterface.hpp
/// \brief user interface implementation that just traces calls
//
#pragma once

#include "common.hpp"
#include "IUserInterface.hpp"

/// user interface implementation that just traces calls
class TraceOnlyUserInterface : public IUserInterface
{
public:
   // virtual IUserInterface methods

   /// notifies callback class
   virtual void Notify(UserInterfaceNotification notify,
      unsigned int param = 0) override
   {
      UaTrace("Notify: %u, param=%u\n", notify, param);
   }

   /// called to print text to textscroll
   virtual void PrintScroll(const char* text) override
   {
      UaTrace("Print: %s\n", text);
   }

   /// starts conversation with object in current level, on list position
   virtual void StartConversation(Uint16 listPos) override
   {
      UaTrace("StartConversation: listPos=%04x\n", listPos);
   }

   /// starts showing cutscene with given number
   virtual void ShowCutscene(unsigned int cutsceneNumber) override
   {
      UaTrace("ShowCutscene: number=%u\n", cutsceneNumber);
   }

   /// shows map
   virtual void ShowMap() override
   {
      UaTrace("ShowMap\n");
   }
};
