// Property List Control
// Copyright (C) 2005 Michael Fink
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

// needed includes
#include "stdatl.hpp"
#include "PropertyListCtrl.hpp"

using namespace nsPropertyList;

// CInplaceTextEditControl methods

void CInplaceTextEditControl::Create(HWND hWnd, CRect rect, LPCTSTR pszPropertyValue)
{
   rect.top++;

   // create
   BaseClass::Create(hWnd, rect, _T(""), WS_VISIBLE | WS_CHILD);
   SetWindowText(pszPropertyValue);

   SetFocus();
   SetSel(0,-1);

   // set font of owner
   HFONT hFont = CWindow(hWnd).GetFont();
   SetFont(hFont);
}

bool CInplaceTextEditControl::AcceptChanges()
{
   // get text
   CString cszText;
   int nLength = GetWindowTextLength();
   CWindow::GetWindowText(cszText.GetBuffer(nLength+1), nLength+1);
   cszText.ReleaseBuffer(nLength);

   return m_pInplaceParentCallback->EndLabelEdit(cszText);
}

void CInplaceTextEditControl::Finish()
{
   ATLTRACE(_T("Finish\n"));
   if (!m_bFinished)
   {
      m_bFinished = true;
//      ::SetFocus(GetParent());
      DestroyWindow();
   }
}

LRESULT CInplaceTextEditControl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   ATLTRACE(_T("OnChar\n"));
   switch (wParam)
   {
   case VK_RETURN:
      if (!AcceptChanges())
      {
         bHandled = true;
         break;
      }
      // fall-through

   case VK_ESCAPE:
      Finish();
      break;

   default:
      bHandled = false;
      break;
   }

   return 0;
}

LRESULT CInplaceTextEditControl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("OnKillFocus\n"));
   if (!m_bFinished)
   {
      AcceptChanges();
      Finish();
   }
   ATLTRACE(_T("OnKillFocus end\n"));
   return 0;
}

LRESULT CInplaceTextEditControl::OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("OnNcDestroy\n"));
   if (!m_bFinished)
      m_pInplaceParentCallback->NoticeFinish(this);
   m_hWnd = NULL;
   ATLTRACE(_T("OnNcDestroy end\n"));
   return 0;
}


// CPropertyListCtrlBase methods

CPropertyListCtrlBase::CPropertyListCtrlBase()
:m_nSortOrder(PLSO_CATEGORIZED),
 m_crGrayBackground(RGB(224,224,224)),
 m_pStorage(NULL),
 m_nInplaceEditItem(0),
 m_pInplaceEdit(NULL),
 m_nLeftAreaSize(14),
 m_bReadonly(false)
{
}

void CPropertyListCtrlBase::AddGroups(SPropertyGroupInfo* pGroupInfo)
{
   while(pGroupInfo != NULL && pGroupInfo->m_nPropertyGroupId != UINT(-1))
   {
      m_mapAllGroups.SetAt(pGroupInfo->m_nPropertyGroupId, *pGroupInfo);
      pGroupInfo++;
   }
}

void CPropertyListCtrlBase::AddItems(SPropertyItemInfo* pItemInfo)
{
   while(pItemInfo != NULL && pItemInfo->m_nPropertyId != UINT(-1))
   {
      m_mapAllItems.SetAt(pItemInfo->m_nPropertyId, *pItemInfo);
      pItemInfo++;
   }
}

void CPropertyListCtrlBase::Init()
{
   ATLASSERT(m_hWnd != NULL); // control must be created before calling

   if (m_pStorage == NULL)
      m_pStorage = &m_defaultStorage;

   InsertColumn(0, _T(""), LVCFMT_LEFT, 100, 0);
   InsertColumn(1, _T(""), LVCFMT_LEFT, 100, 0);

   BuildPropertiesList();

   m_fontNormal = GetFont();

   LOGFONT logFont;
   m_fontNormal.GetLogFont(logFont);

   logFont.lfWeight = FW_BOLD;
   m_fontGroupItem.CreateFontIndirect(&logFont);

   // create icon list to set item height
   m_nItemHeight = 16;
   m_ilSizeIcon.Create(1, m_nItemHeight, ILC_COLOR, 0, 0);

   SetImageList(m_ilSizeIcon, LVSIL_SMALL);

   // note: LVS_EX_FULLROWSELECT is needed so that HitTestEx can recognize items with subitem > 0
   SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
}

bool CPropertyListCtrlBase::SetReadonly(bool bReadonly)
{
   bool bOldReadonly = m_bReadonly;
   m_bReadonly = bReadonly;

   RedrawWindow(NULL, NULL, RDW_INVALIDATE);

   return bOldReadonly;
}

void CPropertyListCtrlBase::UpdateValues()
{
   BuildPropertiesList();
   RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void CPropertyListCtrlBase::ExpandGroupItem(UINT nListItemIndex, bool bExpand)
{
   ATLASSERT(true == IsListItemAPropertyGroup(nListItemIndex));
   ExpandGroup(GetListItemPropertyGroupId(nListItemIndex), bExpand);
}

void CPropertyListCtrlBase::ExpandGroup(UINT nPropertyGroupId, bool bExpand)
{
   ATLASSERT(NULL != m_mapAllGroups.Lookup(nPropertyGroupId));

   SPropertyGroupInfo& propGroupInfo = m_mapAllGroups[nPropertyGroupId];
   propGroupInfo.m_bExpanded = bExpand;

   BuildPropertiesList();
}

bool CPropertyListCtrlBase::IsListItemAPropertyGroup(unsigned int nListItemIndex) const
{
   ATLASSERT(nListItemIndex < m_aAllProperties.GetCount()); // range check
   return m_aAllProperties[nListItemIndex].m_bIsGroup;
}

UINT CPropertyListCtrlBase::GetListItemPropertyGroupId(unsigned int nListItemIndex) const
{
   ATLASSERT(nListItemIndex < m_aAllProperties.GetCount()); // range check
   return m_aAllProperties[nListItemIndex].m_nPropertyGroupOrItemId;
}

UINT CPropertyListCtrlBase::GetListItemPropertyItemId(unsigned int nListItemIndex) const
{
   ATLASSERT(nListItemIndex < m_aAllProperties.GetCount()); // range check
   ATLASSERT(!IsListItemAPropertyGroup(nListItemIndex)); // must be no group item
   return m_aAllProperties[nListItemIndex].m_nPropertyGroupOrItemId;
}

bool CPropertyListCtrlBase::IsPropertyGroupExpanded(unsigned int nListItemIndex) const
{
   ATLASSERT(nListItemIndex < m_aAllProperties.GetCount()); // range check
   ATLASSERT(IsListItemAPropertyGroup(nListItemIndex)); // must be a group item

   UINT nPropertyGroupId = GetListItemPropertyGroupId(nListItemIndex);

   SPropertyGroupInfo propGroupInfo;
   ATLVERIFY(true == m_mapAllGroups.Lookup(nPropertyGroupId, propGroupInfo));

   return propGroupInfo.m_bExpanded;
}

CString CPropertyListCtrlBase::GetPropertyGroupName(UINT nPropertyGroupId) const
{
   SPropertyGroupInfo propGroupInfo;
   ATLVERIFY(true == m_mapAllGroups.Lookup(nPropertyGroupId, propGroupInfo));
   return GetString(propGroupInfo.m_sPropertyGroupName);
}

CString CPropertyListCtrlBase::GetPropertyItemName(UINT nPropertyItemId) const
{
   SPropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(nPropertyItemId, propItemInfo));
   return GetString(propItemInfo.m_sPropertyItemName);
}

CString CPropertyListCtrlBase::GetPropertyItemDesc(UINT nPropertyItemId) const
{
   SPropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(nPropertyItemId, propItemInfo));
   return GetString(propItemInfo.m_sPropertyItemDesc);
}

void CPropertyListCtrlBase::LeftButtonClick(CPoint pt, unsigned int nItem, unsigned int nSubItem)
{
   CRect rectBox;
   GetItemRect(nItem, rectBox, LVIR_BOUNDS);
   GetPlusMinusBoxRect(rectBox);

   if (IsListItemAPropertyGroup(nItem) && nSubItem == 0 && TRUE == rectBox.PtInRect(pt))
   {
      // toggle group item expansion
      ExpandGroupItem(nItem, !IsPropertyGroupExpanded(nItem));
   }
   else
   if (!IsListItemAPropertyGroup(nItem) && nSubItem == 1)
   {
      // yes, user clicked on a property value subitem
      EditProperty(nItem);
   }
   else
   {
      SetFocus(); // cancen edit boxes

      // select item
      SelectItem(nItem);
      Update(nItem);
   }
}

void CPropertyListCtrlBase::BuildPropertiesList()
{
   m_aAllProperties.RemoveAll();

   if (PLSO_CATEGORIZED == m_nSortOrder)
   {
      // categorized sort order
      SPropertyGroupInfo propGroupInfo;
      UINT nGroupId = UINT(-1);

      POSITION posGroups=m_mapAllGroups.GetStartPosition();
      while (posGroups != NULL)
      {
         m_mapAllGroups.GetNextAssoc(posGroups, nGroupId, propGroupInfo);

         // add group info
         SPropertyData propGroupData;
         propGroupData.m_bIsGroup = true;
         propGroupData.m_nPropertyGroupOrItemId = nGroupId;
         m_aAllProperties.Add(propGroupData);

         UINT nItemId = UINT(-1);
         SPropertyItemInfo propItemInfo;

         // when group is collapsed, don't add items
         if (!propGroupInfo.m_bExpanded)
            continue;

         // go through all properties with current group id
         POSITION posItems = m_mapAllItems.GetStartPosition();
         while (posItems != NULL)
         {
            m_mapAllItems.GetNextAssoc(posItems, nItemId, propItemInfo);

            if (nGroupId == propItemInfo.m_nPropertyGroupId)
            {
               // add property info
               SPropertyData propItemData;
               propItemData.m_bIsGroup = false;
               //propItemData.m_nPropertyGroupId = nGroupId; // TODO remove
               propItemData.m_nPropertyGroupOrItemId = nItemId;
               m_aAllProperties.Add(propItemData);
            }
         }
      }
   }
   else
   {
      // alphabetical sort order
      // TODO
   }

   // set virtual list item count
   SetItemCount(static_cast<int>(m_aAllProperties.GetCount()));
}

bool CPropertyListCtrlBase::HitTestEx(CPoint point, unsigned int& nItem, unsigned int& nSubItem)
{
   UINT nFlags = 0;
   nItem = HitTest(point, &nFlags);

   if ((nFlags & LVHT_ONITEMLABEL) == 0)
      return false; // didn't hit label

   int xpos = 0;
   nSubItem = 0;
   while (xpos < point.x)
      xpos += GetColumnWidth(nSubItem++);

   nSubItem--;

   return true;
}

void CPropertyListCtrlBase::GetSubItemRect(unsigned int nItem, unsigned int nSubItem, CRect& rect)
{
   GetItemRect(nItem, rect, LVIR_LABEL);

   unsigned int xpos = 0;
   for(unsigned int n=0; n<nSubItem; n++)
      xpos += GetColumnWidth(static_cast<int>(n));

   if (nSubItem != 0)
      rect.left = xpos;// TODO needed? + 3;
   rect.right = xpos + GetColumnWidth(nSubItem);
   // rect.bottom++; // TODO needed?
}

void CPropertyListCtrlBase::EditProperty(unsigned int nListItem)
{
   // check if the list control or the property is read-only
   if (m_bReadonly)
      return;

   // TODO check if property is also read-only

   SetFocus();

   m_nInplaceEditItem = nListItem;

   // simulate sending LVN_BEGINLABELEDIT
   NMLVDISPINFO dispInfo;
   dispInfo.hdr.hwndFrom = m_hWnd;
   dispInfo.hdr.idFrom = static_cast<UINT_PTR>(-1);
   dispInfo.hdr.code = LVN_BEGINLABELEDIT;
   dispInfo.item.mask = 0;
   dispInfo.item.iItem = nListItem;
   dispInfo.item.iSubItem = 1;

   // send to parent
   int nRet = SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&dispInfo);

   if (nRet != 0)
      return;

   CRect rectItem;
   GetSubItemRect(nListItem, 1, rectItem);
   rectItem.bottom--;
   rectItem.right--;

   UINT nPropertyId = GetListItemPropertyItemId(nListItem);
   CString cszPropertyValue = GetPropertyItemValue(nPropertyId);

   // create and init edit control
   m_pInplaceEdit = CreateEditControl(nPropertyId);

   m_pInplaceEdit->Init(this);
   m_pInplaceEdit->Create(m_hWnd, rectItem, cszPropertyValue);
}

IInplaceEditControl* CPropertyListCtrlBase::CreateEditControl(UINT nPropertyItemId)
{
   SPropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(nPropertyItemId, propItemInfo));

   IInplaceEditControl* pEditControl = NULL;

   // create edit control dependent on property type
   switch (propItemInfo.m_enPropertyType)
   {
   case enPropertyTypeText:
      pEditControl = new CInplaceTextEditControl();
      break;

   case enPropertyTypeComboBox:
      ATLASSERT(false); // TODO implement
      break;

   case enPropertyTypeCustom:
      pEditControl = propItemInfo.m_pCustomProperty->CreateInplaceEditControl();
      break;

   default:
      ATLASSERT(false);
      break;
   }

   return pEditControl;
}

CString CPropertyListCtrlBase::GetString(const SStringOrId& sStringOrId) const
{
   if (sStringOrId.m_nStringId == UINT(-1))
      return CString(sStringOrId.m_pszString);
   else
   {
      CString cszText;
      cszText.LoadString(sStringOrId.m_nStringId);
      return cszText;
   }
}

void CPropertyListCtrlBase::GetPlusMinusBoxRect(CRect& rect)
{
   rect.top = rect.top + 3;
   rect.left = rect.left + 3;
   rect.bottom = rect.top + 9;
   rect.right = rect.left + 9;
}

void CPropertyListCtrlBase::DrawItem(unsigned int nItem, unsigned int nSubItem, CDCHandle dc, UINT uItemState)
{
   if (IsListItemAPropertyGroup(nItem))
   {
      if (nSubItem == 0)
         DrawGroupItem(nItem, dc, uItemState);
   }
   else
   {
      if (nSubItem == 0)
         DrawPropertyNameItem(nItem, dc, uItemState);
      else
         DrawPropertyValueItem(nItem, dc, uItemState);
   }
}

void CPropertyListCtrlBase::DrawGroupItem(unsigned int nItem, CDCHandle dc, UINT /*uItemState*/)
{
   CRect rect;
   GetItemRect(nItem, rect, LVIR_BOUNDS);

   // draw background
   dc.SetBkColor(m_crGrayBackground);
   dc.FillSolidRect(rect, m_crGrayBackground);

   CRect rectText(rect);
   rectText.left += m_nLeftAreaSize + 2 + 1;
   rectText.bottom -= 2;

   UINT nGroupId = GetListItemPropertyGroupId(nItem);
   CString cszText = GetPropertyGroupName(nGroupId);

   // draw group name text
   HFONT oldFont = dc.SelectFont(m_fontGroupItem);
   dc.SetTextColor(RGB(128,128,128));

   dc.DrawText(cszText, cszText.GetLength(), rectText,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

   CSize sTextExtent;
   dc.GetTextExtent(cszText, cszText.GetLength(), &sTextExtent);

   dc.SelectFont(oldFont);

   // draw focus rect when focused
/*
   bool bIsFocused = (uItemState & CDIS_FOCUS) != 0;
   if (bIsFocused)
   {
      CRect rectFocus(rectText);
      rectFocus.right = rectFocus.left + sTextExtent.cx;
      rectFocus.top = rectFocus.bottom - sTextExtent.cy;

      rectFocus.InflateRect(3,1,3,1);

      dc.DrawFocusRect(rectFocus);
   }
*/

   // draw box with plus or minus sign
   CRect rectBox(rect);
   GetPlusMinusBoxRect(rectBox);

   dc.FrameRect(rectBox, (HBRUSH)GetStockObject(BLACK_BRUSH));

   // draw inner area
   CRect rectBoxInner(rectBox);
   rectBoxInner.DeflateRect(1,1);
   dc.FillSolidRect(rectBoxInner, RGB(255,255,255));

   // draw minus sign
   dc.MoveTo(rectBox.left+2, rectBox.top+4);
   dc.LineTo(rectBox.left+7, rectBox.top+4);

   // complete plus sign when not expanded
   if (!IsPropertyGroupExpanded(nItem))
   {
      dc.MoveTo(rectBox.left+4, rectBox.top+2);
      dc.LineTo(rectBox.left+4, rectBox.top+7);
   }
}

void CPropertyListCtrlBase::DrawPropertyNameItem(unsigned int nItem, CDCHandle dc, UINT uItemState)
{
   CRect rect;
   GetItemRect(nItem, rect, LVIR_BOUNDS);
   rect.right = GetColumnWidth(0)-1;

   CRect rectFrontBar(rect);
   rectFrontBar.right = rectFrontBar.left + m_nLeftAreaSize;
   dc.FillSolidRect(rectFrontBar, m_crGrayBackground);

   // draw grid lines around name area
   CRect rectNameArea(rect);
   rectNameArea.left = rectFrontBar.right;

   CPen pen;
   pen.CreatePen(PS_SOLID, 1, m_crGrayBackground);
   HPEN oldPen = dc.SelectPen(pen);

   dc.MoveTo(rectFrontBar.left, rectNameArea.bottom-1);
   dc.LineTo(rectNameArea.right-1, rectNameArea.bottom-1);
   dc.LineTo(rectNameArea.right-1, rectNameArea.top-1);

   dc.SelectPen(oldPen);

   rectNameArea.DeflateRect(0,0,1,1);

   // draw blue selection box when selected
   COLORREF crNameArea = RGB(255,255,255);
   bool bIsFocused = (uItemState & CDIS_FOCUS) != 0;
   if (bIsFocused)
      crNameArea = RGB(0,0,128);

   dc.FillSolidRect(rectNameArea, crNameArea);

   rectNameArea.left += 2;
   rectNameArea.bottom -= 2;

   // draw property item name
   UINT nPropertyId = GetListItemPropertyItemId(nItem);
   CString cszText = GetPropertyItemName(nPropertyId);

   COLORREF crText = bIsFocused ? RGB(255,255,255) : RGB(0,0,0);
   if (m_bReadonly) // TODO check if item is readonly
      crText = RGB(128,128,128);

   dc.SetTextColor(crText);

   dc.DrawText(cszText, cszText.GetLength(), rectNameArea,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}

void CPropertyListCtrlBase::DrawPropertyValueItem(unsigned int nItem, CDCHandle dc, UINT /*uItemState*/)
{
   CRect rect;
   GetItemRect(nItem, rect, LVIR_BOUNDS);
   rect.left = GetColumnWidth(0);

   // draw grid lines around name area
   CRect rectNameArea(rect);

   CPen pen;
   pen.CreatePen(PS_SOLID, 1, m_crGrayBackground);
   HPEN oldPen = dc.SelectPen(pen);

   dc.MoveTo(rectNameArea.left, rectNameArea.bottom-1);
   dc.LineTo(rectNameArea.right-1, rectNameArea.bottom-1);
   dc.LineTo(rectNameArea.right-1, rectNameArea.top-1);
   dc.SelectPen(oldPen);

   rectNameArea.DeflateRect(0,0,1,1);

   // draw name area background
   dc.FillSolidRect(rectNameArea, RGB(255,255,255));

   rectNameArea.left += 3;
   rectNameArea.bottom -= 2;

   // draw property item name
   UINT nPropertyId = GetListItemPropertyItemId(nItem);
   CString cszText = GetPropertyItemValue(nPropertyId);

   dc.SetTextColor(RGB(0,0,0));

   dc.DrawText(cszText, cszText.GetLength(), rectNameArea,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}

bool CPropertyListCtrlBase::EndLabelEdit(const CString& cszValue)
{
   CString cszPropertyValue(cszValue);

   // simulate LVN_ENDLABELEDIT message
   NMLVDISPINFO dispinfo;
   dispinfo.hdr.hwndFrom = m_hWnd;
   dispinfo.hdr.idFrom = 0;
   dispinfo.hdr.code = LVN_ENDLABELEDIT;

   dispinfo.item.mask = LVIF_TEXT;
   dispinfo.item.iItem = m_nInplaceEditItem;
   dispinfo.item.iSubItem = 1;
   dispinfo.item.pszText = (LPTSTR)(LPCTSTR)cszPropertyValue; // TODO hack

   // send to parent's parent, since it is a notification, which gets reflected back
   // TODO cleanup
//   HWND hWnd = ::GetParent(GetParent());
//   int nRet = ::SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispinfo);
   int nRet = GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM)&dispinfo);

   if (1 == nRet)
   {
      UINT nPropertyId = GetListItemPropertyItemId(m_nInplaceEditItem);
      SetPropertyItemValue(nPropertyId, cszValue);
   }

   return 1 == nRet;
}

// TODO pass pInplaceEdit at all?
void CPropertyListCtrlBase::NoticeFinish(IInplaceEditControl* pInplaceEdit)
{
   pInplaceEdit;
   ATLASSERT(m_pInplaceEdit == pInplaceEdit);

   PostMessage(PLM_FINISH_INPLACE, 0, reinterpret_cast<LPARAM>(this));
}
