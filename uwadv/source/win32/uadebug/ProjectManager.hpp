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
/// \file ProjectManager.hpp
/// \brief project manager class
//
#pragma once

class ProjectManager
{
public:
   ProjectManager();
   virtual ~ProjectManager();

   bool IsOpen() const { return m_isOpen; }

   void LoadProject(LPCTSTR projectFilename);

   void CloseProject();

   void SaveProject();

   CString GetProjectName() const { return m_projectName; }

protected:
   CString m_projectFilename;
   CString m_projectName;
   bool m_isOpen;
};
