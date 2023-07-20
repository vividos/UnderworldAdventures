#pragma once

#include "OutputWindow.hpp"

class OutputDockingWindow : public DockingWindowBase
{
   typedef OutputDockingWindow ThisClass;
   typedef DockingWindowBase BaseClass;

public:
   /// ctor
   OutputDockingWindow()
      :BaseClass(idOutputWindow)
   {
   }

   DECLARE_DOCKING_WINDOW(_T("Output"), CSize(200, 100)/*docked*/, CSize(500, 150)/*floating*/, dockwins::CDockingSide::sBottom);

   DECLARE_WND_CLASS_EX(_T("OutputDockingWindow"), CS_DBLCLKS, COLOR_WINDOW)

   BEGIN_MSG_MAP(ThisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      CHAIN_MSG_MAP(BaseClass)
      REFLECT_NOTIFICATIONS()
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

private:
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
   /// output window
   OutputWindow m_outputWindow;
};
