/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file files.cpp

   \brief resource files management

*/

// needed includes
#include "common.hpp"
#include "files.hpp"
#include "zziplib/SDL_rwops_zzip.h"


// ua_files_manager methods

ua_files_manager::ua_files_manager()
{
}

//! checks if a file with given filename is available
bool ua_file_isavail(const std::string& base, const char* fname)
{
   std::string filename = base;
   filename += fname;

   return ua_file_exists(filename.c_str());
}

void ua_files_manager::init(ua_settings& settings)
{
   ua_trace("initializing files manager\n");

   // init config files list
   init_cfgfiles_list();

   // load all settings
   bool foundconfig = false;

   ua_trace("loading all config files ...\n");

   unsigned int max = cfgfiles_list.size();
   for(unsigned int i=0; i<max; i++)
   {
      bool fileok = true;
      try {
         ua_trace("loading %s\n",cfgfiles_list[i].c_str());
         settings.load(cfgfiles_list[i].c_str());
      } catch (ua_exception e) {
         fileok = false;
      }
      foundconfig |= fileok;
   }

   if (!foundconfig)
      throw ua_exception("could not find any config files");

   // check "uadata-path"
   {
      // get uadata path
      uadata_path = settings.get_string(ua_setting_uadata_path);

      // check if path ends with slash
      std::string::size_type pos = uadata_path.find_last_of("\\/");
      if (pos != std::string::npos && pos != uadata_path.size()-1)
      {
         // add slash and write back
         uadata_path.append("/");
         settings.set_value(ua_setting_uadata_path,uadata_path);
      }
   }

   // get savegame folder name
   {
      std::string savegame_folder(settings.get_string(ua_setting_savegame_folder));

      // replace strings in savegame folder
      replace_system_vars(savegame_folder);

      // check if path ends with slash
      std::string::size_type pos = savegame_folder.find_last_of("\\/");
      if (pos != std::string::npos && pos != savegame_folder.size()-1)
      {
         // add slash
         savegame_folder.append("/");
      }

      // store back value
      settings.set_value(ua_setting_savegame_folder,savegame_folder);

      // try to create folder (when not already present)
      ua_trace("creating savegame folder \"%s\"\n",savegame_folder.c_str());
      ua_mkdir(savegame_folder.c_str(),0700);
   }

   // expand variable placeholders (%uahome%, etc.) in custom keymap value
   {
      std::string custom_keymap(settings.get_string(ua_setting_custom_keymap));

      // replace strings in savegame folder
      replace_system_vars(custom_keymap);

      // store back value
      settings.set_value(ua_setting_custom_keymap,custom_keymap);
   }

   // check for available games

   // check for uw1
   {
      settings.set_value(ua_setting_uw1_avail,false);
      settings.set_value(ua_setting_uw1_is_uw_demo,false);

      std::string base = settings.get_string(ua_setting_uw1_path);

      // check if path ends with slash
      std::string::size_type pos = base.find_last_of("\\/");
      if (pos != std::string::npos && pos != base.size()-1)
      {
         // add slash and write back
         base.append("/");
         settings.set_value(ua_setting_uw1_path,base);
      }

      if (!base.empty())
      {
         if (ua_file_isavail(base,"data/cnv.ark") &&
             ua_file_isavail(base,"data/strings.pak") &&
             ua_file_isavail(base,"data/pals.dat") &&
             ua_file_isavail(base,"data/allpals.dat"))
         {
            // could be uw1 or uw_demo
            if (ua_file_isavail(base,"uw.exe")||ua_file_isavail(base,"uwdemo.exe"))
            {
               settings.set_value(ua_setting_uw1_avail,true);

               // check if we only have the demo
               if (ua_file_isavail(base,"data/level13.st") &&
                   ua_file_isavail(base,"data/level13.anx") &&
                   ua_file_isavail(base,"data/level13.txm"))
               {
                  settings.set_value(ua_setting_uw1_is_uw_demo,true);
               }
            }
         }
      }
   }

   // check for uw2
   {
      settings.set_value(ua_setting_uw2_avail,false);

      std::string base = settings.get_string(ua_setting_uw2_path);

      // check if path ends with slash
      std::string::size_type pos = base.find_last_of("\\/");
      if (pos != std::string::npos && pos != base.size()-1)
      {
         // add slash and write back
         base.append("/");
         settings.set_value(ua_setting_uw2_path,base);
      }

      if (!base.empty())
      {
         if (ua_file_isavail(base,"data/cnv.ark") &&
             ua_file_isavail(base,"data/strings.pak") &&
             ua_file_isavail(base,"data/pals.dat") &&
             ua_file_isavail(base,"data/allpals.dat") &&
             ua_file_isavail(base,"uw2.exe") &&
             ua_file_isavail(base,"data/t64.tr"))
         {
            settings.set_value(ua_setting_uw2_avail,true);
         }
      }
   }

   ua_trace("\n");
}

void ua_files_manager::replace_system_vars(std::string& path)
{
   std::string::size_type pos = 0;

   // replace ~ at start of path
   if ( 0 == path.find("~") )
   {
      const char* home = getenv("HOME");
      if (home != NULL)
         path.replace(pos,1,home);
   }

   // replace %uahome%
   while( std::string::npos != (pos = path.find("%uahome%") ) )
      path.replace(pos,8,uahome_path.c_str());

   // replace %uadata%
   while( std::string::npos != (pos = path.find("%uadata%") ) )
      path.replace(pos,8,uadata_path.c_str());

   // replace any double-slashes occured from previous replacements
   while( std::string::npos != (pos = path.find("//") ) ||
          std::string::npos != (pos = path.find("\\\\") )    )
      path.replace(pos,2,"/");
}

void ua_files_manager::init_cfgfiles_list()
{
   // determine uahome path
   {
#ifdef HAVE_HOME

      const char *homedir = getenv("HOME");
      if (homedir != NULL)
      {
         // User has a home directory
         uahome_path = homedir;

#ifndef BEOS
         uahome_path += "/.uwadv/";
#else
         uahome_path += "/config/settings/uwadv/";
#endif
      }

      // try to create home folder
      ua_trace("creating uahome folder \"%s\"\n",uahome_path.c_str());
      ua_mkdir(uahome_path.c_str(),0700);

#else // !HAVE_HOME

      // assume current working folder as home dir
      uahome_path = "./";

#endif // HAVE_HOME
   }

   // set up config files list

   cfgfiles_list.clear();

#ifdef CONFIGDIR

   std::string globalcfgfile = CONFIGDIR;
   globalcfgfile += "uwadv.cfg";

   cfgfiles_list.push_back(globalcfgfile);

#endif

   // add cfg file in uahome path
   std::string homecfgfile(uahome_path);
   homecfgfile += "uwadv.cfg";
   cfgfiles_list.push_back(homecfgfile);
}

SDL_RWops* ua_files_manager::get_uadata_file(const char* relpath)
{
   // first, we search for the real
   std::string filename(uadata_path);
   filename.append(relpath);

   // try to open file
   SDL_RWops* ret = SDL_RWFromFile(filename.c_str(),"rb");

   if (ret == NULL)
   {
      // not found

      // try all uadata resource files
      unsigned int uacount=0;
      bool found=true;
      do
      {
         // try to open file
         char buffer[256];
         sprintf(buffer,"uadata%02u.uar",uacount);

         std::string uarfile(uadata_path);
         uarfile.append(buffer);

         // check if file can be found
         FILE *fd = fopen(uarfile.c_str(),"rb");
         found = (fd != NULL);
         if (fd != NULL) fclose(fd);

         if (found)
         {
            // try to open from zip file
            std::string zippath(uarfile);
            zippath.append("/");
            zippath.append(relpath);

            ret = SDL_RWFromZZIP(zippath.c_str(),"rb");
            if (ret != NULL)
               break;
         }

         uacount++;
      }
      while(found);
   }

   return ret;
}
