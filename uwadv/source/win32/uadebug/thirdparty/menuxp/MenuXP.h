///////////////////////////////////////////////////////////////////////////////
//
// MenuXP.h
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Draw.h"

namespace WTL
{

template <class T, class TBase>
class ATL_NO_VTABLE CCommandBarCtrlImplXP : public TBase
{
public:
    DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

// Message map and handlers
   typedef CCommandBarCtrlImplXP< T, TBase >    thisClass;
   typedef TBase                                baseClass;

   BEGIN_MSG_MAP(CTabbedMDICommandBarCtrlImplXP)
      CHAIN_MSG_MAP(baseClass)
   ALT_MSG_MAP(1)      // Parent window messages
      NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
      CHAIN_MSG_MAP_ALT(baseClass,1)
   ALT_MSG_MAP(2)      // MDI client window messages
      CHAIN_MSG_MAP_ALT(baseClass,2)
   ALT_MSG_MAP(3)      // Message hook messages
      CHAIN_MSG_MAP_ALT(baseClass,3)
   END_MSG_MAP()

   LRESULT OnCustomDraw (int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
   {
        TCHAR sClass[128];

        GetClassName (pnmh->hwndFrom, sClass, 128);

        if ( _tcscmp (sClass, _T("WTL_CommandBarXP")) )
        {
            return CDRF_DODEFAULT;
        }
        NMCUSTOMDRAW* pCustomDraw = (NMCUSTOMDRAW*)pnmh;

        if ( pCustomDraw->dwDrawStage == CDDS_PREPAINT )
        {
            // Request prepaint notifications for each item
            return CDRF_NOTIFYITEMDRAW;
        }
        if ( pCustomDraw->dwDrawStage == CDDS_ITEMPREPAINT )
        {
            CDCHandle cDC (pCustomDraw->hdc);
            CRect rc = pCustomDraw->rc;
            TCHAR sBtnText[128];

    		::SendMessage (pnmh->hwndFrom, TB_GETBUTTONTEXT, pCustomDraw->dwItemSpec, (LPARAM)sBtnText);

            if ( pCustomDraw->uItemState & CDIS_HOT )
            {
                COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
                CPenDC pen (cDC, crHighLight);
                CBrushDC brush (cDC, (pCustomDraw->uItemState & CDIS_SELECTED) ? HLS_TRANSFORM (crHighLight, +50, -50) : HLS_TRANSFORM (crHighLight, +70, -57));

                cDC.Rectangle (rc);
                cDC.SetTextColor ((pCustomDraw->uItemState & CDIS_SELECTED) ? ::GetSysColor (COLOR_HIGHLIGHTTEXT) : RGB(0,0,0));
            }
            else
            {
                cDC.FillSolidRect (rc, ::GetSysColor (COLOR_3DFACE));
                cDC.SetTextColor (::GetSysColor (m_bParentActive ? COLOR_BTNTEXT : COLOR_3DSHADOW));
            }
            cDC.SetBkMode (TRANSPARENT);
            cDC.SelectFont ((HFONT)GetStockObject (DEFAULT_GUI_FONT));
            cDC.DrawText (sBtnText, _tcslen (sBtnText), rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

            return CDRF_SKIPDEFAULT;
        }
        bHandled = FALSE;

        return CDRF_DODEFAULT;
    }

#define IMGPADDING 6
#define TEXTPADDING 8

// From <winuser.h>
#define OBM_CHECK 32760

	void DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		_MenuItemData* pmd = (_MenuItemData*)lpDrawItemStruct->itemData;
		CDCHandle dc = lpDrawItemStruct->hDC;
		const RECT& rcItem = lpDrawItemStruct->rcItem;
        LPCRECT pRect = &rcItem;
		BOOL bDisabled = lpDrawItemStruct->itemState & ODS_GRAYED;
    	BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bChecked = lpDrawItemStruct->itemState & ODS_CHECKED;
        COLORREF crBackImg = CLR_NONE;
        CDCHandle* pDC = &dc; 

        if ( bSelected )
        {
            COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
            CPenDC pen (*pDC, crHighLight);
            CBrushDC brush (*pDC, crBackImg = bDisabled ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), +73, 0) : HLS_TRANSFORM (crHighLight, +70, -57));

            pDC->Rectangle (pRect);
        }
        else
        {
            CRect rc (pRect);

            rc.right = m_szBitmap.cx+IMGPADDING;
            pDC->FillSolidRect (rc, crBackImg = HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), +20, 0));
            rc.left = rc.right;
            rc.right = pRect->right;
            pDC->FillSolidRect (rc, HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), +75, 0));
        }
        if ( pmd->fType & MFT_SEPARATOR )
        {
            CPenDC pen (*pDC, HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -18, 0));

            pDC->MoveTo (pRect->left+m_szBitmap.cx+IMGPADDING+TEXTPADDING,  (pRect->top+pRect->bottom)/2);
            pDC->LineTo (pRect->right-1, (pRect->top+pRect->bottom)/2);
        }
		else
		{
            CRect rc (pRect);
            CString sCaption = pmd->lpstrText;
            int nTab = sCaption.Find ('\t');

            if ( nTab >= 0 )
            {
                sCaption = sCaption.Left (nTab);
            }
            pDC->SetTextColor (bDisabled ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -18, 0) : ::GetSysColor (COLOR_MENUTEXT));
            pDC->SetBkMode (TRANSPARENT);

            CBoldDC bold (*pDC, (lpDrawItemStruct->itemState & ODS_DEFAULT) != 0);

            rc.left = m_szBitmap.cx+IMGPADDING+TEXTPADDING;
            pDC->DrawText (sCaption, sCaption.GetLength(), rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

            if ( nTab >= 0 )
            {    
                rc.right -= TEXTPADDING+4;
                pDC->DrawText (pmd->lpstrText+nTab+1, _tcslen (pmd->lpstrText+nTab+1), rc, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
            }
            if ( bChecked  )
            {
                COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
                CPenDC pen (*pDC, crHighLight);
                CBrushDC brush (*pDC, crBackImg = bDisabled ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), +73, 0) :
                                                              (bSelected ? HLS_TRANSFORM (crHighLight, +50, -50) : HLS_TRANSFORM (crHighLight, +70, -57)));

                pDC->Rectangle (CRect (pRect->left+1, pRect->top+1, pRect->left+m_szButton.cx-2, pRect->bottom-1));
            }
			if ( m_hImageList != NULL && pmd->iButton >= 0 )
            {
                bool bOver = !bDisabled && bSelected;

                if ( bDisabled || (bSelected && !bChecked) )
                {
                    HICON hIcon = ImageList_ExtractIcon (NULL, m_hImageList, pmd->iButton);
                    CBrush brush;

                    brush.CreateSolidBrush (bOver ? HLS_TRANSFORM (::GetSysColor (COLOR_HIGHLIGHT), +50, -66) : HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -27, 0));
                    pDC->DrawState (CPoint (pRect->left + ( bOver ? 4 : 3 ), rc.top + ( bOver ? 5 : 4 )),
                                    CSize (m_szBitmap.cx, m_szBitmap.cx), hIcon, DSS_MONO, brush);
                    DestroyIcon (hIcon);
                }
                if ( !bDisabled )
                {
                    ::ImageList_Draw (m_hImageList, pmd->iButton, pDC->m_hDC,
                                      pRect->left+( (bSelected && !bChecked) ? 2 : 3 ), rc.top+( (bSelected && !bChecked) ? 3 : 4 ), ILD_TRANSPARENT);
                }
            }
            else if ( bChecked )
            {
                // Draw the check mark
                rc.left  = pRect->left+5;
                rc.right = rc.left + m_szBitmap.cx+IMGPADDING;
                pDC->SetBkColor (crBackImg);
                HBITMAP hBmp = LoadBitmap (NULL, MAKEINTRESOURCE(OBM_CHECK));
                pDC->DrawState (CPoint (rc.left,rc.top+3), CSize(rc.Size()), hBmp, DSS_NORMAL, (HBRUSH)NULL);
                DeleteObject (hBmp);
            }
		}
    }
};


// SDI Command Bar
class CCommandBarCtrlXP : public CCommandBarCtrlImplXP< CCommandBarCtrlXP, CCommandBarCtrl >
{
public:
    DECLARE_WND_SUPERCLASS(_T("WTL_CommandBarXP"), GetWndClassName())
};


// MDI Command Bar
class CMDICommandBarCtrlXP : public CCommandBarCtrlImplXP< CMDICommandBarCtrlXP, CMDICommandBarCtrl >
{
public:
    DECLARE_WND_SUPERCLASS(_T("WTL_MDICommandBarXP"), GetWndClassName())
};


// Tabbed MDI Command Bar
class CTabbedMDICommandBarCtrlXP : public CCommandBarCtrlImplXP< CTabbedMDICommandBarCtrlXP, CTabbedMDICommandBarCtrlImpl< CTabbedMDICommandBarCtrlXP, CCommandBarCtrlBase, CControlWinTraits > >
{
public:
    DECLARE_WND_SUPERCLASS(_T("WTL_TabbedMDICommandBarXP"), GetWndClassName())
};


}; //namespace WTL
