//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2021 Underworld Adventures Team
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
/// \file uaconfig.cpp
/// \brief Underworld Adventures win32 config application
//
#include "ConfigDlg.hpp"

CComModule _Module;

/// main function
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
   LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
   HRESULT res = _Module.Init(NULL, hInstance);
   ATLVERIFY(SUCCEEDED(res));

   ConfigDlg cfg;
   cfg.RunDialog();

   _Module.Term();

   return 0;
}
