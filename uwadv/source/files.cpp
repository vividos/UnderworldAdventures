/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   functions to create a image from a string, in a specific font. fonts are

*/

// needed includes
#include "common.hpp"
#include "files.hpp"
#include "resource/zziplib/SDL_rwops_zzip.h"
#include <iostream>


// ua_files_manager methods

ua_files_manager::ua_files_manager()
{
}

//! checks if a file with given filename is available
bool ua_file_isavail(std::string base, std::string fname)
{
   std::string filename = base;
   filename += fname;

   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
      return false;

   fclose(fd);
   return true;
}

void ua_files_manager::init(ua_settings &settings)
{
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

   ua_trace("\n");

   // get uadata path
   uadata_path = settings.get_string(ua_setting_uadata_path);

   // get savegame folder name
   {
      std::string savegame_folder(settings.get_string(ua_setting_savegame_folder));

      // replace strings in savegame folder
      replace_system_vars(savegame_folder);

      // store back value
      settings.set_value(ua_setting_savegame_folder,savegame_folder);

      // try to create folder (when not already present)
      ua_trace("creating savegame folder \"%s\"\n",savegame_folder.c_str());
      ua_mkdir(savegame_folder.c_str(),0700);
   }

   // check for available games

   // check for uw1
   {
      settings.set_value(ua_setting_uw1_avail,false);
      settings.set_value(ua_setting_uw1_is_uw_demo,false);

      std::string base = settings.get_string(ua_setting_uw1_path);
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

   // replace %home%
   while( std::string::npos != (pos = path.find("%home%") ) )
   {
      const char* home = getenv("HOME");
      if (home != NULL)
         path.replace(pos,6,home);
      else
         break;
   }

   // replace %uadata%
   while( std::string::npos != (pos = path.find("%uadata%") ) )
      path.replace(pos,8,uadata_path.c_str());
}

void ua_files_manager::init_cfgfiles_list()
{
   cfgfiles_list.clear();


#ifdef CONFIGDIR

   std::string globalcfgfile = CONFIGDIR;
   globalcfgfile += "uwadv.cfg";

   cfgfiles_list.push_back(globalcfgfile);

#endif


#ifdef HAVE_HOME

   std::string homecfgfile;

   const char *homedir = getenv("HOME");
   if(homedir)
   {
      // User has a home directory
      homecfgfile = homedir;

#ifndef BEOS
      homecfgfile += "/.";
#else
      homecfgfile += "/config/settings/";
#endif

      homecfgfile += "uwadv.cfg";
   }
   else
   {
      homecfgfile = "uwadv.cfg";
   }

   cfgfiles_list.push_back(homecfgfile);

#endif

   // last resort: get file from local directory

   cfgfiles_list.push_back("uwadv.cfg");


#if 0
   // some debugging info

   std::cout << "Trying the following config files:" << std::endl;
   std::vector<std::string>::iterator iter;
   for (iter = cfgfiles_list.begin();
        iter != cfgfiles_list.end();
        ++iter)
      std::cout << *iter << std::endl;
   std::cout << std::endl;
#endif
}

SDL_RWops *ua_files_manager::get_uadata_file(const char *relpath)
{
   // first, we search for the real
   std::string filename(uadata_path);
   filename.append(relpath);

   // try to open file
   SDL_RWops *ret = SDL_RWFromFile(filename.c_str(),"rb");

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

int ua_files_manager::load_lua_script(lua_State *L, const char *basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   SDL_RWops *script = get_uadata_file(scriptname.c_str());

   // not found? try binary one
   if (script==NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = get_uadata_file(scriptname.c_str());
   }

   // still not found? exception
   if (script==NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw ua_exception(text.c_str());
   }

   ua_trace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   // load script into buffer
   std::vector<char> buffer;
   unsigned int len=0;
   {
      SDL_RWseek(script,0,SEEK_END);
      len = SDL_RWtell(script);
      SDL_RWseek(script,0,SEEK_SET);

      buffer.resize(len+1,0);

      SDL_RWread(script,&buffer[0],len,1);
      buffer[len]=0;
   }
   SDL_RWclose(script);

   // execute script
   int ret = lua_dobuffer(L,&buffer[0],len,basename);

   if (ret!=0)
      ua_trace("Lua script ended with error code %u\n",ret);

   return ret;
}
