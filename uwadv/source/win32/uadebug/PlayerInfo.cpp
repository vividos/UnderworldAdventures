//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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
/// \file PlayerInfo.cpp
/// \brief player info docking window
//
#include "stdatl.hpp"
#include "PlayerInfo.hpp"
#include "DebugClient.hpp"

enum T_enPropertyGroup
{
   enGroupPosition = 0,
   enGroupAttributes,
   enGroupSkills
};

BEGIN_PROPERTY_GROUP_LIST(g_aPropertyGroups)
PROPERTY_GROUP(enGroupPosition, PROPERTY_STRING(_T("Position")))
PROPERTY_GROUP(enGroupAttributes, PROPERTY_STRING(_T("Attributes")))
PROPERTY_GROUP(enGroupSkills, PROPERTY_STRING(_T("Skills")))
END_PROPERTY_GROUP_LIST()

enum T_enPropertyItem
{
   enPropPosX = 0x0000,
   enPropPosY,
   enPropPosZ,
   enPropPosAngle,

   enPropAttrGender,
   enPropAttrHandedness,
   enPropAttrAppearance,
   enPropAttrProfession,
   enPropAttrMaplevel,
   enPropAttrStrength,
   enPropAttrDexterity,
   enPropAttrIntelligence,
   enPropAttrVitality,
   enPropAttrMaxVitality,
   enPropAttrMana,
   enPropAttrMaxMana,
   enPropAttrWeariness,
   enPropAttrHungriness,
   enPropAttrPoisoned,
   enPropAttrMentalstate,
   enPropAttrNightvision,
   enPropAttrTalks,
   enPropAttrKills,
   enPropAttrExpLevel,
   enPropAttrExpPoints,
   enPropAttrDifficulty,

   enPropSkillAttack,
   enPropSkillDefense,
   enPropSkillUnarmed,
   enPropSkillSword,
   enPropSkillAxe,
   enPropSkillMace,
   enPropSkillMissile,
   enPropSkillMana,
   enPropSkillLore,
   enPropSkillCasting,
   enPropSkillTraps,
   enPropSkillSearch,
   enPropSkillTrack,
   enPropSkillSneak,
   enPropSkillRepair,
   enPropSkillCharm,
   enPropSkillPicklock,
   enPropSkillAcrobat,
   enPropSkillAppraise,
   enPropSkillSwimming
};


BEGIN_PROPERTY_ITEM_LIST(g_aPropertyItems)
PROPERTY_ITEM(enPropPosX, enGroupPosition, enPropertyTypeText, PROPERTY_STRING(_T("xpos")), PROPERTY_STRING(_T("X tilemap position.")))
PROPERTY_ITEM(enPropPosY, enGroupPosition, enPropertyTypeText, PROPERTY_STRING(_T("ypos")), PROPERTY_STRING(_T("Z tilemap position.")))
PROPERTY_ITEM(enPropPosZ, enGroupPosition, enPropertyTypeText, PROPERTY_STRING(_T("zpos")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropPosAngle, enGroupPosition, enPropertyTypeText, PROPERTY_STRING(_T("Angle")), PROPERTY_STRING(_T("TODO")))

PROPERTY_ITEM(enPropAttrGender, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Gender")), PROPERTY_STRING(_T("Gender; 0 means male, 1 means female.")))
PROPERTY_ITEM(enPropAttrHandedness, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Handedness")), PROPERTY_STRING(_T("Handedness; 0 means left-handedness; 1 means right-handedness.")))
PROPERTY_ITEM(enPropAttrAppearance, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Appearance")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrProfession, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Profession")), PROPERTY_STRING(_T("Map level; 0-based.")))
PROPERTY_ITEM(enPropAttrMaplevel, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Map Level")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrStrength, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Strength")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrDexterity, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Dexterity")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrIntelligence, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Intelligence")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrVitality, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Vitality")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrMaxVitality, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Max. Vitality")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrMana, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Mana")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrMaxMana, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Max. Mana")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrWeariness, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Weariness")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrHungriness, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Hungriness")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrPoisoned, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Poisoned")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrMentalstate, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Mental State")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrNightvision, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Night Vision")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrTalks, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("No. Talks")), PROPERTY_STRING(_T("Number of conversations.")))
PROPERTY_ITEM(enPropAttrKills, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("No. Kills")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrExpLevel, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Exp. Level")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrExpPoints, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Experience")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropAttrDifficulty, enGroupAttributes, enPropertyTypeText, PROPERTY_STRING(_T("Difficulty")), PROPERTY_STRING(_T("Difficulty; 0 means easy, 1 means normal.")))

PROPERTY_ITEM(enPropSkillAttack, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Attack")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillDefense, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Defense")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillUnarmed, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Unarmed")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillSword, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Sword")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillAxe, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Axe")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillMace, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Mace")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillMissile, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Missile")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillMana, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Mana")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillLore, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Lore")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillCasting, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Casting")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillTraps, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Traps")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillSearch, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Search")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillTrack, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Track")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillSneak, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Sneak")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillRepair, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Repair")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillCharm, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Charm")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillPicklock, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Picklock")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillAcrobat, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Acrobat")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillAppraise, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Appraise")), PROPERTY_STRING(_T("TODO")))
PROPERTY_ITEM(enPropSkillSwimming, enGroupSkills, enPropertyTypeText, PROPERTY_STRING(_T("Swimming")), PROPERTY_STRING(_T("TODO")))
END_PROPERTY_ITEM_LIST()

void CPlayerInfoWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch (notify.m_enCode)
   {
   case ncUpdateData:
      UpdateData();
      break;

   case ncSetReadonly:
      m_bReadonly = true;
      m_listCtrl.SetReadonly(true);
      m_listCtrl.SetBkColor(RGB(255, 0, 255));
      break;

   case ncSetReadWrite:
      m_listCtrl.SetReadonly(false);
      m_listCtrl.SetBkColor(RGB(255, 255, 255));
      m_bReadonly = false;
      break;
   }
}

void CPlayerInfoWindow::UpdateData()
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   CDebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

   // update all position info values
   for (unsigned int i = 0; i < 4; i++)
      m_adValues[i] = playerInfo.GetPosInfo(i);

   // update all attributes
   unsigned int max = playerInfo.GetAttrCount();
   for (unsigned int n = 0; n < max; n++)
      m_anValues[n] = playerInfo.GetAttribute(n);

   debugClient.Lock(false);

   m_listCtrl.UpdateValues();
}

LRESULT CPlayerInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
      CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
      CDebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

      for(unsigned int n=0; n<4; n++)
         m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetPosInfoName(n));

      unsigned int max = playerInfo.GetAttrCount();
      for(unsigned int i=0; i<max; i++)
         m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetAttrName(i));

      m_listCtrl.Init(this);
      m_listCtrl.SetColumnEditable(1, true);
   */

   m_listCtrl.Create(m_hWnd, rcDef, NULL);

   m_listCtrl.AddGroups(g_aPropertyGroups);
   m_listCtrl.AddItems(g_aPropertyItems);

   m_listCtrl.SetStorage(this);

   m_listCtrl.Init();

   return 0;
}
/*
void CPlayerInfoWindow::OnUpdatedValue(unsigned int item, unsigned int nSubItem, LPCTSTR pszText)
{
   nSubItem;
   ATLASSERT(nSubItem == 1);
   ATLASSERT(pszText != NULL);

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   CDebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

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

void CPlayerInfoWindow::GetProperty(unsigned int nPropertyItemId, CString& cszProperty)
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   if (nPropertyItemId < 4)
   {
      // position
      cszProperty.Format(_T("%3.2f"), m_adValues[nPropertyItemId]);
   }
   else
   {
      // attributes and skills
      cszProperty.Format(_T("%u"), m_anValues[nPropertyItemId - 4]);
   }

   debugClient.Lock(false);
}

void CPlayerInfoWindow::SetProperty(unsigned int nPropertyItemId, const CString& cszProperty)
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   CDebugClientPlayerInterface playerInfo = debugClient.GetPlayerInterface();

   if (nPropertyItemId < 4)
   {
      // position
      double dValue = ::_tcstod(cszProperty, NULL);
      playerInfo.SetPosInfo(nPropertyItemId, dValue);
   }
   else
   {
      // attributes and skills
      unsigned int nValue = static_cast<unsigned int>(_tcstoul(cszProperty, NULL, 10));
      m_anValues[nPropertyItemId - 4] = nValue;

      playerInfo.SetAttribute(nPropertyItemId - 4, nValue);
   }

   debugClient.Lock(false);
}

bool CPlayerInfoWindow::VerifyProperty(unsigned int nPropertyItemId, const CString& cszProperty)
{
   CString cszCheck(cszProperty);

   // allowed characters
   LPCTSTR pszChars = _T("0123456789");

   if (nPropertyItemId < 4)
      pszChars = _T("0123456789.");

   // remove all valid chars
   int nPos = 0;
   while (-1 != (nPos = cszCheck.FindOneOf(pszChars)))
      cszCheck.Delete(nPos);

   // property is valid when no more chars in string
   return cszCheck.IsEmpty();
}

LRESULT CPlayerInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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

LRESULT CPlayerInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}
