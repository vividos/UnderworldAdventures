//
// Property List Control
// Copyright (C) 2005 Michael Fink
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.
//
/// \file PropertyListCtrl.hpp
/// \brief Property List Control
/// \details
///
/// Copyright (C) 2005,2019 Michael Fink
///
/// A property list control presents properties to the use which the user may
/// edit. The properties can be grouped together by topic, and groups can be
/// collapsed and expanded for better overview.
///
/// A good example for a property list control is the "Properties" tab in
/// Microsoft Visual Studio .NET 2003.
///
/// Usage:
///
/// In any case, add add property group and property item lists to the file where
/// you will create your property list control, as follows:
///
/// For groups:
///
///    enum PropertyGroup
///    {
///       group1=0,
///       group2,
///    };
///
///    BEGIN_PROPERTY_GROUP_LIST(g_propertyGroupList)
///       PROPERTY_GROUP(group1, PROPERTY_STRING(_T("Group 1")))
///       PROPERTY_GROUP(group2, PROPERTY_STRING(_T("Group 1")))
///    END_PROPERTY_GROUP_LIST()
///
/// For properties (note that every property must get its own unique id):
///
///    enum PropertyItem
///    {
///       prop1=0,
///       prop2,
///       prop3,
///       prop4,
///       prop5
///    };
///
///    BEGIN_PROPERTY_ITEM_LIST(g_propertyItemList)
///       PROPERTY_ITEM(prop1, group1, propertyTypeText, PROPERTY_STRING(_T("Prop1")), PROPERTY_STRING(_T("Modifies prop1")))
///       PROPERTY_ITEM(prop2, group1, propertyTypeText, PROPERTY_STRING(_T("Prop2")), PROPERTY_STRING(_T("Modifies prop2")))
///       PROPERTY_ITEM(prop3, group2, propertyTypeText, PROPERTY_STRING(_T("Prop3")), PROPERTY_STRING(_T("Modifies prop3")))
///       PROPERTY_ITEM(prop4, group2, propertyTypeText, PROPERTY_STRING(_T("Prop4")), PROPERTY_STRING(_T("Modifies prop4")))
///       PROPERTY_ITEM(prop5, group2, propertyTypeText, PROPERTY_STRING(_T("Prop5")), PROPERTY_STRING(_T("Modifies prop5")))
///    END_PROPERTY_ITEM_LIST()
///
/// If you want to use string resource ids instead of putting the strings directly
/// in your source files, use the PROPERTY_STRING_ID macro, as follows:
///
///    PROPERTY_STRING_ID(IDS_PROP_PROP1)
///
/// Depending on how you want to use the property list control, you have to use
/// the control in different ways:
///
/// 1. Use the class PropertyListCtrl as a view class in your SDI or MDI
///    application:
///
///    In MainFrame, add or replace:
///
///       PropertyListCtrl m_view;
///
///    In MainFrame, add this to the bottom of the message map:
///
///       [...]
///       REFLECT_NOTIFICATIONS()
///    END_MSG_MAP()
///
///    This ensures that notifications are reflected back from the parent class
///    (MainFrame) to the property list control.
///
///    In MainFrame::OnCreate() add or replace:
///
///       m_hWndClient = m_view.Create(m_hWnd, rcDefault);
///       m_view.AddGroups(g_propertyGroupList);
///       m_view.AddItems(g_propertyItemList);
///       m_view.Init();
///
/// 2. Use the class as a dynamic control in a dialog (e.g. derived from
///    CDialogImpl):
///
///    In your dialog class, add a private member:
///
///       PropertyListCtrl m_propertyList;
///
///    In your dialog class, add this to the bottom of the message map, again to
///    reflect notifications back to the control:
///
///       [...]
///       REFLECT_NOTIFICATIONS()
///    END_MSG_MAP()
///
///    In CMyDialog::OnInitInstance(), add the following:
///
///       CRect rect;
///       GetDlgItem(IDC_MYFRAME).GetWindowRect(rect);
///       ScreenToClient();
///
///       m_propertyList.Create(m_hWnd, rect);
///       m_propertyList.AddGroups(g_propertyGroupList);
///       m_propertyList.AddItems(g_propertyItemList);
///       m_propertyList.Init();
///
///    This creates your control on the dialog where you define an invisible
///    control, e.g. a picture frame control named IDC_MYFRAME. The frame is used
///    to position the control on the dialog.
///
/// All property values are kept in an internal default storage that stores all
/// property values as strings. You can access the properties with the two methods
/// GetPropertyItemValue() and SetPropertyItemValue(). They take a property id.
///
/// If you want to change the storage of your property item values, you can derive
/// a class from the interface IPropertyStorage and implement all pure virtual
/// methods. Before calling m_view.Init(), set the new storage per
///
///    m_view.SetStorage(pMyPropertyStorage).
///
/// If you want to implement own property types, consider looking at the property
/// type "propertyTypeCustom", the interface IPropertyItemCustom and the
/// interface IInplaceEditControl, but it requires a bit understanding what is
/// going on in the control.
///
/// Note: the control doesn't support "dynamic properties" that Microsoft Visual
/// Studio supports (e.g. seen on the "Events" page of the "Properties" window for
/// a class in Class View).
//
#pragma once

#include <atlcoll.h>
#include <atlmisc.h>

/// type a property can have
enum PropertyType
{
   propertyTypeText = 0,   ///< text; uses inplace edit control
   propertyTypeComboBox, ///< choice; uses inplace combobox control

   propertyTypeCustom,   ///< custom property; uses IPropertyItemCustom to customize property
};


namespace PropertyList
{
#define IDC_PROPGRID_CATEGORIZED 1024
#define IDC_PROPGRID_ALPHABETICAL 1025

   /// struct used to hold a string or a string resource id
   struct StringOrId
   {
      UINT m_stringId;    ///< string id, or -1 when unused
      LPCTSTR m_string; ///< string, or NULL when unused
   };

   // defines a property string by giving the string itself
#define PROPERTY_STRING(str) { UINT(-1), str }

// defines a property string by giving its string resource id
#define PROPERTY_STRING_ID(str_id) { str_id, NULL }

// ********************
// Property Group infos
// ********************

/// information about a property group item
   struct PropertyGroupInfo
   {
      UINT m_propertyGroupId;
      StringOrId m_propertyGroupName;
      bool m_isExpanded;
   };

   // macros to define property group lists

   // property group list start
#define BEGIN_PROPERTY_GROUP_LIST(var) \
   static PropertyList::PropertyGroupInfo var[] = {

// property group item
#define PROPERTY_GROUP(id, name_string_or_id) \
   { id, name_string_or_id, true },

// property group list end
#define END_PROPERTY_GROUP_LIST() { UINT(-1), PROPERTY_STRING_ID(UINT(-1)) } };


// *******************
// Property item infos
// *******************

   class IPropertyItemCustom;

   /// information about a property item
   struct PropertyItemInfo
   {
      /// proprety item id; must be unique throughout the property list
      UINT m_propertyId;
      /// property group id from a PropertyGroupInfo struct
      UINT m_propertyGroupId;

      /// property type
      PropertyType m_propertyType;
      /// custom item type interface; used when property type is propertyTypeCustom
      IPropertyItemCustom* m_customProperty;

      /// string or string id of property item name
      StringOrId m_propertyItemName;
      /// string or string id of property item description text
      StringOrId m_propertyItemDesc;
   };

   // macros to define property item lists

   // property group list start
#define BEGIN_PROPERTY_ITEM_LIST(var) \
   static PropertyList::PropertyItemInfo var[] = {

// property item
#define PROPERTY_ITEM(id, group_id, type, name_string_or_id, desc_string_or_id) \
   { id, group_id, type, NULL, name_string_or_id, desc_string_or_id },

// custom property item
#define PROPERTY_ITEM_CUSTOM(id, group_id, interface_ptr, name_string_or_id, desc_string_or_id) \
   { id, group_id, propertyTypeCustom, interface_ptr, name_string_or_id, desc_string_or_id },

// property group list end
#define END_PROPERTY_ITEM_LIST() \
   { UINT(-1), UINT(-1), propertyTypeCustom, NULL, PROPERTY_STRING_ID(UINT(-1)), PROPERTY_STRING_ID(UINT(-1)) }\
};


/// property item data; represents an item in the list control
   struct PropertyData
   {
      /// ctor
      PropertyData() :m_isGroup(true), m_propertyGroupOrItemId(UINT(-1)) {}

      /// is list item a property group item?
      bool m_isGroup;

      /// property group id or property id, depending on m_isGroup
      UINT m_propertyGroupOrItemId;
   };


   /// \brief interface to property storage
   /// reimplement this interface and call SetStorage() to have alternate property storage
   class IPropertyStorage
   {
   public:
      /// dtor
      virtual ~IPropertyStorage() {}

      /// retrieves property value for given property id
      virtual void GetProperty(unsigned int propertyItemId, CString& propertyValue) = 0;

      /// sets new property value for given property id
      virtual void SetProperty(unsigned int propertyItemId, const CString& propertyValue) = 0;

      /// verifies if property value for given property id is correct
      virtual bool VerifyProperty(unsigned int propertyItemId, const CString& propertyValue)
      {
         (void)propertyItemId;
         (void)propertyValue;
         return true;
      }
   };


   /// default storage for properties; just stores property values as strings
   class PropertyStorageDefault : public IPropertyStorage
   {
   public:
      virtual void GetProperty(unsigned int propertyItemId, CString& propertyValue)
      {
         if (!m_mapProperties.Lookup(propertyItemId, propertyValue))
         {
            propertyValue.Empty();
            m_mapProperties.SetAt(propertyItemId, propertyValue);
         }
      }

      virtual void SetProperty(unsigned int propertyItemId, const CString& propertyValue)
      {
         m_mapProperties.SetAt(propertyItemId, propertyValue);
      }

   private:
      /// map with all properties stored as string
      CAtlMap<unsigned int, CString> m_mapProperties;
   };


#define PLM_FINISH_INPLACE (WM_USER + 200)

   /// inplace edit control callback interface
   class IInplaceParentCallback
   {
   public:
      /// ctor
      virtual ~IInplaceParentCallback() {}

      /// ends label edit and returns new property value; returns false when new value is not accepted
      virtual bool EndLabelEdit(const CString& value) = 0;

      /// notices list control that inplace edit control is finished and can be deleted
      virtual void NoticeFinish(class IInplaceEditControl* inplaceEdit) = 0;
   };

   /// interface for inplace edit controls
   class IInplaceEditControl
   {
   public:
      /// ctor
      IInplaceEditControl() :m_inplaceParentCallback(NULL) {}

      /// dtor
      virtual ~IInplaceEditControl() noexcept {}

      /// initializes inplace edit control
      void Init(IInplaceParentCallback* inplaceParentCallback)
      {
         m_inplaceParentCallback = inplaceParentCallback;
      }

      /// creates inplace edit control
      virtual void Create(HWND hWnd, CRect rect, LPCTSTR propertyValue) = 0;

   protected:
      /// callback to inplace parent class
      IInplaceParentCallback* m_inplaceParentCallback;
   };


   /// text edit control for editing text properties
   class InplaceTextEditControl :
      public CWindowImpl<InplaceTextEditControl, CEdit>,
      public IInplaceEditControl
   {
      typedef CWindowImpl<InplaceTextEditControl, CEdit> BaseClass;
   public:
      /// ctor
      InplaceTextEditControl() :m_isFinished(false) {}

      virtual void Create(HWND hWnd, CRect rect, LPCTSTR propertyValue);

      BEGIN_MSG_MAP(InplaceTextEditControl)
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
      bool m_isFinished;
   };


   /// interface for custom property item types
   class IPropertyItemCustom
   {
   public:
      /// dtor
      virtual ~IPropertyItemCustom() {}

      /// custom drawing for custom item; returns true when item was custom-drawn
      virtual bool DrawItem(CRect rect) = 0;

      /// creates an inplace edit control for use when editing the property
      virtual IInplaceEditControl* CreateInplaceEditControl() = 0;
   };


   /// base class for property list; implements use-independent functions
   class PropertyListCtrlBase :
      public CListViewCtrl,
      public IInplaceParentCallback
   {
   public:
      /// ctor
      PropertyListCtrlBase();
      virtual ~PropertyListCtrlBase() {}

      /// deletes all property groups
      void DeleteAllGroups();
      /// deletes all property items
      void DeleteAllItems();

      /// dynamically adds property groups
      void AddGroups(PropertyGroupInfo* groupInfo);
      /// dynamically adds property items
      void AddItems(PropertyItemInfo* itemInfo);

      /// sets storage interface
      void SetStorage(IPropertyStorage* storage) { m_storage = storage; }

      /// returns current storage interface
      IPropertyStorage* GetStorage() const { return m_storage; }

      /// initializes property list; call after adding groups and property items
      void Init();

      /// sets read-only state of list control
      bool SetReadOnly(bool readonly);

      /// updates all values from storage
      void UpdateValues();

      /// sort order
      enum ListSortOrder
      {
         PLSO_CATEGORIZED = 0, ///< uses groups to categorize property items
         PLSO_ALPHABETICAL = 1 ///< uses alphabetical sorting of property items; doesn't usegroups
      };

      /// sets new sort order
      void SetSortOrder(UINT sortOrder)
      {
         m_sortOrder = sortOrder;
         BuildPropertiesList();
         // TODO redraw?
      }

      /// expands an item that is a group
      void ExpandGroupItem(UINT listItemIndex, bool expand = true);
      /// expands a group item
      void ExpandGroup(UINT propertyGroupId, bool expand = true);

      /// determines if a given list item is a property group
      bool IsListItemAPropertyGroup(unsigned int listItemIndex) const;
      /// returns property group id of given list item; item must be a group item
      UINT GetListItemPropertyGroupId(unsigned int listItemIndex) const;
      /// returns property id of given list item; item must be a property item
      UINT GetListItemPropertyItemId(unsigned int listItemIndex) const;

      /// determines if a group item is expanded
      bool IsPropertyGroupExpanded(unsigned int listItemIndex) const;

      /// returns property group name by given property group id
      CString GetPropertyGroupName(UINT propertyGroupId) const;
      /// returns property item name by given property id
      CString GetPropertyItemName(UINT propertyItemId) const;
      /// returns description string of property with given property id
      CString GetPropertyItemDesc(UINT propertyItemId) const;

      /// returns property value for given property id
      CString GetPropertyItemValue(UINT propertyItemId) const
      {
         ATLASSERT(m_storage != NULL); // called Init() or SetStorage()?
         CString value;
         m_storage->GetProperty(propertyItemId, value);
         return value;
      }

      /// sets property value for given property id
      void SetPropertyItemValue(UINT propertyItemId, const CString& value)
      {
         ATLASSERT(m_storage != NULL); // called Init() or SetStorage()?
         m_storage->SetProperty(propertyItemId, value);
      }

      void LeftButtonClick(CPoint pt, unsigned int item, unsigned int subItem);

   protected:
      /// rebuilds properties list depending on sort order
      void BuildPropertiesList();

      /// does hit-testing for a specific subitem; returns false when no subitem was hit
      bool HitTestEx(CPoint point, unsigned int& item, unsigned int& subItem);

      /// retrieves rectangle of given subitem
      void GetSubItemRect(unsigned int item, unsigned int subItem, CRect& rect);

      /// starts editing a property
      void EditProperty(unsigned int listItem);

      /// creates edit control for given property id
      IInplaceEditControl* CreateEditControl(UINT propertyItemId);

      // retrieves string by struct that either contains string or string resource id
      CString GetString(const StringOrId& stringOrId) const;

      /// returns rect of plus/minus box; takes item rect
      void GetPlusMinusBoxRect(CRect& rect);

      /// draw list control item
      void DrawItem(unsigned int item, unsigned int subItem, CDCHandle dc, UINT itemState);
      /// draws group item area
      void DrawGroupItem(unsigned int item, CDCHandle dc, UINT itemState);
      /// draws property item name column
      void DrawPropertyNameItem(unsigned int item, CDCHandle dc, UINT itemState);
      /// draws property item value column
      void DrawPropertyValueItem(unsigned int item, CDCHandle dc, UINT itemState);

      // virtual methods from IInplaceParentCallback
      virtual bool EndLabelEdit(const CString& value) override;
      virtual void NoticeFinish(IInplaceEditControl* inplaceEdit) override;

   protected:
      /// normal text
      CFont m_fontNormal;
      /// bold text for group items
      CFont m_fontGroupItem;

      /// background color; used for group items and left bar
      COLORREF m_crGrayBackground;

      /// height of list item
      int m_itemHeight;

      /// size of left area
      unsigned int m_leftAreaSize;

      /// pointer to property storage
      IPropertyStorage* m_storage;

      /// number of item currently inplace-edited
      unsigned int m_inplaceEditItem;

      /// current inplace edit control
      IInplaceEditControl* m_inplaceEdit;

      /// indicates if list control is read-only
      bool m_isReadOnly;

   private:
      /// mapping of all group ids to group info structs
      CAtlMap<UINT, PropertyGroupInfo> m_mapAllGroups;
      /// mapping of all property ids to property info structs
      CAtlMap<UINT, PropertyItemInfo> m_mapAllItems;

      /// array with all property item data structs
      CAtlArray<PropertyData> m_allProperties;

      /// default storage when no other storage was set using SetStorage()
      PropertyStorageDefault m_defaultStorage;

      /// sort order; see ListSortOrder
      unsigned int m_sortOrder;

      /// image list to properly size list item
      CImageList m_imageListSizeIcon;
   };


   /// window style traits for property list control
   typedef CWinTraitsOR<
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
      LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_EDITLABELS | LVS_OWNERDATA,
      WS_EX_CLIENTEDGE> CPropertyListCtrlWinTraits;

   /// property list control implementation
   template <class T,
      class TBase = PropertyListCtrlBase,
      class TWinTraits = CPropertyListCtrlWinTraits>
      class ATL_NO_VTABLE PropertyListCtrlImpl :
      public ATL::CWindowImpl<T, TBase, TWinTraits>,
      public CCustomDraw<PropertyListCtrlImpl<T, TBase, TWinTraits> >
   {
      /// type of base class
      typedef TBase BaseClass;

   public:
      DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName()) // use base classes window class

      /// ctor
      PropertyListCtrlImpl() :m_splitRatio(0.5) {}
      /// dtor
      ~PropertyListCtrlImpl() {}

   protected:
      // message map
      BEGIN_MSG_MAP(PropertyListCtrl)
         ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() // checks for forgotten REFLECT_NOTIFICATIONS() macro in base class
         CHAIN_MSG_MAP_ALT(CCustomDraw<PropertyListCtrlImpl>, 1) // hand over messages to custom draw class
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

         /// called when creating list control
      LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
      {
         bHandled = FALSE; // let list control handle WM_CREATE
         return 0;
      }

      /// called when sizing the control
      LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
      {
         bHandled = FALSE;
         int width = static_cast<int>(static_cast<double>(GET_X_LPARAM(lParam)) * m_splitRatio);
         // TODO commented out, moves the list control content around
         SetColumnWidth(0, width);
         SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
         return 0;
      }

      /// called to delete inplace edit control
      LRESULT OnFinishedInplaceEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
      {
         delete m_inplaceEdit;
         m_inplaceEdit = 0;
         return 0;
      }

      /// called when user left-clicks on the list control
      LRESULT OnLeftButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
      {
         CPoint pt;
         pt.x = GET_X_LPARAM(lParam);
         pt.y = GET_Y_LPARAM(lParam);

         unsigned int item = 0, subItem = 0;
         bool hitResult = HitTestEx(pt, item, subItem);

         if (hitResult && item >= 0)
            LeftButtonClick(pt, item, subItem);

         return 0;
      }

      /// called for virtual lists to retrieve item content
      LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
      {
         NMLVDISPINFO* dispInfo = reinterpret_cast<NMLVDISPINFO*>(pnmh);

         dispInfo->item.mask |= LVIF_DI_SETITEM; // store infos and don't ask for them again

         if ((dispInfo->item.mask & LVIF_TEXT) != 0)
            dispInfo->item.pszText = _T("");

         return 0;
      }

      /// called to prepare cache for virtual items
      LRESULT OnOdCacheHint(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
      {
         // TODO not needed (?)
         return 0;
      }

      /// called when item edit is about to begin
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

      /// called when item aspect changed
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
         // get item drawing messages
         return CDRF_NOTIFYITEMDRAW /*| CDRF_NOTIFYPOSTPAINT*/;
      }

      DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
      {
         // get item drawing messages for all subitems
         return CDRF_NOTIFYSUBITEMDRAW;
      }

      /// called when drawing subitems
      DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
      {
         DWORD item = lpNMCustomDraw->dwItemSpec;

         LPNMLVCUSTOMDRAW lpNMLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(lpNMCustomDraw);
         unsigned int subItem = static_cast<unsigned int>(lpNMLVCustomDraw->iSubItem);

         CDCHandle dc(lpNMCustomDraw->hdc);
         DrawItem(item, subItem, dc, lpNMCustomDraw->uItemState);

         return CDRF_SKIPDEFAULT;
      }

   private:
      /// ratio of size of first column to second column
      double m_splitRatio;
   };

} // namespace end


/// \brief property list control
/// this class can be used as view in MainFrame or as dynamically created
/// control on dialogs
class PropertyListCtrl :
   public PropertyList::PropertyListCtrlImpl<PropertyListCtrl, PropertyList::PropertyListCtrlBase>
{
   typedef PropertyList::PropertyListCtrlImpl<PropertyListCtrl, PropertyList::PropertyListCtrlBase> BaseClass;

public:
   DECLARE_WND_SUPERCLASS(_T("WTL_PropertyListCtrl"), BaseClass::GetWndClassName())
};
