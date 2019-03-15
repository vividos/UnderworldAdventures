//
// Property List Control
// Copyright (C) 2005,2019 Michael Fink
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.
//
/// \file PropertyListCtrl.cpp
/// \brief Property List Control implementation
//
#include "stdatl.hpp"
#include "PropertyListCtrl.hpp"

using namespace PropertyList;

// InplaceTextEditControl methods

void InplaceTextEditControl::Create(HWND hWnd, CRect rect, LPCTSTR propertyValue)
{
   rect.top++;

   // create
   BaseClass::Create(hWnd, rect, _T(""), WS_VISIBLE | WS_CHILD);
   SetWindowText(propertyValue);

   SetFocus();
   SetSel(0, -1);

   // set font of owner
   HFONT hFont = CWindow(hWnd).GetFont();
   SetFont(hFont);
}

bool InplaceTextEditControl::AcceptChanges()
{
   // get text
   CString text;
   int length = GetWindowTextLength();
   CWindow::GetWindowText(text.GetBuffer(length + 1), length + 1);
   text.ReleaseBuffer(length);

   return m_inplaceParentCallback->EndLabelEdit(text);
}

void InplaceTextEditControl::Finish()
{
   ATLTRACE(_T("Finish\n"));
   if (!m_isFinished)
   {
      m_isFinished = true;
      //::SetFocus(GetParent());
      DestroyWindow();
   }
}

LRESULT InplaceTextEditControl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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

LRESULT InplaceTextEditControl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("OnKillFocus\n"));
   if (!m_isFinished)
   {
      AcceptChanges();
      Finish();
   }
   ATLTRACE(_T("OnKillFocus end\n"));
   return 0;
}

LRESULT InplaceTextEditControl::OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("OnNcDestroy\n"));
   if (!m_isFinished)
      m_inplaceParentCallback->NoticeFinish(this);
   m_hWnd = NULL;
   ATLTRACE(_T("OnNcDestroy end\n"));
   return 0;
}


// PropertyListCtrlBase methods

PropertyListCtrlBase::PropertyListCtrlBase()
   :m_sortOrder(PLSO_CATEGORIZED),
   m_crGrayBackground(RGB(224, 224, 224)),
   m_storage(NULL),
   m_inplaceEditItem(0),
   m_inplaceEdit(NULL),
   m_leftAreaSize(14),
   m_isReadOnly(false)
{
}

void PropertyListCtrlBase::AddGroups(PropertyGroupInfo* groupInfo)
{
   while (groupInfo != NULL && groupInfo->m_propertyGroupId != UINT(-1))
   {
      m_mapAllGroups.SetAt(groupInfo->m_propertyGroupId, *groupInfo);
      groupInfo++;
   }
}

void PropertyListCtrlBase::AddItems(PropertyItemInfo* itemInfo)
{
   while (itemInfo != NULL && itemInfo->m_propertyId != UINT(-1))
   {
      m_mapAllItems.SetAt(itemInfo->m_propertyId, *itemInfo);
      itemInfo++;
   }
}

void PropertyListCtrlBase::Init()
{
   ATLASSERT(m_hWnd != NULL); // control must be created before calling

   if (m_storage == NULL)
      m_storage = &m_defaultStorage;

   InsertColumn(0, _T(""), LVCFMT_LEFT, 100, 0);
   InsertColumn(1, _T(""), LVCFMT_LEFT, 100, 0);

   BuildPropertiesList();

   m_fontNormal = GetFont();

   LOGFONT logFont;
   m_fontNormal.GetLogFont(logFont);

   logFont.lfWeight = FW_BOLD;
   m_fontGroupItem.CreateFontIndirect(&logFont);

   // create icon list to set item height
   m_itemHeight = 16;
   m_imageListSizeIcon.Create(1, m_itemHeight, ILC_COLOR, 0, 0);

   SetImageList(m_imageListSizeIcon, LVSIL_SMALL);

   // note: LVS_EX_FULLROWSELECT is needed so that HitTestEx can recognize items with subitem > 0
   SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
}

bool PropertyListCtrlBase::SetReadOnly(bool readonly)
{
   bool bOldReadonly = m_isReadOnly;
   m_isReadOnly = readonly;

   RedrawWindow(NULL, NULL, RDW_INVALIDATE);

   return bOldReadonly;
}

void PropertyListCtrlBase::UpdateValues()
{
   BuildPropertiesList();
   RedrawWindow(NULL, NULL, RDW_INVALIDATE);
}

void PropertyListCtrlBase::ExpandGroupItem(UINT listItemIndex, bool expand)
{
   ATLASSERT(true == IsListItemAPropertyGroup(listItemIndex));
   ExpandGroup(GetListItemPropertyGroupId(listItemIndex), expand);
}

void PropertyListCtrlBase::ExpandGroup(UINT propertyGroupId, bool expand)
{
   ATLASSERT(NULL != m_mapAllGroups.Lookup(propertyGroupId));

   PropertyGroupInfo& propGroupInfo = m_mapAllGroups[propertyGroupId];
   propGroupInfo.m_isExpanded = expand;

   BuildPropertiesList();
}

bool PropertyListCtrlBase::IsListItemAPropertyGroup(unsigned int listItemIndex) const
{
   ATLASSERT(listItemIndex < m_allProperties.GetCount()); // range check
   return m_allProperties[listItemIndex].m_isGroup;
}

UINT PropertyListCtrlBase::GetListItemPropertyGroupId(unsigned int listItemIndex) const
{
   ATLASSERT(listItemIndex < m_allProperties.GetCount()); // range check
   return m_allProperties[listItemIndex].m_propertyGroupOrItemId;
}

UINT PropertyListCtrlBase::GetListItemPropertyItemId(unsigned int listItemIndex) const
{
   ATLASSERT(listItemIndex < m_allProperties.GetCount()); // range check
   ATLASSERT(!IsListItemAPropertyGroup(listItemIndex)); // must be no group item
   return m_allProperties[listItemIndex].m_propertyGroupOrItemId;
}

bool PropertyListCtrlBase::IsPropertyGroupExpanded(unsigned int listItemIndex) const
{
   ATLASSERT(listItemIndex < m_allProperties.GetCount()); // range check
   ATLASSERT(IsListItemAPropertyGroup(listItemIndex)); // must be a group item

   UINT propertyGroupId = GetListItemPropertyGroupId(listItemIndex);

   PropertyGroupInfo propGroupInfo;
   ATLVERIFY(true == m_mapAllGroups.Lookup(propertyGroupId, propGroupInfo));

   return propGroupInfo.m_isExpanded;
}

CString PropertyListCtrlBase::GetPropertyGroupName(UINT propertyGroupId) const
{
   PropertyGroupInfo propGroupInfo;
   ATLVERIFY(true == m_mapAllGroups.Lookup(propertyGroupId, propGroupInfo));
   return GetString(propGroupInfo.m_propertyGroupName);
}

CString PropertyListCtrlBase::GetPropertyItemName(UINT propertyItemId) const
{
   PropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(propertyItemId, propItemInfo));
   return GetString(propItemInfo.m_propertyItemName);
}

CString PropertyListCtrlBase::GetPropertyItemDesc(UINT propertyItemId) const
{
   PropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(propertyItemId, propItemInfo));
   return GetString(propItemInfo.m_propertyItemDesc);
}

void PropertyListCtrlBase::LeftButtonClick(CPoint pt, unsigned int item, unsigned int subItem)
{
   CRect rectBox;
   GetItemRect(item, rectBox, LVIR_BOUNDS);
   GetPlusMinusBoxRect(rectBox);

   if (IsListItemAPropertyGroup(item) && subItem == 0 && TRUE == rectBox.PtInRect(pt))
   {
      // toggle group item expansion
      ExpandGroupItem(item, !IsPropertyGroupExpanded(item));
   }
   else if (!IsListItemAPropertyGroup(item) && subItem == 1)
   {
      // yes, user clicked on a property value subitem
      EditProperty(item);
   }
   else
   {
      SetFocus(); // cancen edit boxes

      // select item
      SelectItem(item);
      Update(item);
   }
}

void PropertyListCtrlBase::BuildPropertiesList()
{
   m_allProperties.RemoveAll();

   if (PLSO_CATEGORIZED == m_sortOrder)
   {
      // categorized sort order
      PropertyGroupInfo propGroupInfo;
      UINT nGroupId = UINT(-1);

      POSITION posGroups = m_mapAllGroups.GetStartPosition();
      while (posGroups != NULL)
      {
         m_mapAllGroups.GetNextAssoc(posGroups, nGroupId, propGroupInfo);

         // add group info
         PropertyData propGroupData;
         propGroupData.m_isGroup = true;
         propGroupData.m_propertyGroupOrItemId = nGroupId;
         m_allProperties.Add(propGroupData);

         UINT itemId = UINT(-1);
         PropertyItemInfo propItemInfo;

         // when group is collapsed, don't add items
         if (!propGroupInfo.m_isExpanded)
            continue;

         // go through all properties with current group id
         POSITION posItems = m_mapAllItems.GetStartPosition();
         while (posItems != NULL)
         {
            m_mapAllItems.GetNextAssoc(posItems, itemId, propItemInfo);

            if (nGroupId == propItemInfo.m_propertyGroupId)
            {
               // add property info
               PropertyData propItemData;
               propItemData.m_isGroup = false;
               //propItemData.m_propertyGroupId = nGroupId; // TODO remove
               propItemData.m_propertyGroupOrItemId = itemId;
               m_allProperties.Add(propItemData);
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
   SetItemCount(static_cast<int>(m_allProperties.GetCount()));
}

bool PropertyListCtrlBase::HitTestEx(CPoint point, unsigned int& item, unsigned int& subItem)
{
   UINT nFlags = 0;
   item = HitTest(point, &nFlags);

   if ((nFlags & LVHT_ONITEMLABEL) == 0)
      return false; // didn't hit label

   int xpos = 0;
   subItem = 0;
   while (xpos < point.x)
      xpos += GetColumnWidth(subItem++);

   subItem--;

   return true;
}

void PropertyListCtrlBase::GetSubItemRect(unsigned int item, unsigned int subItem, CRect& rect)
{
   GetItemRect(item, rect, LVIR_LABEL);

   unsigned int xpos = 0;
   for (unsigned int n = 0; n < subItem; n++)
      xpos += GetColumnWidth(static_cast<int>(n));

   if (subItem != 0)
      rect.left = xpos;// TODO needed? + 3;
   rect.right = xpos + GetColumnWidth(subItem);
   // rect.bottom++; // TODO needed?
}

void PropertyListCtrlBase::EditProperty(unsigned int nListItem)
{
   // check if the list control or the property is read-only
   if (m_isReadOnly)
      return;

   // TODO check if property is also read-only

   SetFocus();

   m_inplaceEditItem = nListItem;

   // simulate sending LVN_BEGINLABELEDIT
   NMLVDISPINFO dispInfo;
   dispInfo.hdr.hwndFrom = m_hWnd;
   dispInfo.hdr.idFrom = static_cast<UINT_PTR>(-1);
   dispInfo.hdr.code = LVN_BEGINLABELEDIT;
   dispInfo.item.mask = 0;
   dispInfo.item.iItem = nListItem;
   dispInfo.item.iSubItem = 1;

   // send to parent
   int ret = SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&dispInfo);

   if (ret != 0)
      return;

   CRect rectItem;
   GetSubItemRect(nListItem, 1, rectItem);
   rectItem.bottom--;
   rectItem.right--;

   UINT nPropertyId = GetListItemPropertyItemId(nListItem);
   CString propertyValue = GetPropertyItemValue(nPropertyId);

   // create and init edit control
   m_inplaceEdit = CreateEditControl(nPropertyId);

   m_inplaceEdit->Init(this);
   m_inplaceEdit->Create(m_hWnd, rectItem, propertyValue);
}

IInplaceEditControl* PropertyListCtrlBase::CreateEditControl(UINT propertyItemId)
{
   PropertyItemInfo propItemInfo;
   ATLVERIFY(true == m_mapAllItems.Lookup(propertyItemId, propItemInfo));

   IInplaceEditControl* editControl = NULL;

   // create edit control dependent on property type
   switch (propItemInfo.m_propertyType)
   {
   case propertyTypeText:
      editControl = new InplaceTextEditControl();
      break;

   case propertyTypeComboBox:
      ATLASSERT(false); // TODO implement
      break;

   case propertyTypeCustom:
      editControl = propItemInfo.m_customProperty->CreateInplaceEditControl();
      break;

   default:
      ATLASSERT(false);
      break;
   }

   return editControl;
}

CString PropertyListCtrlBase::GetString(const StringOrId& stringOrId) const
{
   if (stringOrId.m_stringId == UINT(-1))
      return CString(stringOrId.m_string);
   else
   {
      CString text;
      text.LoadString(stringOrId.m_stringId);
      return text;
   }
}

void PropertyListCtrlBase::GetPlusMinusBoxRect(CRect& rect)
{
   rect.top = rect.top + 3;
   rect.left = rect.left + 3;
   rect.bottom = rect.top + 9;
   rect.right = rect.left + 9;
}

void PropertyListCtrlBase::DrawItem(unsigned int item, unsigned int subItem, CDCHandle dc, UINT uItemState)
{
   if (IsListItemAPropertyGroup(item))
   {
      if (subItem == 0)
         DrawGroupItem(item, dc, uItemState);
   }
   else
   {
      if (subItem == 0)
         DrawPropertyNameItem(item, dc, uItemState);
      else
         DrawPropertyValueItem(item, dc, uItemState);
   }
}

void PropertyListCtrlBase::DrawGroupItem(unsigned int item, CDCHandle dc, UINT /*uItemState*/)
{
   CRect rect;
   GetItemRect(item, rect, LVIR_BOUNDS);

   // draw background
   dc.SetBkColor(m_crGrayBackground);
   dc.FillSolidRect(rect, m_crGrayBackground);

   CRect rectText(rect);
   rectText.left += m_leftAreaSize + 2 + 1;
   rectText.bottom -= 2;

   UINT nGroupId = GetListItemPropertyGroupId(item);
   CString text = GetPropertyGroupName(nGroupId);

   // draw group name text
   HFONT oldFont = dc.SelectFont(m_fontGroupItem);
   dc.SetTextColor(RGB(128, 128, 128));

   dc.DrawText(text, text.GetLength(), rectText,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

   CSize sTextExtent;
   dc.GetTextExtent(text, text.GetLength(), &sTextExtent);

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
   rectBoxInner.DeflateRect(1, 1);
   dc.FillSolidRect(rectBoxInner, RGB(255, 255, 255));

   // draw minus sign
   dc.MoveTo(rectBox.left + 2, rectBox.top + 4);
   dc.LineTo(rectBox.left + 7, rectBox.top + 4);

   // complete plus sign when not expanded
   if (!IsPropertyGroupExpanded(item))
   {
      dc.MoveTo(rectBox.left + 4, rectBox.top + 2);
      dc.LineTo(rectBox.left + 4, rectBox.top + 7);
   }
}

void PropertyListCtrlBase::DrawPropertyNameItem(unsigned int item, CDCHandle dc, UINT uItemState)
{
   CRect rect;
   GetItemRect(item, rect, LVIR_BOUNDS);
   rect.right = GetColumnWidth(0) - 1;

   CRect rectFrontBar(rect);
   rectFrontBar.right = rectFrontBar.left + m_leftAreaSize;
   dc.FillSolidRect(rectFrontBar, m_crGrayBackground);

   // draw grid lines around name area
   CRect rectNameArea(rect);
   rectNameArea.left = rectFrontBar.right;

   CPen pen;
   pen.CreatePen(PS_SOLID, 1, m_crGrayBackground);
   HPEN oldPen = dc.SelectPen(pen);

   dc.MoveTo(rectFrontBar.left, rectNameArea.bottom - 1);
   dc.LineTo(rectNameArea.right - 1, rectNameArea.bottom - 1);
   dc.LineTo(rectNameArea.right - 1, rectNameArea.top - 1);

   dc.SelectPen(oldPen);

   rectNameArea.DeflateRect(0, 0, 1, 1);

   // draw blue selection box when selected
   COLORREF crNameArea = RGB(255, 255, 255);
   bool bIsFocused = (uItemState & CDIS_FOCUS) != 0;
   if (bIsFocused)
      crNameArea = RGB(0, 0, 128);

   dc.FillSolidRect(rectNameArea, crNameArea);

   rectNameArea.left += 2;
   rectNameArea.bottom -= 2;

   // draw property item name
   UINT nPropertyId = GetListItemPropertyItemId(item);
   CString text = GetPropertyItemName(nPropertyId);

   COLORREF crText = bIsFocused ? RGB(255, 255, 255) : RGB(0, 0, 0);
   if (m_isReadOnly) // TODO check if item is readonly
      crText = RGB(128, 128, 128);

   dc.SetTextColor(crText);

   dc.DrawText(text, text.GetLength(), rectNameArea,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}

void PropertyListCtrlBase::DrawPropertyValueItem(unsigned int item, CDCHandle dc, UINT /*uItemState*/)
{
   CRect rect;
   GetItemRect(item, rect, LVIR_BOUNDS);
   rect.left = GetColumnWidth(0);

   // draw grid lines around name area
   CRect rectNameArea(rect);

   CPen pen;
   pen.CreatePen(PS_SOLID, 1, m_crGrayBackground);
   HPEN oldPen = dc.SelectPen(pen);

   dc.MoveTo(rectNameArea.left, rectNameArea.bottom - 1);
   dc.LineTo(rectNameArea.right - 1, rectNameArea.bottom - 1);
   dc.LineTo(rectNameArea.right - 1, rectNameArea.top - 1);
   dc.SelectPen(oldPen);

   rectNameArea.DeflateRect(0, 0, 1, 1);

   // draw name area background
   dc.FillSolidRect(rectNameArea, RGB(255, 255, 255));

   rectNameArea.left += 3;
   rectNameArea.bottom -= 2;

   // draw property item name
   UINT nPropertyId = GetListItemPropertyItemId(item);
   CString text = GetPropertyItemValue(nPropertyId);

   dc.SetTextColor(RGB(0, 0, 0));

   dc.DrawText(text, text.GetLength(), rectNameArea,
      DT_LEFT | DT_BOTTOM | DT_EDITCONTROL | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}

bool PropertyListCtrlBase::EndLabelEdit(const CString& value)
{
   CString propertyValue(value);

   // simulate LVN_ENDLABELEDIT message
   NMLVDISPINFO dispinfo;
   dispinfo.hdr.hwndFrom = m_hWnd;
   dispinfo.hdr.idFrom = 0;
   dispinfo.hdr.code = LVN_ENDLABELEDIT;

   dispinfo.item.mask = LVIF_TEXT;
   dispinfo.item.iItem = m_inplaceEditItem;
   dispinfo.item.iSubItem = 1;
   dispinfo.item.pszText = (LPTSTR)(LPCTSTR)propertyValue; // TODO hack

   // send to parent's parent, since it is a notification, which gets reflected back
   // TODO cleanup
//   HWND hWnd = ::GetParent(GetParent());
//   int nRet = ::SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispinfo);
   int nRet = GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM)&dispinfo);

   if (1 == nRet)
   {
      UINT nPropertyId = GetListItemPropertyItemId(m_inplaceEditItem);
      SetPropertyItemValue(nPropertyId, value);
   }

   return 1 == nRet;
}

// TODO pass inplaceEdit at all?
void PropertyListCtrlBase::NoticeFinish(IInplaceEditControl* inplaceEdit)
{
   (void)inplaceEdit;
   ATLASSERT(m_inplaceEdit == inplaceEdit);

   PostMessage(PLM_FINISH_INPLACE, 0, reinterpret_cast<LPARAM>(this));
}
