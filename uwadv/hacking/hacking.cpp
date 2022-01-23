//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2019,2021 Underworld Adventures Team
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
// hacking main function
//

// all main functions
extern int bytdecode_main();
extern int cnvdec_main();
extern int critdec_main();
extern int cutsdec_main();
extern int fontdecode_main();
extern int grdecode_main();
extern int levarkdec_main();
extern int miscdecode_main();
extern int sgdecode_main();
extern int trdecode_main();
extern int uwmdldec_main();

/// main function
int main()
{
   //return bytdecode_main();
   return cnvdec_main();
   //return critdec_main();
   //return cutsdec_main();
   //return fontdecode_main();
   //return grdecode_main();
   //return levarkdec_main();
   //return miscdecode_main();
   //return sgdecode_main();
   //return trdecode_main();
   //return uwmdldec_main();
}
