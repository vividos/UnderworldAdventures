//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
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
/// \file PlayerInfoWindow.cpp
/// \brief player info docking window
//
#include "stdatl.hpp"
#include "PlayerInfoWindow.hpp"
#include "DebugClient.hpp"

enum PropertyGroup
{
   groupPosition = 0,
   groupAttributes,
   groupSkills
};

BEGIN_PROPERTY_GROUP_LIST(g_propertyGroups)
PROPERTY_GROUP(groupPosition, PROPERTY_STRING(_T("Position")))
PROPERTY_GROUP(groupAttributes, PROPERTY_STRING(_T("Attributes")))
PROPERTY_GROUP(groupSkills, PROPERTY_STRING(_T("Skills")))
END_PROPERTY_GROUP_LIST()

enum PropertyItem
{
   propertyPosX = 0x0000,
   propertyPosY,
   propertyPosZ,
   propertyPosAngle,

   propertyAttrGender,
   propertyAttrHandedness,
   propertyAttrAppearance,
   propertyAttrProfession,
   propertyAttrMaplevel,
   propertyAttrStrength,
   propertyAttrDexterity,
   propertyAttrIntelligence,
   propertyAttrVitality,
   propertyAttrMaxVitality,
   propertyAttrMana,
   propertyAttrMaxMana,
   propertyAttrWeariness,
   propertyAttrHungriness,
   propertyAttrPoisoned,
   propertyAttrMentalstate,
   propertyAttrNightvision,
   propertyAttrTalks,
   propertyAttrKills,
   propertyAttrExpLevel,
   propertyAttrExpPoints,
   propertyAttrDifficulty,

   propertySkillAttack,
   propertySkillDefense,
   propertySkillUnarmed,
   propertySkillSword,
   propertySkillAxe,
   propertySkillMace,
   propertySkillMissile,
   propertySkillMana,
   propertySkillLore,
   propertySkillCasting,
   propertySkillTraps,
   propertySkillSearch,
   propertySkillTrack,
   propertySkillSneak,
   propertySkillRepair,
   propertySkillCharm,
   propertySkillPicklock,
   propertySkillAcrobat,
   propertySkillAppraise,
   propertySkillSwimming
};


BEGIN_PROPERTY_ITEM_LIST(g_propertyItems)
PROPERTY_ITEM(propertyPosX, groupPosition, propertyTypeText, PROPERTY_STRING(_T("xpos")), PROPERTY_STRING(_T("X tilemap position.")))
PROPERTY_ITEM(propertyPosY, groupPosition, propertyTypeText, PROPERTY_STRING(_T("ypos")), PROPERTY_STRING(_T("Y tilemap position.")))
PROPERTY_ITEM(propertyPosZ, groupPosition, propertyTypeText, PROPERTY_STRING(_T("zpos")), PROPERTY_STRING(_T("Z timemap position.")))
PROPERTY_ITEM(propertyPosAngle, groupPosition, propertyTypeText, PROPERTY_STRING(_T("Angle")), PROPERTY_STRING(_T("View angle.")))

PROPERTY_ITEM(propertyAttrGender, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Gender")), PROPERTY_STRING(_T("Gender; 0 means male, 1 means female.")))
PROPERTY_ITEM(propertyAttrHandedness, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Handedness")), PROPERTY_STRING(_T("Handedness; 0 means left-handedness; 1 means right-handedness.")))
PROPERTY_ITEM(propertyAttrAppearance, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Appearance")), PROPERTY_STRING(_T("Appearance; values from 0 to 4 for different portraits.")))
PROPERTY_ITEM(propertyAttrProfession, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Profession")), PROPERTY_STRING(_T("Profession; value from 0 to 7")))
PROPERTY_ITEM(propertyAttrMaplevel, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Map Level")), PROPERTY_STRING(_T("Map level; 0-based.")))
PROPERTY_ITEM(propertyAttrStrength, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Strength")), PROPERTY_STRING(_T("Strength attribute value.")))
PROPERTY_ITEM(propertyAttrDexterity, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Dexterity")), PROPERTY_STRING(_T("Dexterity attribute value.")))
PROPERTY_ITEM(propertyAttrIntelligence, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Intelligence")), PROPERTY_STRING(_T("Intelligence attribute value.")))
PROPERTY_ITEM(propertyAttrVitality, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Vitality")), PROPERTY_STRING(_T("Current vitality value.")))
PROPERTY_ITEM(propertyAttrMaxVitality, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Max. Vitality")), PROPERTY_STRING(_T("Maximum vitality value.")))
PROPERTY_ITEM(propertyAttrMana, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Mana")), PROPERTY_STRING(_T("Current mana value.")))
PROPERTY_ITEM(propertyAttrMaxMana, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Max. Mana")), PROPERTY_STRING(_T("Maximum mana value.")))
PROPERTY_ITEM(propertyAttrWeariness, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Weariness")), PROPERTY_STRING(_T("Weariness level.")))
PROPERTY_ITEM(propertyAttrHungriness, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Hungriness")), PROPERTY_STRING(_T("Hungriness level.")))
PROPERTY_ITEM(propertyAttrPoisoned, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Poisoned")), PROPERTY_STRING(_T("Poisoned flag.")))
PROPERTY_ITEM(propertyAttrMentalstate, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Mental State")), PROPERTY_STRING(_T("Mental state value.")))
PROPERTY_ITEM(propertyAttrNightvision, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Night Vision")), PROPERTY_STRING(_T("Night vision value.")))
PROPERTY_ITEM(propertyAttrTalks, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("No. Talks")), PROPERTY_STRING(_T("Number of conversations so far.")))
PROPERTY_ITEM(propertyAttrKills, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("No. Kills")), PROPERTY_STRING(_T("Number of kills so far.")))
PROPERTY_ITEM(propertyAttrExpLevel, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Exp. Level")), PROPERTY_STRING(_T("Current experience level")))
PROPERTY_ITEM(propertyAttrExpPoints, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Experience")), PROPERTY_STRING(_T("Current experience points")))
PROPERTY_ITEM(propertyAttrDifficulty, groupAttributes, propertyTypeText, PROPERTY_STRING(_T("Difficulty")), PROPERTY_STRING(_T("Difficulty; 0 means easy, 1 means normal.")))

PROPERTY_ITEM(propertySkillAttack, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Attack")), PROPERTY_STRING(_T("Attack skill value.")))
PROPERTY_ITEM(propertySkillDefense, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Defense")), PROPERTY_STRING(_T("Defense skill value.")))
PROPERTY_ITEM(propertySkillUnarmed, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Unarmed")), PROPERTY_STRING(_T("Unarmed combat skill value.")))
PROPERTY_ITEM(propertySkillSword, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Sword")), PROPERTY_STRING(_T("Sword skill value.")))
PROPERTY_ITEM(propertySkillAxe, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Axe")), PROPERTY_STRING(_T("Axe skill value.")))
PROPERTY_ITEM(propertySkillMace, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Mace")), PROPERTY_STRING(_T("Mace skill value.")))
PROPERTY_ITEM(propertySkillMissile, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Missile")), PROPERTY_STRING(_T("Missile weapon skill value.")))
PROPERTY_ITEM(propertySkillMana, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Mana")), PROPERTY_STRING(_T("Mana skill value.")))
PROPERTY_ITEM(propertySkillLore, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Lore")), PROPERTY_STRING(_T("Lore skill value.")))
PROPERTY_ITEM(propertySkillCasting, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Casting")), PROPERTY_STRING(_T("Casting skill value.")))
PROPERTY_ITEM(propertySkillTraps, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Traps")), PROPERTY_STRING(_T("Traps skill value.")))
PROPERTY_ITEM(propertySkillSearch, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Search")), PROPERTY_STRING(_T("Search skill value.")))
PROPERTY_ITEM(propertySkillTrack, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Track")), PROPERTY_STRING(_T("Track skill value.")))
PROPERTY_ITEM(propertySkillSneak, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Sneak")), PROPERTY_STRING(_T("Sneak skill value.")))
PROPERTY_ITEM(propertySkillRepair, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Repair")), PROPERTY_STRING(_T("Repair skill value.")))
PROPERTY_ITEM(propertySkillCharm, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Charm")), PROPERTY_STRING(_T("Charm skill value.")))
PROPERTY_ITEM(propertySkillPicklock, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Picklock")), PROPERTY_STRING(_T("Picklock skill value.")))
PROPERTY_ITEM(propertySkillAcrobat, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Acrobat")), PROPERTY_STRING(_T("Acrobat skill value.")))
PROPERTY_ITEM(propertySkillAppraise, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Appraise")), PROPERTY_STRING(_T("Appraise skill value.")))
PROPERTY_ITEM(propertySkillSwimming, groupSkills, propertyTypeText, PROPERTY_STRING(_T("Swimming")), PROPERTY_STRING(_T("Swimming skill value.")))
END_PROPERTY_ITEM_LIST()

void PlayerInfoWindow::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUpdateData:
      UpdateData();
      break;

   case notifyCodeSetReadonly:
      m_isReadOnly = true;
      m_listCtrl.SetReadOnly(true);
      m_listCtrl.SetBkColor(RGB(255, 0, 255));
      break;

   case notifyCodeSetReadWrite:
      m_listCtrl.SetReadOnly(false);
      m_listCtrl.SetBkColor(RGB(255, 255, 255));
      m_isReadOnly = false;
      break;
   }
}

void PlayerInfoWindow::UpdateData()
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   DebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

   // update all position info values
   for (unsigned int i = 0; i < 4; i++)
      m_doubleValues[i] = playerInfo.GetPosInfo(i);

   // update all attributes
   unsigned int max = playerInfo.GetAttrCount();
   for (unsigned int n = 0; n < max; n++)
      m_intValues[n] = playerInfo.GetAttribute(n);

   debugClient.Lock(false);

   m_listCtrl.UpdateValues();
}

LRESULT PlayerInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up list control
   CRect rcDef;
   GetClientRect(rcDef);
   /*
      m_listCtrl.Create(m_hWnd, rcDef, NULL, WS_CHILD | WS_VISIBLE |
         LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER | LVS_SINGLESEL);

      m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

      m_listCtrl.InsertColumn(0, (LPCTSTR)_T("Attribute"), LVCFMT_LEFT, 90, -1);
      m_listCtrl.InsertColumn(1, (LPCTSTR)_T("Value"), LVCFMT_LEFT, 90, -1);

      // insert all text descriptions
      DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
      DebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

      for(unsigned int n=0; n<4; n++)
         m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetPosInfoName(n));

      unsigned int max = playerInfo.GetAttrCount();
      for(unsigned int i=0; i<max; i++)
         m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetAttrName(i));

      m_listCtrl.Init(this);
      m_listCtrl.SetColumnEditable(1, true);
   */

   m_listCtrl.Create(m_hWnd, rcDef, NULL);

   m_listCtrl.AddGroups(g_propertyGroups);
   m_listCtrl.AddItems(g_propertyItems);

   m_listCtrl.SetStorage(this);

   m_listCtrl.Init();

   return 0;
}
/*
void PlayerInfoWindow::OnUpdatedValue(unsigned int item, unsigned int nSubItem, LPCTSTR pszText)
{
   nSubItem;
   ATLASSERT(nSubItem == 1);
   ATLASSERT(pszText != NULL);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   DebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

   if (item < 4)
   {
      // position info
      double d = _tcstod(pszText, NULL);
      playerInfo.SetPosInfo(item, d);
   }
   else
   {
      // attribute
      unsigned long nVal = _tcstoul(pszText, NULL, 10);
      playerInfo.SetAttribute(item-4, nVal);
   }

   debugClient.Lock(false);
}
*/

void PlayerInfoWindow::GetProperty(unsigned int propertyItemId, CString& propertyValue)
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   if (propertyItemId < 4)
   {
      // position
      propertyValue.Format(_T("%3.2f"), m_doubleValues[propertyItemId]);
   }
   else
   {
      // attributes and skills
      propertyValue.Format(_T("%u"), m_intValues[propertyItemId - 4]);
   }

   debugClient.Lock(false);
}

void PlayerInfoWindow::SetProperty(unsigned int propertyItemId, const CString& propertyValue)
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   DebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

   if (propertyItemId < 4)
   {
      // position
      double dValue = ::_tcstod(propertyValue, NULL);
      playerInfo.SetPosInfo(propertyItemId, dValue);
   }
   else
   {
      // attributes and skills
      unsigned int value = static_cast<unsigned int>(_tcstoul(propertyValue, NULL, 10));
      m_intValues[propertyItemId - 4] = value;

      playerInfo.SetAttribute(propertyItemId - 4, value);
   }

   debugClient.Lock(false);
}

bool PlayerInfoWindow::VerifyProperty(unsigned int propertyItemId, const CString& propertyValue)
{
   CString check(propertyValue);

   // allowed characters
   LPCTSTR pszChars = _T("0123456789");

   if (propertyItemId < 4)
      pszChars = _T("0123456789.");

   // remove all valid chars
   int pos = 0;
   while (-1 != (pos = check.FindOneOf(pszChars)))
      check.Delete(pos);

   // property is valid when no more chars in string
   return check.IsEmpty();
}

LRESULT PlayerInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition list control
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }
   bHandled = FALSE;
   return 1;
}

LRESULT PlayerInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}
