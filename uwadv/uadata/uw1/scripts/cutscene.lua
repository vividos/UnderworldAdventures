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
cuts_anim_continue = 14      -- continues animation after stop

-- string block base for cutscenes
cuts_strbase = 12*256


-- tables

-- cutscene timetables
cutscenes = {
   [0] =  -- cutscene 0 : introduction (by Dirk Manders)
   { 
      strblock = cuts_strbase + 0,
      timetable =
      {
      -- Part 1: Prelude
         -- 1.1: Dark background, narrator speaks
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 1.0, action = cuts_sound_play,         value = "26" },
         { time = 0.0, action = cuts_text_fadein,        value = 0 },
         { time = 2.8, action = cuts_text_fadeout,       value = 0 },

         { time = 0.8, action = cuts_sound_play,         value = "27" },
         { time = 0.0, action = cuts_text_fadein,        value = 1 },
         { time = 7.5, action = cuts_text_fadeout,       value = 0 },

         -- 1.2: Garamon appears, narrator keeps speaking
         { time = 1.0, action = cuts_sound_play,         value = "28" },
         { time = 0.0, action = cuts_text_fadein,        value = 2 },
         { time = 0.5, action = cuts_anim_fadein,        value = "cs000.n02" },

         { time = 4.0, action = cuts_sound_play,         value = "29" },
         { time = 0.0, action = cuts_text_show,          value = 3 },

         -- 1.3: Garamon speaks
         { time = 4.2, action = cuts_sound_play,         value = "23" },
         { time = 0.0, action = cuts_text_set_color,     value = 196 },
         { time = 0.0, action = cuts_text_show,          value = 4 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n03" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 13 },

         { time = 3.8, action = cuts_sound_play,         value = "24" },
         { time = 0.0, action = cuts_text_show,          value = 5 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n04" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },

         { time = 4.8, action = cuts_sound_play,         value = "25" },
         { time = 0.0, action = cuts_text_show,          value = 6 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n05" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 15 },
         
         -- 1.4: Garamon vanishes, narrator speaks again
         { time = 4.0, action = cuts_sound_play,         value = "30" },
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 0.0, action = cuts_text_fadein,        value = 7 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n07" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 22 },
         { time = 4.5, action = cuts_text_fadeout,       value = 0 },

      -- Part 2: Room kidnapping scene
         -- 2.1: Woman screams
         { time = 1.0, action = cuts_anim_fadein,        value = "cs000.n10" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 1 },
         { time = 0.5, action = cuts_sound_play,         value = "31" },
         { time = 0.0, action = cuts_text_set_color,     value = 163 },
         { time = 0.5, action = cuts_text_show,          value = 8 },
         { time = 1.0, action = cuts_anim_set_stopframe, value = 12 },
         { time = 0.0, action = cuts_anim_continue,      value = 0 },

         -- 2.2: Tyball speaks and vanishes
         { time = 1.0, action = cuts_sound_play,         value = "37" },
         { time = 0.0, action = cuts_text_set_color,     value = 179 },
         { time = 0.0, action = cuts_text_show,          value = 9 },

         { time = 3.5, action = cuts_sound_play,         value = "38" },
         { time = 0.0, action = cuts_text_show,          value = 10 },

         { time = 4.0, action = cuts_sound_play,         value = "39" },
         { time = 0.0, action = cuts_text_show,          value = 11 },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 133 },
         { time = 0.7, action = cuts_anim_continue,      value = 0 },
         { time = 5.0, action = cuts_text_hide,          value = 0 },

         -- 2.3: Narrator speaks
         { time = 5.7, action = cuts_sound_play,         value = "32" },
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 0.0, action = cuts_text_fadein,        value = 12 },
         { time = 5.5, action = cuts_text_fadeout,       value = 0 },

         -- 2.4: Guards come in and speak
         { time = 6.0, action = cuts_sound_play,         value = "12" },
         { time = 0.0, action = cuts_text_set_color,     value = 163 },
         { time = 0.0, action = cuts_text_show,          value = 13 },

         -- 2.5: Single guards speaks (close-up)
         { time = 3.5, action = cuts_sound_play,         value = "13" },
         { time = 0.0, action = cuts_text_show,          value = 14 },
         { time = 0.5, action = cuts_anim_show,          value = "cs000.n15" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 13 },

         { time = 3.5, action = cuts_anim_show,          value = "cs000.n16" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_sound_play,         value = "14" },
         { time = 0.0, action = cuts_text_show,          value = 15 },
         { time = 4.0, action = cuts_text_fadeout,       value = 0 },
         { time = 1.0, action = cuts_anim_fadeout,       value = 0 },

      -- Part 3: Falsely accused
         -- 3.1: Narrator speaks, baron Almric wide view
         { time = 1.0, action = cuts_anim_fadein,        value = "cs000.n11" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 1 },
         { time = 0.5, action = cuts_sound_play,         value = "33" },
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 0.5, action = cuts_text_fadein,        value = 16 },

         { time = 3.5, action = cuts_sound_play,         value = "34" },
         { time = 0.2, action = cuts_text_show,          value = 17 },
         { time = 3.0, action = cuts_text_fadeout,       value = 0 },

         -- 3.2: Almric speaks
         { time = 0.2, action = cuts_anim_show,          value = "cs000.n11" },
         { time = 0.5, action = cuts_sound_play,         value = "00" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 13 },
         { time = 0.0, action = cuts_text_set_color,     value = 251 },
         { time = 0.0, action = cuts_text_show,          value = 18 },

         -- 3.3: Guard speaks, close-up (a bit tricky, there is no exact matching animation)
         { time = 3.0, action = cuts_sound_play,         value = "15" },
         { time = 0.1, action = cuts_anim_show,          value = "cs000.n17" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 5 },
         { time = 0.0, action = cuts_text_set_color,     value = 163 },
         { time = 0.0, action = cuts_text_show,          value = 19 },

         { time = 2.0, action = cuts_sound_play,         value = "65" },
         { time = 0.1, action = cuts_anim_set_stopframe, value = 15 },
         { time = 0.0, action = cuts_anim_continue,      value = 0 },
         { time = 0.0, action = cuts_text_show,          value = 20 },

         { time = 3.0, action = cuts_sound_play,         value = "16" },
         { time = 0.2, action = cuts_text_show,          value = 21 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n17" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 19 },

         { time = 5.0, action = cuts_sound_play,         value = "17" },
         { time = 0.0, action = cuts_text_show,          value = 22 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n16" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 11 },

         { time = 3.0, action = cuts_sound_play,         value = "18" },
         { time = 0.0, action = cuts_text_show,          value = 23 },
         { time = 0.0, action = cuts_anim_show,          value = "cs000.n17" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 10 },
         { time = 2.0, action = cuts_text_hide,          value = 0 },

         -- 3.4: Almric speaks again
         { time = 0.4, action = cuts_anim_show,          value = "cs000.n11" },
         { time = 0.5, action = cuts_sound_play,         value = "50" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 7 },
         { time = 0.0, action = cuts_text_set_color,     value = 251 },
         { time = 0.0, action = cuts_text_show,          value = 24 },
         { time = 1.5, action = cuts_text_hide,          value = 0 },

         -- 3.5: Narrator speaks, baron Almric wide view
         { time = 0.5, action = cuts_sound_play,         value = "35" },
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 0.0, action = cuts_text_fadein,        value = 25 },
         { time = 2.5, action = cuts_text_fadeout,       value = 0 },

         -- 3.6: Almric speaks, close-up
         { time = 1.0, action = cuts_anim_show,          value = "cs000.n12" },
         { time = 0.0, action = cuts_sound_play,         value = "01" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 13 },
         { time = 0.0, action = cuts_text_set_color,     value = 251 },
         { time = 0.0, action = cuts_text_show,          value = 26 },

         { time = 3.0, action = cuts_sound_play,         value = "02" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 29 },
         { time = 0.0, action = cuts_text_show,          value = 27 },

         { time = 5.5, action = cuts_sound_play,         value = "03" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 22 },
         { time = 0.0, action = cuts_text_show,          value = 28 },
         { time = 4.5, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 5 },

         { time = 2.0, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_sound_play,         value = "04" },
         { time = 0.0, action = cuts_text_show,          value = 29 },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 22 },

         { time = 5.0, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_sound_play,         value = "05" },
         { time = 0.0, action = cuts_text_show,          value = 30 },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 4.5, action = cuts_text_hide,          value = 0 },

         -- 3.7: Narrator speaks, baron Almric wide view
         { time = 0.5, action = cuts_anim_show,          value = "cs000.n11" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 1 },
         { time = 0.5, action = cuts_sound_play,         value = "36" },
         { time = 0.0, action = cuts_text_set_color,     value = 147 },
         { time = 0.0, action = cuts_text_fadein,        value = 31 },
         { time = 3.5, action = cuts_text_fadeout,       value = 0 },

         -- 3.8: Almric speaks, close-up
         { time = 0.5, action = cuts_sound_play,         value = "06" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_text_set_color,     value = 251 },
         { time = 0.0, action = cuts_text_show,          value = 32 },

         { time = 5.0, action = cuts_sound_play,         value = "07" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n14" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 29 },
         { time = 0.0, action = cuts_text_show,          value = 33 },

         { time = 6.0, action = cuts_sound_play,         value = "08" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n12" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 13 },
         { time = 0.0, action = cuts_text_show,          value = 34 },
         { time = 2.0, action = cuts_anim_show,          value = "cs000.n12" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 10 },

         { time = 3.0, action = cuts_sound_play,         value = "58" },
         { time = 0.3, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_text_show,          value = 35 },

         { time = 4.5, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_sound_play,         value = "09" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_text_show,          value = 36 },

         { time = 4.5, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_sound_play,         value = "10" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_text_show,          value = 37 },

         { time = 4.5, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_sound_play,         value = "11" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 10 },
         { time = 0.0, action = cuts_text_show,          value = 38 },
         { time = 2.0, action = cuts_anim_show,          value = "cs000.n13" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 5.0, action = cuts_text_hide,          value = 0 },
         { time = 1.0, action = cuts_anim_fadeout,       value = 0 },

      -- Part 4: Into the abyss
         -- 4.1: Walking through the fields
         { time = 1.0, action = cuts_anim_fadein,        value = "cs000.n20" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 41 },
         { time = 8.6, action = cuts_anim_fadeout,       value = 0 },

         -- 4.2: Opening the abyss
         { time = 0.5, action = cuts_anim_fadein,        value = "cs000.n22" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 8 },
         { time = 3.5, action = cuts_anim_fadeout,       value = 0 },

         -- 4.3: Guard says some final words
         { time = 0.5, action = cuts_sound_play,         value = "19" },
         { time = 0.0, action = cuts_anim_fadein,        value = "cs000.n24" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 21 },
         { time = 0.0, action = cuts_text_set_color,     value = 163 },
         { time = 0.5, action = cuts_text_show,          value = 39 },
         { time = 5.0, action = cuts_text_hide,          value = 0 },

         { time = 0.5, action = cuts_anim_show,          value = "cs000.n25" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 23 },
         { time = 0.0, action = cuts_sound_play,         value = "20" },
         { time = 0.0, action = cuts_text_show,          value = 40 },
         { time = 5.0, action = cuts_text_hide,          value = 0 },

         { time = 0.5, action = cuts_anim_show,          value = "cs000.n24" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 15 },
         { time = 0.0, action = cuts_sound_play,         value = "22" },
         { time = 0.0, action = cuts_text_show,          value = 41 },
         { time = 5.0, action = cuts_text_hide,          value = 0 },
         { time = 0.0, action = cuts_anim_fadeout,       value = 0 },

         -- 4.4: The doors close... the adventure begins!
         { time = 0.5, action = cuts_anim_fadein,        value = "cs000.n23" },
         { time = 0.0, action = cuts_anim_set_stopframe, value = 9 },

      -- The End
         { time = 2.5, action = cuts_finished,           value = 0 }
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

      if cuts_finished ~= cutscenes[cuts].timetable[curstep].action
      then
         curstep = curstep + 1
      end
   end
end
