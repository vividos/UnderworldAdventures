#include "pch.hpp"
#include "OutputDockingWindow.hpp"

LRESULT OutputDockingWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_outputWindow.Create(m_hWnd);

   return 0;
}

LRESULT OutputDockingWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      RECT rc;
      GetClientRect(&rc);
      m_outputWindow.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   bHandled = FALSE;
   return 1;
}

LRESULT OutputDockingWindow::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (m_outputWindow.m_hWnd != nullptr &&
      m_outputWindow.IsWindowVisible())
      m_outputWindow.SetFocus(true);

   bHandled = FALSE;
   return 1;
}
