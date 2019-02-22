//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file ingame_orig.cpp
/// \brief ingame user interface, from the original uw1
//
#include "common.hpp"
#include "ingame_orig.hpp"
#include "resourcemanager.hpp"
#include "renderer.hpp"
#include "underworld.hpp"
#include "audio.hpp"
#include "save_game.hpp"
#include "cutscene_view.hpp"
#include "uwadv/debug.hpp"
#include "conversation.hpp"
#include "models.hpp"

/// time to fade in/out
const double ua_ingame_orig_screen::fade_time = 0.5;

/// x resolution of savegame screenshot
const unsigned int ua_ingame_screenshot_xres = 160;
/// y resolution of savegame screenshot
const unsigned int ua_ingame_screenshot_yres = 100;

/// ctor; sets parent pointers for ingame controls
ua_ingame_orig_screen::ua_ingame_orig_screen(IGame& game)
   :ua_screen(game), vitality_flask(true), mana_flask(false),
   dragon_left(true), dragon_right(false),
   m_playerPhysics(game.GetUnderworld().GetPlayer(), game.get_settings().GetBool(Base::settingUwadvFeatures))
{
   compass.set_parent(this);
   runeshelf.set_parent(this);
   spellarea.set_parent(this);
   vitality_flask.set_parent(this);
   mana_flask.set_parent(this);
   gargoyle_eyes.set_parent(this);
   dragon_left.set_parent(this);
   dragon_right.set_parent(this);
   command_buttons.set_parent(this);
   view3d.set_parent(this);
   powergem.set_parent(this);
   move_arrows.set_parent(this);
}

void ua_ingame_orig_screen::init()
{
   ua_screen::init();

   UaTrace("orig. ingame user interface started\n");

   // init all variables
   move_turn_left = move_turn_right = move_walk_forward =
      move_walk_backwards = move_run_forward = false;

   priority_cursor = false;

   gamemode = ua_mode_default;
   tickcount = 0;

   fade_state = 0;

   fadeout_action = ua_action_none;
   fadeout_param = 0;

   game.get_image_manager().load_list(img_objects, "objects");

   // set OpenGL flags
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   uw_notify(ua_notify_level_change);

   Base::Settings& settings = game.get_settings();

   // load keymap
   keymap.Init(settings);
   register_keymap(&keymap);

   // init images/subwindows

   // load background
   {
      if (settings.GetGametype() == Base::gameUw2)
      {
         // uw2: no background for now
         img_background.get_image().create(320, 200);
      }
      else
      {
         // uw1/uw_demo
         const char* mainscreenname = "data/main.byt";

         // replace name when using uw_demo
         if (settings.GetBool(Base::settingUw1IsUwdemo))
            mainscreenname = "data/dmain.byt";

         game.get_image_manager().ImageManager::load(img_background.get_image(),
            mainscreenname, 0, 0, ua_img_byt);
      }

      img_background.init(game, 0, 0);

      img_background.get_image().fill_rect(236, 7, 83, 114, 0); // panel area
      img_background.get_image().fill_rect(272, 3, 10, 4, 0);
      img_background.get_image().fill_rect(272, 121, 10, 18, 0);

      img_background.update();

      register_window(&img_background);
   }

   // init 3d view window
   view3d.init(game, 54, 20);
   register_window(&view3d);

   game.get_renderer().set_viewport3d(52, 19, 172, 112);

   // init compass
   compass.init(game, 112, 131);
   compass.add_border(img_background.get_image());
   compass.update();
   register_window(&compass);

   // init textscroll
   {
      unsigned int scrollwidth = 290;

      // adjust scroll width for uw_demo
      if (game.get_settings().GetBool(Base::settingUw1IsUwdemo))
         scrollwidth = 218;

      textscroll.init(game, 15, 169, scrollwidth, 30, 42);
      //textscroll.init(game, 11,169, 299,29, 42);
      textscroll.set_color_code(ua_cc_black);

      textscroll.print("Welcome to the Underworld Adventures!\n"
         " \\3http://uwadv.sourceforge.net/\\0");

      register_window(&textscroll);

      // fill message scroll background area
      img_background.get_image().fill_rect(16, 169, scrollwidth, 30, 42);
   }

   // runeshelf
   runeshelf.init(game, 176, 138);
   runeshelf.update_runeshelf();
   register_window(&runeshelf);

   // active spells
   spellarea.init(game, 52, 136);
   spellarea.update_spell_area();
   register_window(&spellarea);

   // vitality/mana flasks
   vitality_flask.init(game, 248, 125);
   vitality_flask.add_border(img_background.get_image());
   register_window(&vitality_flask);

   mana_flask.init(game, 284, 125);
   mana_flask.add_border(img_background.get_image());
   register_window(&mana_flask);

   // gargoyle eyes
   gargoyle_eyes.init(game, 128, 4);
   //gargoyle_eyes.add_border(img_background.get_image());
   gargoyle_eyes.update_eyes();
   register_window(&gargoyle_eyes);

   // left and right side dragons
   dragon_left.init(game, 36, 65);
   register_window(&dragon_left);

   dragon_right.init(game, 204, 65);
   register_window(&dragon_right);

   // init command buttons
   command_buttons.init(game, 0, 0); // buttons are self-repositioning
   register_window(&command_buttons);

   // init panel
   panel.init(this, 235, 6);
   register_window(&panel);

   // init powergem
   powergem.init(game, 4, 139);
   powergem.add_border(img_background.get_image());
   powergem.update_gem();
   register_window(&powergem);

   // init move arrows
   move_arrows.init(game, 107, 154);
   register_window(&move_arrows);

   // init mouse cursor
   mousecursor.init(game, 0);
   mousecursor.show(true);

   register_window(&mousecursor);

   resume();
}

void ua_ingame_orig_screen::suspend()
{
   UaTrace("suspending orig. ingame user interface\n\n");

   game.get_renderer().clear();


   // unregister ourselves
   // TODO what to do?
   //game.GetUserInterface().GetGameLogic().register_callback();
}

void ua_ingame_orig_screen::resume()
{
   UaTrace("resuming orig. ingame user interface\n");


   // register us as callback
   // TODO what to do?
   //game.GetGameLogic().register_callback(this);


   // setup fade-in
   fade_state = 0;
   fading.init(true, game.get_tickrate(), fade_time);

   if (fadeout_action == ua_action_conversation)
   {
      // after conversations, play "Dark Abyss"
      game.get_audio_manager().StartMusicTrack(Audio::musicUw1_DarkAbyss, true);
   }
   else
   {
      // normal start, play "Descent"
      game.get_audio_manager().StartMusicTrack(Audio::musicUw1_Descent, true);
   }
}

void ua_ingame_orig_screen::destroy()
{
   suspend();

   // only windows and other stuff has to be destroyed that wasn't registered
   // with ua_screen::register_window()



   UaTrace("orig. ingame user interface finished\n\n");
}

void ua_ingame_orig_screen::draw()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // 3d world
   {
      // set up 3d world
      ua_vector3d view_offset(0.0, 0.0, 20.0);
      game.get_renderer().setup_camera3d(view_offset);

      // render a const world
      game.get_renderer().render_underworld(game.GetUnderworld());
   }

   // render 2d user interface
   {
      game.get_renderer().setup_camera2d();

      glEnable(GL_BLEND);

      if (fade_state == 0 || fade_state == 2)
      {
         // when fading in/out, lay a quad over the already rendered 3d view
         Uint8 alpha = fading.get_fade_value();
         if (alpha < 255)
         {
            glColor4ub(0, 0, 0, 255 - alpha);
            glBindTexture(GL_TEXTURE_2D, 0);

            // draw quad
            glBegin(GL_QUADS);
            glVertex2i(52, 68);
            glVertex2i(226, 68);
            glVertex2i(226, 182);
            glVertex2i(52, 182);
            glEnd();
         }

         glColor3ub(alpha, alpha, alpha);
      }
      else
         glColor3ub(255, 255, 255);

      // draw all registered windows
      ua_screen::draw();

      glDisable(GL_BLEND);
   }
}

bool ua_ingame_orig_screen::process_event(SDL_Event& event)
{
   return ua_screen::process_event(event);
}

void ua_ingame_orig_screen::key_event(bool key_down, Base::EKeyType key)
{
   Underworld::Player& pl = game.GetUnderworld().GetPlayer();
   ua_ingame_game_mode last_gamemode = gamemode;

   switch (key)
   {
      // run forward keys
   case Base::keyRunForward:
   case Base::keyRunForwardEasymove:
      move_run_forward = key_down;
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_walk, 1.0);
         m_playerPhysics.set_movement_mode(ua_move_walk);
      }
      else
      {
         if (move_walk_backwards)
            m_playerPhysics.set_movement_factor(ua_move_walk, -0.4);
         else
            if (move_walk_forward)
               m_playerPhysics.set_movement_factor(ua_move_walk, 0.6);
            else
               if (!view3d.get_mouse_move())
                  m_playerPhysics.set_movement_mode(0, ua_move_walk);
      }

      break;

      // walk forward keys
   case Base::keyWalkForward:
   case Base::keyWalkForwardEasymove:
      move_walk_forward = key_down;
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_walk, 0.6);
         m_playerPhysics.set_movement_mode(ua_move_walk);
      }
      else
      {
         if (move_walk_backwards)
            m_playerPhysics.set_movement_factor(ua_move_walk, -0.4);
         else
            if (move_run_forward)
               m_playerPhysics.set_movement_factor(ua_move_walk, 1.0);
            else
               if (!view3d.get_mouse_move())
                  m_playerPhysics.set_movement_mode(0, ua_move_walk);
      }
      break;

      // turn left keys
   case Base::keyTurnLeft:
   case Base::keyTurnLeftEasymove:
      move_turn_left = key_down;
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_rotate, 1.0);
         m_playerPhysics.set_movement_mode(ua_move_rotate);
      }
      else
      {
         if (move_turn_right)
            m_playerPhysics.set_movement_factor(ua_move_rotate, -1.0);
         else
            if (!view3d.get_mouse_move())
               m_playerPhysics.set_movement_mode(0, ua_move_rotate);
      }
      break;

      // turn right keys
   case Base::keyTurnRight:
   case Base::keyTurnRightEasymove:
      move_turn_right = key_down;
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_rotate, -1.0);
         m_playerPhysics.set_movement_mode(ua_move_rotate);
      }
      else
      {
         if (move_turn_left)
            m_playerPhysics.set_movement_factor(ua_move_rotate, 1.0);
         else
            if (!view3d.get_mouse_move())
               m_playerPhysics.set_movement_mode(0, ua_move_rotate);
      }
      break;

      // slide left key
   case Base::keySlideLeft:
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_slide, -1.0);
         m_playerPhysics.set_movement_mode(ua_move_slide);
      }
      else
         if (!view3d.get_mouse_move())
            m_playerPhysics.set_movement_mode(0, ua_move_slide);
      break;

      // slide right key
   case Base::keySlideRight:
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_slide, 1.0);
         m_playerPhysics.set_movement_mode(ua_move_slide);
      }
      else
         if (!view3d.get_mouse_move())
            m_playerPhysics.set_movement_mode(0, ua_move_slide);
      break;

      // walk backwards keys
   case Base::keyWalkBackwards:
   case Base::keyWalkBackwardsEasymove:
      move_walk_backwards = key_down;

      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_walk, -0.4);
         m_playerPhysics.set_movement_mode(ua_move_walk);
      }
      else
      {
         if (move_walk_forward || move_run_forward)
            m_playerPhysics.set_movement_factor(ua_move_walk, move_run_forward ? 1.0 : 0.6);
         else
            if (!view3d.get_mouse_move())
               m_playerPhysics.set_movement_mode(0, ua_move_walk);
      }
      break;

      // look up key
   case Base::keyLookUp:
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_lookup, 1.0);
         m_playerPhysics.set_movement_mode(ua_move_lookup);
      }
      else
         m_playerPhysics.set_movement_mode(0, ua_move_lookup);
      break;

      // look down key
   case Base::keyLookDown:
      if (key_down)
      {
         m_playerPhysics.set_movement_factor(ua_move_lookup, -1.0);
         m_playerPhysics.set_movement_mode(ua_move_lookup);
      }
      else
         m_playerPhysics.set_movement_mode(0, ua_move_lookup);
      break;

      // center look key
   case Base::keyCenterView:
      if (key_down)
      {
         m_playerPhysics.set_movement_mode(0, ua_move_lookup);
         m_playerPhysics.set_movement_factor(ua_move_lookup, 0.0);
         pl.SetPanAngle(0.0);
      }
      break;

   case Base::keyMenuUp:
      if (key_down && gamemode == ua_mode_options)
         command_buttons.select_previous_button();
      break;

   case Base::keyMenuDown:
      if (key_down && gamemode == ua_mode_options)
         command_buttons.select_next_button();
      break;

   case Base::keyMenuLeft:
   case Base::keyMenuRight:
      // note: as we don't have menus with 2 columns, these are unused here
      break;

   case Base::keyMenuTopOfList:
   case Base::keyMenuTopOfList2:
      if (key_down && gamemode == ua_mode_options)
         command_buttons.select_previous_button(true);
      break;

   case Base::keyMenuBottomOfList:
   case Base::keyMenuBottomOfList2:
      if (key_down && gamemode == ua_mode_options)
         command_buttons.select_next_button(true);
      break;

   case Base::keyMenuPressButton:
      if (key_down && gamemode == ua_mode_options)
         command_buttons.do_button_action();
      break;

      // combat keys
   case Base::keyCombatBash:
   case Base::keyCombatSlash:
   case Base::keyCombatThrust:
      if (gamemode == ua_mode_fight)
      {
         if (key_down)
         {
            // start combat weapon drawback
            game.GetGameLogic().UserAction(ua_action_combat_draw,
               key == Base::keyCombatBash ? 0 :
               key == Base::keyCombatSlash ? 1 : 2);
         }
         else
         {
            // end combat weapon drawback
            game.GetGameLogic().UserAction(ua_action_combat_release, 0);
         }
      }
      break;

      // options menu
   case Base::keySpecialOptions:
      if (key_down && gamemode != ua_mode_options)
      {
         command_buttons.select_button(0);
         command_buttons.do_button_action();
      }
      break;

      // talk mode
   case Base::keySpecialTalkMode:
      if (key_down || gamemode == ua_mode_options)
         break;

      if (gamemode != ua_mode_talk)
      {
         command_buttons.select_button(1);
         gamemode = ua_mode_talk;
      }
      else
      {
         command_buttons.select_button(-1);
         gamemode = ua_mode_default;
      }
      break;

      // get mode
   case Base::keySpecialGetMode:
      if (key_down || gamemode == ua_mode_options)
         break;

      if (gamemode != ua_mode_get)
      {
         command_buttons.select_button(2);
         gamemode = ua_mode_get;
      }
      else
      {
         command_buttons.select_button(-1);
         gamemode = ua_mode_default;
      }
      break;

      // look mode
   case Base::keySpecialLookMode:
      if (key_down || gamemode == ua_mode_options)
         break;

      if (gamemode != ua_mode_look)
      {
         command_buttons.select_button(3);
         gamemode = ua_mode_look;
      }
      else
      {
         command_buttons.select_button(-1);
         gamemode = ua_mode_default;
      }
      break;

      // fight mode
   case Base::keySpecialFightMode:
      if (key_down || gamemode == ua_mode_options)
         break;

      if (gamemode != ua_mode_fight)
      {
         /*bool ret = */game.GetGameLogic().UserAction(ua_action_combat_enter);
         //if (ret)
         {
            command_buttons.select_button(4);
            gamemode = ua_mode_fight;
         }
      }
      else
      {
         command_buttons.select_button(-1);
         gamemode = ua_mode_default;
      }

      break;

      // use mode
   case Base::keySpecialUseMode:
      if (key_down || gamemode == ua_mode_options)
         break;

      if (gamemode != ua_mode_use)
      {
         command_buttons.select_button(5);
         gamemode = ua_mode_use;
      }
      else
      {
         command_buttons.select_button(-1);
         gamemode = ua_mode_default;
      }
      break;

   case Base::keySpecialFlipPanel:
      break;

   case Base::keySpecialCastSpell:
      if (!key_down)
         game.GetGameLogic().UserAction(ua_action_clicked_spells);
      break;

   case Base::keySpecialUseTrack:
      if (!key_down)
         game.GetGameLogic().UserAction(ua_action_track_creatures);
      break;

   case Base::keySpecialSleep:
      if (!key_down)
         game.GetGameLogic().UserAction(ua_action_sleep);
      break;

      // quicksave key
   case Base::keySpecialQuicksave:
      if (key_down)
      {
         uw_print("\\0quicksaving...");
         schedule_action(ua_action_quicksave, false);
      }
      break;

      // quickload key
   case Base::keySpecialQuickload:
      if (key_down && game.get_savegames_manager().IsQuicksaveAvail())
      {
         uw_print("\\0quickloading...");
         schedule_action(ua_action_quickload, false);
      }
      break;

      // "load game" key
   case Base::keyGameRestoreGame:
      if (key_down)
         schedule_action(ua_action_load_game, true);
      break;

      // "save game" key
   case Base::keyGameSaveGame:
      if (key_down)
         schedule_action(ua_action_save_game, true);
      break;

      // returns from menu to game
   case Base::keyGameReturnToGame:
      break;

      // "debugger" key
   case Base::keyUaDebug:
      if (key_down)
         game.get_debugger().start_debugger(&game);
      break;

      // exit screen key
   case Base::keyUaReturnMenu:
      if (key_down)
         schedule_action(ua_action_exit, true);
      break;

      // "screenshot" key
   case Base::keyUaScreenshot:
      if (key_down)
      {
         // TODO take a screenshot
//         do_screenshot(false);
//         save_screenshot();
      }
      break;

   default:
      break;
   }

   // leaving fight mode?
   if (last_gamemode != gamemode && last_gamemode == ua_mode_fight)
   {
      game.GetGameLogic().UserAction(ua_action_combat_leave);
   }
}

void ua_ingame_orig_screen::tick()
{
   ua_screen::tick();

   // evaluate underworld;
   // only evaluate when the user is not in the options menu
   if (fadeout_action == ua_action_none && gamemode != ua_mode_options)
   {
      game.GetGameLogic().EvaluateUnderworld(double(tickcount) / game.get_tickrate());

      tickcount++;

      // do renderer-specific tick processing
      game.get_renderer().tick(game.get_tickrate());
   }

   // action to perform?
   if (((fade_state == 0 || fade_state == 2) && fading.tick()) || fade_state == 5)
   {
      if (fade_state == 5)
         fade_state = 1;
      else
         fade_state++;

      // now it's time
      do_action(fadeout_action);

      fadeout_action = ua_action_none;
   }

   // resume when reentering the screen
   if (fade_state == 3)
      resume();
}

void ua_ingame_orig_screen::schedule_action(ua_ingame_orig_action action, bool fadeout_before)
{
   fadeout_action = action;

   if (fadeout_before)
   {
      fade_state = 2;
      fading.init(false, game.get_tickrate(), fade_time);
   }
   else
      fade_state = 5; // special schedule_action fade state

   // check which action we scheduled
   switch (action)
   {
   case ua_action_save_game:
   case ua_action_quicksave:
      // render savegame preview image
      do_savegame_screenshot(ua_ingame_screenshot_xres,
         ua_ingame_screenshot_yres);
      break;

   default:
      break;
   }
}

/// \todo implement all actions
void ua_ingame_orig_screen::do_action(ua_ingame_orig_action action)
{
   switch (action)
   {
   case ua_action_none:
      break;

      // leave the ingame_orig screen
   case ua_action_exit:
      game.remove_screen();
      break;

      // start "load game" screen
   case ua_action_load_game:
      game.replace_screen(new ua_save_game_screen(game, false, true), true);
      break;

      // start "save game" screen
   case ua_action_save_game:
      game.replace_screen(new ua_save_game_screen(game, false, false), true);
      break;

      // quickloading
   case ua_action_quickload:
      if (game.get_savegames_manager().IsQuicksaveAvail())
      {
         Base::Savegame sg = game.get_savegames_manager().LoadQuicksaveSavegame();
         game.GetUnderworld().Load(sg);
         uw_print("quickloading done.");
      }
      break;

      // quicksaving
   case ua_action_quicksave:
   {
      // set player infos
      Base::SavegameInfo info;
      Underworld::Player& pl = game.GetUnderworld().GetPlayer();
      pl.FillSavegamePlayerInfos(info);
      info.gamePrefix = game.get_settings().GetString(Base::settingGamePrefix);

      Base::Savegame sg = game.get_savegames_manager().SaveQuicksaveSavegame(info);

      game.GetUnderworld().Save(sg);
      uw_print("quicksaving done.");
   }
   break;

   // starts conversation
   case ua_action_conversation:
      game.replace_screen(new ua_conversation_screen(game, fadeout_param), true);
      break;

      // shows cutscene
   case ua_action_cutscene:
      game.replace_screen(new ua_cutscene_view_screen(game, fadeout_param), true);
      break;

   default:
      break;
   }
}

bool ua_ingame_orig_screen::get_move_state(ua_ingame_move_state movestate)
{
   bool state = false;
   switch (movestate)
   {
   case ua_move_turn_left: state = move_turn_left; break;
   case ua_move_turn_right: state = move_turn_right; break;
   case ua_move_walk_forward: state = move_walk_forward; break;
   case ua_move_run_forward: state = move_run_forward; break;
   case ua_move_walk_backwards: state = move_walk_backwards; break;
   default: break;
   }
   return state;
}

/// Sets a new cursor image for the mouse cursor. There can be normal cursor
/// images and prioritized cursor images. The last ones cannot be changed or
/// resetted by unprioritized ones. Passing a negative index resets priority
/// mode and sets the negated index as normal cursor index. If the cursor
/// index is > 0x100, an object image is used for the cursor image.
/// \param index cursor image index to set
/// \param priority indicates if priority cursor image should be set
void ua_ingame_orig_screen::set_cursor(int index, bool priority)
{
   if (!priority && priority_cursor)
      return; // ignore new non-priority cursor

   priority_cursor = priority;

   // reset priority cursor
   if (index < 0 && priority)
   {
      priority_cursor = false;
      index = -index;
   }

   if (index > 0x100)
   {
      // change mouse cursor type to object image
      mousecursor.set_custom(img_objects[unsigned(index) - 0x100]);
   }
   else
   {
      // set image from cursors
      mousecursor.set_type(unsigned(index));
   }
}

void ua_ingame_orig_screen::uw_notify(EUserInterfaceNotification notify,
   unsigned int param)
{
   switch (notify)
   {
   case ua_notify_update_powergem:
      powergem.update_gem();
      break;

   case ua_notify_update_gargoyle_eyes:
      gargoyle_eyes.update_eyes();
      break;

   case ua_notify_update_flasks:
      vitality_flask.update_flask();
      mana_flask.update_flask();
      break;

   case ua_notify_update_show_ingame_animation:
      break;

   case ua_notify_update_spell_area:
      spellarea.update_spell_area();
      break;

   case ua_notify_animate_dragons:
      //dragon_left.animate();
      //dragon_right.animate();
      break;

   case ua_notify_level_change:
      game.get_renderer().prepare_level(game.GetUnderworld().GetCurrentLevel());
      break;

   case ua_notify_select_target:
      //target_select_mode = true
      set_cursor(param + 0x100, true);
      break;

   default:
      break;
   }
}

void ua_ingame_orig_screen::uw_print(const char* text)
{
   textscroll.print(text);
}

void ua_ingame_orig_screen::uw_start_conversation(Uint16 list_pos)
{
   fadeout_param = list_pos;
   schedule_action(ua_action_conversation, true);
}

// TODO move? delete?
//void ua_ingame_orig_screen::uw_get_object_triangles(unsigned int x,
//   unsigned int y, const Underworld::Object& obj,
//   std::vector<ua_triangle3d_textured>& alltriangles)
//{
//   game.get_renderer().get_model3d_bounding_triangles(x,y,obj,alltriangles);
//}

void ua_ingame_orig_screen::do_savegame_screenshot(
   unsigned int xres, unsigned int yres)
{
   std::vector<Uint32> screenshot_rgba;
   unsigned int screenshot_xres, screenshot_yres;

   // set up viewport and camera
   // note: viewport is set only for having a proper aspect ratio; the real
   // viewport is set some more lines below
   game.get_renderer().set_viewport3d(0, 0, xres, yres);

   ua_vector3d view_offset(0, 0, 20.0);
   game.get_renderer().setup_camera3d(view_offset);

   glViewport(0, 0, xres, yres);

   glClear(GL_COLOR_BUFFER_BIT);

   // render a const world
   game.get_renderer().render_underworld(game.GetUnderworld());

   // prepare screenshot
   screenshot_xres = xres;
   screenshot_yres = yres;

   screenshot_rgba.clear();
   screenshot_rgba.resize(xres*yres, 0);

   // read in scanlines
   glReadBuffer(GL_BACK);

   for (unsigned int i = 0; i < yres; i++)
   {
      glReadPixels(0, yres - i - 1, xres, 1, GL_RGBA, GL_UNSIGNED_BYTE,
         &screenshot_rgba[i*xres]);
   }

   // reset viewport to original
   game.get_renderer().set_viewport3d(52, 19, 172, 112);

   // set in savegames manager
   game.get_savegames_manager().SetSaveScreenshot(
      screenshot_xres, screenshot_yres, screenshot_rgba);
}
