--
-- lua cutscene script
--

--
-- available C functions:
-- cuts_play_sound(self,"01")   plays "sound/01.voc"
-- cuts_finished(self)          indicates that cutscene has finished
--

function cuts_init(this)
   self = this               -- sets "self" as userdata for all C functions to call
   done = 0
end

function cuts_tick(time)
   if time > 3.0 and done ~= 1
   then
      cuts_play_sound(self,"00")
      done = 1
   end

   if time > 8.0
   then
      cuts_finished(self)
   end
end
