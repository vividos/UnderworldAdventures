//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file GameLogic.hpp
/// \brief game logic
//
#pragma once

#include "Base.hpp"
#include "pch.hpp"
#include "Underworld.hpp"
#include "Properties.hpp"
#include "GameStrings.hpp"
#include "IUserInterface.hpp"

class IScripting;

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   class Object;
   class Level;

   /// game logic class
   class GameLogic
   {
   public:
      /// ctor
      GameLogic(IScripting* scripting);

      /// registers or unregisters user interface
      void RegisterUserInterface(IUserInterface* userInterface)
      {
         m_userInterface = userInterface;
      }

      /// evaluates whole underworld for a given time point
      void EvaluateUnderworld(double time);

      /// user performed an action
      void UserAction(UserInterfaceUserAction action, unsigned int param = 0);

      /// stops or resumes game
      void PauseEvaluation(bool isStopped = true) { m_isStopped = isStopped; }

      /// returns the attack power, ranging from 0 to 100
      unsigned int GetAttackPower() { return m_attackPower; }

      /// returns current player level
      Level& GetCurrentLevel()
      {
         return m_underworld.GetCurrentLevel();
      }

      /// returns current player level; const version
      const Level& GetCurrentLevel() const
      {
         return m_underworld.GetCurrentLevel();
      }

      /// changes current player level
      void ChangeLevel(unsigned int level);

      /// returns game strings
      GameStrings& GetGameStrings()
      {
         return m_gameStrings;
      }

      /// returns object properties
      ObjectProperties& GetObjectProperties() { return m_properties; }
      /// returns object properties; const version
      const ObjectProperties& GetObjectProperties() const { return m_properties; }

      /// returns underworld
      Underworld& GetUnderworld()
      {
         return m_underworld;
      }

   private:
      /// checks if player is near move triggers
      void CheckMoveTrigger();

   protected:
      /// interface to user interface; may be null
      IUserInterface* m_userInterface;

      /// pointer to scripting class
      IScripting* m_scripting;

      /// is true when game is stopped
      bool m_isStopped;

      /// indicates if player is attacking
      bool m_isAttacking;

      /// attack power, ranging from 0 to 100
      unsigned int m_attackPower;

      /// last timestamp that evaluation occured
      double m_lastEvalTime;

      /// set with active triggers
      std::set<Uint16> m_activeTriggers;

      /// object properties
      ObjectProperties m_properties;

      /// game strings
      GameStrings m_gameStrings;

      /// underworld object
      Underworld m_underworld;
   };

} // namespace Underworld
