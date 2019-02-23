// Property List Control
// Copyright (C) 2005 Michael Fink
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

/*! \file PropertyListCtrl.h

Property List Control
Copyright (C) 2005 Michael Fink

A property list control presents properties to the use which the user may
edit. The properties can be grouped together by topic, and groups can be
collapsed and expanded for better overview.

A good example for a property list control is the "Properties" tab in
Microsoft Visual Studio .NET 2003.

Usage:

In any case, add add property group and property item lists to the file where
you will create your property list control, as follows:

For groups:

   enum T_enPropertyGroup
   {
      enGroup1=0,
      enGroup2,
   };

   BEGIN_PROPERTY_GROUP_LIST(g_aPropertyGroupList)
      PROPERTY_GROUP(enGroup1, PROPERTY_STRING(_T("Group 1")))
      PROPERTY_GROUP(enGroup2, PROPERTY_STRING(_T("Group 1")))
   END_PROPERTY_GROUP_LIST()

For properties (note that every property must get its own unique id):

   enum T_enPropertyItem
   {
      enProp1=0,
      enProp2,
      enProp3,
      enProp4,
      enProp5
   };

   BEGIN_PROPERTY_ITEM_LIST(g_aPropertyItemList)
      PROPERTY_ITEM(enProp1, enGroup1, enPropertyTypeText, PROPERTY_STRING(_T("Prop1")), PROPERTY_STRING(_T("Modifies prop1")))
      PROPERTY_ITEM(enProp2, enGroup1, enPropertyTypeText, PROPERTY_STRING(_T("Prop2")), PROPERTY_STRING(_T("Modifies prop2")))
      PROPERTY_ITEM(enProp3, enGroup2, enPropertyTypeText, PROPERTY_STRING(_T("Prop3")), PROPERTY_STRING(_T("Modifies prop3")))
      PROPERTY_ITEM(enProp4, enGroup2, enPropertyTypeText, PROPERTY_STRING(_T("Prop4")), PROPERTY_STRING(_T("Modifies prop4")))
      PROPERTY_ITEM(enProp5, enGroup2, enPropertyTypeText, PROPERTY_STRING(_T("Prop5")), PROPERTY_STRING(_T("Modifies prop5")))
   END_PROPERTY_ITEM_LIST()

If you want to use string resource ids instead of putting the strings directly
in your source files, use the PROPERTY_STRING_ID macro, as follows:

   PROPERTY_STRING_ID(IDS_PROP_PROP1)

Depending on how you want to use the property list control, you have to use
the control in different ways:

1. Use the class CPropertyListCtrl as a view class in your SDI or MDI
   application:

   In CMainFrame, add or replace:

      CPropertyListCtrl m_view;

   In CMainFrame, add this to the bottom of the message map:

      [...]
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   This ensures that notifications are reflected back from the parent class
   (CMainFrame) to the property list control.

   In CMainFrame::OnCreate() add or replace:

      m_hWndClient = m_view.Create(m_hWnd, rcDefault);
      m_view.AddGroups(g_aPropertyGroupList);
      m_view.AddItems(g_aPropertyItemList);
      m_view.Init();

2. Use the class as a dynamic control in a dialog (e.g. derived from
   CDialogImpl):

   In your dialog class, add a private member:

      CPropertyListCtrl m_propertyList;

   In your dialog class, add this to the bottom of the message map, again to
   reflect notifications back to the control:

      [...]
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   In CMyDialog::OnInitInstance(), add the following:

      CRect rect;
      GetDlgItem(IDC_MYFRAME).GetWindowRect(rect);
      ScreenToClient();

      m_propertyList.Create(m_hWnd, rect);
      m_propertyList.AddGroups(g_aPropertyGroupList);
      m_propertyList.AddItems(g_aPropertyItemList);
      m_propertyList.Init();

   This creates your control on the dialog where you define an invisible
   control, e.g. a picture frame control named IDC_MYFRAME. The frame is used
   to position the control on the dialog.

All property values are kept in an internal default storage that stores all
property values as strings. You can access the properties with the two methods
GetPropertyItemValue() and SetPropertyItemValue(). They take a property id.

If you want to change the storage of your property item values, you can derive
a class from the interface IPropertyStorage and implement all pure virtual
methods. Before calling m_view.Init(), set the new storage per

   m_view.SetStorage(pMyPropertyStorage).

If you want to implement own property types, consider looking at the property
type "enPropertyTypeCustom", the interface IPropertyItemCustom and the
interface IInplaceEditControl, but it requires a bit understanding what is
going on in the control.

Note: the control doesn't support "dynamic properties" that Microsoft Visual
Studio supports (e.g. seen on the "Events" page of the "Properties" window for
a class in Class View).

*/
#pragma once

// needed ATL/WTL includes
#include <atlcoll.h>
#include <atlmisc.h>


//! type a property can have
enum T_enPropertyType
{
   enPropertyTypeText=0,   //!< text; uses inplace edit control
   enPropertyTypeComboBox, //!< choice; uses inplace combobox control

   enPropertyTypeCustom,   //!< custom property; uses IPropertyItemCustom to customize property
};


namespace nsPropertyList
{

#define IDC_PROPGRID_CATEGORIZED 1024
#define IDC_PROPGRID_ALPHABETICAL 1025


//! struct used to hold a string or a string resource id
struct SStringOrId
{
   UINT m_nStringId;    //!< string id, or -1 when unused
   LPCTSTR m_pszString; //!< string, or NULL when unused
};

// defines a property string by giving the string itself
#define PROPERTY_STRING(str) { UINT(-1), str }

// defines a property string by giving its string resource id
#define PROPERTY_STRING_ID(str_id) { str_id, NULL }

// ********************
// Property Group infos
// ********************

//! information about a property group item
struct SPropertyGroupInfo
{
   UINT m_nPropertyGroupId;
   SStringOrId m_sPropertyGroupName;
   bool m_bExpanded;
};

// macros to define property group lists

// property group list start
#define BEGIN_PROPERTY_GROUP_LIST(var) \
   static nsPropertyList::SPropertyGroupInfo var[] = {

// property group item
#define PROPERTY_GROUP(id, name_string_or_id) \
   { id, name_string_or_id, true },

// property group list end
#define END_PROPERTY_GROUP_LIST() { UINT(-1), PROPERTY_STRING_ID(UINT(-1)) } };


// *******************
// Property item infos
// *******************

class IPropertyItemCustom;

//! information about a property item
struct SPropertyItemInfo
{
   //! proprety item id; must be unique throughout the property list
   UINT m_nPropertyId;
   //! property group id from a SPropertyGroupInfo struct
   UINT m_nPropertyGroupId;

   //! property type
   T_enPropertyType m_enPropertyType;
   //! custom item type interface; used when property type is enPropertyTypeCustom
   IPropertyItemCustom* m_pCustomProperty;

   //! string or string id of property item name
   SStringOrId m_sPropertyItemName;
   //! string or string id of property item description text
   SStringOrId m_sPropertyItemDesc;
};

// macros to define property item lists

// property group list start
#define BEGIN_PROPERTY_ITEM_LIST(var) \
   static nsPropertyList::SPropertyItemInfo var[] = {

// property item
#define PROPERTY_ITEM(id, group_id, type, name_string_or_id, desc_string_or_id) \
   { id, group_id, type, NULL, name_string_or_id, desc_string_or_id },

// custom property item
#define PROPERTY_ITEM_CUSTOM(id, group_id, interface_ptr, name_string_or_id, desc_string_or_id) \
   { id, group_id, enPropertyTypeCustom, interface_ptr, name_string_or_id, desc_string_or_id },

// property group list end
#define END_PROPERTY_ITEM_LIST() \
   { UINT(-1), UINT(-1), enPropertyTypeCustom, NULL, PROPERTY_STRING_ID(UINT(-1)), PROPERTY_STRING_ID(UINT(-1)) }\
};


//! property item data; represents an item in the list control
struct SPropertyData
{
   //! ctor
   SPropertyData():m_bIsGroup(true), m_nPropertyGroupOrItemId(UINT(-1)){}

   //! is list item a property group item?
   bool m_bIsGroup;

   //! property group id or property id, depending on m_bIsGroup
   UINT m_nPropertyGroupOrItemId;
};


//! interface to property storage
/*! reimplement this interface and call SetStorage() to have alternate property storage */
class IPropertyStorage
{
public:
   //! dtor
   virtual ~IPropertyStorage(){}

   //! retrieves property value for given property id
   virtual void GetProperty(unsigned int nPropertyItemId, CString& cszProperty)=0;

   //! sets new property value for given property id
   virtual void SetProperty(unsigned int nPropertyItemId, const CString& cszProperty)=0;

   //! verifies if property value for given property id is correct
   virtual bool VerifyProperty(unsigned int nPropertyItemId, const CString& cszProperty){ nPropertyItemId; cszProperty; return true; }
};


//! default storage for properties; just stores property values as strings
class CPropertyStorageDefault: public IPropertyStorage
{
public:
   virtual void GetProperty(unsigned int nPropertyItemId, CString& cszProperty)
   {
      if (!m_mapProperties.Lookup(nPropertyItemId, cszProperty))
      {
         cszProperty.Empty();
         m_mapProperties.SetAt(nPropertyItemId, cszProperty);
      }
   }

   virtual void SetProperty(unsigned int nPropertyItemId, const CString& cszProperty)
   {
      m_mapProperties.SetAt(nPropertyItemId, cszProperty);
   }

private:
   //! map with all properties stored as string
   CAtlMap<unsigned int, CString> m_mapProperties;
};


#define PLM_FINISH_INPLACE (WM_USER + 200)

//! inplace edit control callback interface
class IInplaceParentCallback
{
public:
   //! ctor
   virtual ~IInplaceParentCallback(){}

   //! ends label edit and returns new property value; returns false when new value is not accepted
   virtual bool EndLabelEdit(const CString& cszValue)=0;

   //! notices list control that inplace edit control is finished and can be deleted
   virtual void NoticeFinish(class IInplaceEditControl* pInplaceEdit)=0;
};

//! interface for inplace edit controls
class IInplaceEditControl
{
public:
   //! ctor
   IInplaceEditControl():m_pInplaceParentCallback(NULL){}

   //! initializes inplace edit control
   void Init(IInplaceParentCallback* pInplaceParentCallback)
   {
      m_pInplaceParentCallback = pInplaceParentCallback;
   }

   //! creates inplace edit control
   virtual void Create(HWND hWnd, CRect rect, LPCTSTR pszPropertyValue)=0;

protected:
   //! callback to inplace parent class
   IInplaceParentCallback* m_pInplaceParentCallback;
};


//! text edit control for editing text properties
class CInplaceTextEditControl :
   public CWindowImpl<CInplaceTextEditControl, CEdit>,
   public IInplaceEditControl
{
   typedef CWindowImpl<CInplaceTextEditControl, CEdit> BaseClass;
public:
   //! ctor
   CInplaceTextEditControl():m_bFinished(false){}

   virtual void Create(HWND hWnd, CRect rect, LPCTSTR pszPropertyValue);

   BEGIN_MSG_MAP(CInplaceTextEditControl)
      MESSAGE_HANDLER(WM_CHAR, OnChar)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
      MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
   END_MSG_MAP()

protected:
   bool AcceptChanges();
   void Finish();

protected:
   LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
   bool m_bFinished;
};


//! interface for custom property item types
class IPropertyItemCustom
{
public:
   //! dtor
   virtual ~IPropertyItemCustom(){}

   //! custom drawing for custom item; returns true when item was custom-drawn
   virtual bool DrawItem(CRect rect)=0;

   //! creates an inplace edit control for use when editing the property
   virtual IInplaceEditControl* CreateInplaceEditControl()=0;
};


//! base class for property list; implements use-independent functions
class CPropertyListCtrlBase :
   public CListViewCtrl,
   public IInplaceParentCallback
{
public:
   //! ctor
   CPropertyListCtrlBase();
   virtual ~CPropertyListCtrlBase(){}

   //! deletes all property groups
   void DeleteAllGroups();
   //! deletes all property items
   void DeleteAllItems();

   //! dynamically adds property groups
   void AddGroups(SPropertyGroupInfo* pGroupInfo);
   //! dynamically adds property items
   void AddItems(SPropertyItemInfo* pItemInfo);

   //! sets storage interface
   void SetStorage(IPropertyStorage* pStorage){ m_pStorage = pStorage; }

   //! returns current storage interface
   IPropertyStorage* GetStorage() const { return m_pStorage; }

   //! initializes property list; call after adding groups and property items
   void Init();

   //! sets read-only state of list control
   bool SetReadonly(bool bReadonly);

   //! updates all values from storage
   void UpdateValues();

   //! sort order
   enum T_enListSortOrder
   {
      PLSO_CATEGORIZED = 0, //!< uses groups to categorize property items
      PLSO_ALPHABETICAL = 1 //!< uses alphabetical sorting of property items; doesn't usegroups
   };

   //! sets new sort order
   void SetSortOrder(UINT nSortOrder)
   {
      m_nSortOrder = nSortOrder;
      BuildPropertiesList();
      // TODO redraw?
   }

   //! expands an item that is a group
   void ExpandGroupItem(UINT nListItemIndex, bool bExpand=true);
   //! expands a group item
   void ExpandGroup(UINT nPropertyGroupId, bool bExpand=true);

   //! determines if a given list item is a property group
   bool IsListItemAPropertyGroup(unsigned int nListItemIndex) const;
   //! returns property group id of given list item; item must be a group item
   UINT GetListItemPropertyGroupId(unsigned int nListItemIndex) const;
   //! returns property id of given list item; item must be a property item
   UINT GetListItemPropertyItemId(unsigned int nListItemIndex) const;

   //! determines if a group item is expanded
   bool IsPropertyGroupExpanded(unsigned int nListItemIndex) const;

   //! returns property group name by given property group id
   CString GetPropertyGroupName(UINT nPropertyGroupId) const;
   //! returns property item name by given property id
   CString GetPropertyItemName(UINT nPropertyItemId) const;
   //! returns description string of property with given property id
   CString GetPropertyItemDesc(UINT nPropertyItemId) const;

   //! returns property value for given property id
   CString GetPropertyItemValue(UINT nPropertyItemId) const
   {
      ATLASSERT(m_pStorage != NULL); // called Init() or SetStorage()?
      CString cszValue;
      m_pStorage->GetProperty(nPropertyItemId, cszValue);
      return cszValue;
   }

   //! sets property value for given property id
   void SetPropertyItemValue(UINT nPropertyItemId, const CString& cszValue)
   {
      ATLASSERT(m_pStorage != NULL); // called Init() or SetStorage()?
      m_pStorage->SetProperty(nPropertyItemId, cszValue);
   }

   void LeftButtonClick(CPoint pt, unsigned int nItem, unsigned int nSubItem);

protected:
   //! rebuilds properties list depending on sort order
   void BuildPropertiesList();

   //! does hit-testing for a specific subitem; returns false when no subitem was hit
   bool HitTestEx(CPoint point, unsigned int& nItem, unsigned int& nSubItem);

   //! retrieves rectangle of given subitem
   void GetSubItemRect(unsigned int nItem, unsigned int nSubItem, CRect& rect);

   //! starts editing a property
   void EditProperty(unsigned int nListItem);

   //! creates edit control for given property id
   IInplaceEditControl* CreateEditControl(UINT nPropertyItemId);

   // retrieves string by struct that either contains string or string resource id
   CString GetString(const SStringOrId& sStringOrId) const;

   //! returns rect of plus/minus box; takes item rect
   void GetPlusMinusBoxRect(CRect& rect);

   //! draw list control item
   void DrawItem(unsigned int nItem, unsigned int nSubItem, CDCHandle dc, UINT uItemState);
   //! draws group item area
   void DrawGroupItem(unsigned int nItem, CDCHandle dc, UINT uItemState);
   //! draws property item name column
   void DrawPropertyNameItem(unsigned int nItem, CDCHandle dc, UINT uItemState);
   //! draws property item value column
   void DrawPropertyValueItem(unsigned int nItem, CDCHandle dc, UINT uItemState);

   // virtual methods from IInplaceParentCallback
   virtual bool EndLabelEdit(const CString& cszValue);
   virtual void NoticeFinish(IInplaceEditControl* pInplaceEdit);

protected:
   //! normal text
   CFont m_fontNormal;
   //! bold text for group items
   CFont m_fontGroupItem;

   //! background color; used for group items and left bar
   COLORREF m_crGrayBackground;

   //! height of list item
   int m_nItemHeight;

   //! size of left area
   unsigned int m_nLeftAreaSize;

   //! pointer to property storage
   IPropertyStorage* m_pStorage;

   //! number of item currently inplace-edited
   unsigned int m_nInplaceEditItem;

   //! current inplace edit control
   IInplaceEditControl* m_pInplaceEdit;

   //! indicates if list control is read-only
   bool m_bReadonly;

private:
   //! mapping of all group ids to group info structs
   CAtlMap<UINT, SPropertyGroupInfo> m_mapAllGroups;
   //! mapping of all property ids to property info structs
   CAtlMap<UINT, SPropertyItemInfo> m_mapAllItems;

   //! array with all property item data structs
   CAtlArray<SPropertyData> m_aAllProperties;

   //! default storage when no other storage was set using SetStorage()
   CPropertyStorageDefault m_defaultStorage;

   //! sort order; see T_enListSortOrder
   unsigned int m_nSortOrder;

   //! image list to properly size list item
   CImageList m_ilSizeIcon;
};


//! window style traits for property list control
typedef CWinTraitsOR<
   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
      LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_EDITLABELS | LVS_OWNERDATA,
   WS_EX_CLIENTEDGE> CPropertyListCtrlWinTraits;

//! property list control implementation
template <class T,
   class TBase = CPropertyListCtrlBase,
   class TWinTraits = CPropertyListCtrlWinTraits>
class ATL_NO_VTABLE CPropertyListCtrlImpl :
   public ATL::CWindowImpl<T, TBase, TWinTraits>,
   public CCustomDraw<CPropertyListCtrlImpl<T, TBase, TWinTraits> >
{
   //! type of base class
   typedef TBase BaseClass;

public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName()) // use base classes window class

   //! ctor
   CPropertyListCtrlImpl():m_dSplitRatio(0.5){}
   //! dtor
   ~CPropertyListCtrlImpl(){}

protected:
   // message map
   BEGIN_MSG_MAP(CPropertyListCtrl)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() // checks for forgotten REFLECT_NOTIFICATIONS() macro in base class
      CHAIN_MSG_MAP_ALT(CCustomDraw<CPropertyListCtrlImpl>, 1) // hand over messages to custom draw class
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(PLM_FINISH_INPLACE, OnFinishedInplaceEdit)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
      // messsages reflected from parent
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo) // virtual list messages
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ODCACHEHINT, OnOdCacheHint)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINLABELEDIT, OnBeginLabelEdit) // item editing
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ENDLABELEDIT, OnEndLabelEdit)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   //! called when creating list control
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE; // let list control handle WM_CREATE
      return 0;
   }

   //! called when sizing the control
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      bHandled = FALSE;
      int nWidth = static_cast<int>(static_cast<double>(GET_X_LPARAM(lParam)) * m_dSplitRatio);
      // TODO commented out, moves the list control content around
      SetColumnWidth(0, nWidth);
      SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
      return 0;
   }

   //! called to delete inplace edit control
   LRESULT OnFinishedInplaceEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      delete m_pInplaceEdit;
      m_pInplaceEdit = 0;
      return 0;
   }

   //! called when user left-clicks on the list control
   LRESULT OnLeftButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      CPoint pt;
      pt.x = GET_X_LPARAM(lParam);
      pt.y = GET_Y_LPARAM(lParam);

      unsigned int nItem=0, nSubItem=0;
      bool fHitResult = HitTestEx(pt, nItem, nSubItem);

      if (fHitResult && nItem >= 0)
         LeftButtonClick(pt, nItem, nSubItem);

      return 0;
   }

   //! called for virtual lists to retrieve item content
   LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
   {
      // TODO not needed, since we retrieve item text in draw handler
      NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pnmh);

      pDispInfo->item.mask |= LVIF_DI_SETITEM; // store infos and don't ask for them again

      if ((pDispInfo->item.mask & LVIF_TEXT) != 0)
         pDispInfo->item.pszText = _T("");

      return 0;
   }

   //! called to prepare cache for virtual items
   LRESULT OnOdCacheHint(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
   {
      // TODO not needed (?)
      return 0;
   }

   //! called when item edit is about to begin
   LRESULT OnBeginLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
   {
      NMLVDISPINFO* pLvDispInfo = reinterpret_cast<NMLVDISPINFO*>(pnmh);

      // prevent editing group items
      if (IsListItemAPropertyGroup(pLvDispInfo->item.iItem))
         return 1;
      else
      // also prevent editing property names
      if (pLvDispInfo->item.iSubItem == 0)
      {
         SelectItem(pLvDispInfo->item.iItem);
         return 1;
      }
      return 0;
   }

   LRESULT OnEndLabelEdit(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
   {
      return 1;
   }

   //! called when item aspect changed
   LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
   {
      NMLISTVIEW* pNMListView = reinterpret_cast<NMLISTVIEW*>(pnmh);

      // check if state has changed from focused to not focused
      if ((pNMListView->uChanged & LVIF_STATE) != 0 &&
          (pNMListView->uOldState & LVIS_FOCUSED) != 0 &&
          (pNMListView->uNewState & LVIS_FOCUSED) == 0)
      {
         // lost selection; redraw old item; new item is drawn automatically
         Update(pNMListView->iItem);
      }

      return 0;
   }

   // custom-drawing property list control

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
   {
      // get item drawing messages and post-paint messages (TODO needed?)
      return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
   }

   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
   {
      // get item drawing messages for all subitems
      return CDRF_NOTIFYSUBITEMDRAW;
   }

   //! called when drawing subitems
   DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      DWORD nItem = lpNMCustomDraw->dwItemSpec;

      LPNMLVCUSTOMDRAW lpNMLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(lpNMCustomDraw);
      unsigned int nSubItem = static_cast<unsigned int>(lpNMLVCustomDraw->iSubItem);

      CDCHandle dc(lpNMCustomDraw->hdc);
      DrawItem(nItem, nSubItem, dc, lpNMCustomDraw->uItemState);

      return CDRF_SKIPDEFAULT;
   }

private:
   //! ratio of size of first column to second column
   double m_dSplitRatio;
};

} // namespace end


//! property list control
/*! this class can be used as view in CMainFrame or as dynamically created
    control on dialogs
*/
class CPropertyListCtrl :
   public nsPropertyList::CPropertyListCtrlImpl<CPropertyListCtrl, nsPropertyList::CPropertyListCtrlBase>
{
   typedef nsPropertyList::CPropertyListCtrlImpl<CPropertyListCtrl, nsPropertyList::CPropertyListCtrlBase> BaseClass;

public:
   DECLARE_WND_SUPERCLASS(_T("WTL_PropertyListCtrl"), BaseClass::GetWndClassName())
};
