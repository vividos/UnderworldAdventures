--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002 Michael Fink
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--
-- $Id$
--

--
-- cutscene.lua - lua cutscene script; controls the cutscene sequence
--

--
-- registered C functions:
--
-- cuts_do_action(self,actioncode,actionvalue)
--   actioncode is one of the values below; actionvalue can be
--   a value depending on the action. it always must be present.
--


-- constants

-- action codes that can be passed to cuts_do_action()
-- these must be in sync with the code in "source/screens/cutscene_view.cpp"
cuts_finished = 0            -- indicates that cutscene is at it's end
cuts_set_string_block = 1    -- sets game strings block to use
cuts_sound_play = 2          -- plays a sound, "sound/XX.voc"

cuts_text_set_color = 3      -- sets new text color; value is color index
cuts_text_fadein = 4         -- fades in text; value is string number
cuts_text_fadeout = 5        -- fades out text
cuts_text_show = 6           -- shows text instantly; value is string number
cuts_text_hide = 7           -- hides text instantly

cuts_anim_fadein = 8         -- loads animation and fades in first frame
cuts_anim_show = 9           -- loads animation and starts it
cuts_anim_set_stopframe = 10 -- sets frame to stop at; value is stopframe
cuts_anim_fadeout = 11       -- fades out current animation frame
cuts_anim_stop = 12          -- stops animation looping
cuts_anim_hide = 13          -- hides animation

-- string block base for cutscenes
cuts_strbase = 12*256


-- tables

-- cutscene timetables
cutscenes = {
   [0] =  -- cutscene 0 : introduction
   {
      strblock = cuts_strbase + 0,
      timetable =
      {
         { time = 0.0, action = cuts_text_set_color,     value = 11 },
         { time = 1.0, action = cuts_text_fadein,        value = 3 },
         { time = 0.0, action = cuts_anim_fadein,        value = "cs000.n02" },
         { time = 0.0, action = cuts_sound_play,         value = "29" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 10 },
         { time = 3.0, action = cuts_anim_fadeout,       value = 0 },
         { time = 0.0, action = cuts_text_fadeout,       value = 0 },
         { time = 1.0, action = cuts_finished,           value = 0 }
      }
   },

   [1] =  -- cutscene 1: endgame
   {
      strblock = cuts_strbase + 1,
      timetable =
      {
         { time=8.0, func=cuts_finished, value=0 }
      }
   },

   [2] =  -- cutscene 2: tyball action
   {
      strblock = cuts_strbase + 2,
      timetable =
      {
         { time=8.0, func=cuts_finished, value=0 }
      }
   }
}


-- functions

-- initializes cutscene sequence
function cuts_init(this,cutscene)
   self = this       -- sets "self" as userdata for all C function calls
   cuts = cutscene   -- number of cutscene to use
   curtime = 0.0     -- current time frame
   curstep = 1       -- current timetable step
end

-- processes cutscene timetable for every game tick
function cuts_tick(time)
   -- at start, set string block to use
   if time == 0.0
   then
      cuts_do_action(self,cuts_set_string_block,cutscenes[cuts].strblock)
   end

   -- check if new timetable entry can be done
   while time >= curtime + cutscenes[cuts].timetable[curstep].time
   do
      -- perform action
      cuts_do_action(self,
         cutscenes[cuts].timetable[curstep].action,
         cutscenes[cuts].timetable[curstep].value)

      -- update curtime and curstep
      curtime = curtime + cutscenes[cuts].timetable[curstep].time;
      curstep = curstep + 1
   end
end
