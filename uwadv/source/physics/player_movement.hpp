
//! player movement enum
enum ua_player_movement_mode
{
   ua_move_walk = 1,    //!< walks forward (or backwards, when factor is negative)
   ua_move_rotate  = 2, //!< rotates player left (or right)
   ua_move_lookup = 4,  //!< moves player look angle up (or down)
   ua_move_jump = 8,    //!< jumps forward (or factor 0.0 for standing jump)
   ua_move_slide = 16,  //!< slides right (or left)
   ua_move_float = 32,  //!< floats player up (or down)
};



// constants

//! max speed a player can walk, in tiles / second
const double ua_player_max_walk_speed = 2.4;

//! max speed a player can slide left or right
const double ua_player_max_slide_speed = 1.0;

//! max speed a player can rotate, in degrees / second
const double ua_player_max_rotate_speed = 90;

//! max speed a player can rotate the view, in degrees / second
const double ua_player_max_view_rotate_speed = 60;

//! x size of the player ellipsoid
const double ua_ellipsoid_x = 0.2;
//! y size of the player ellipsoid
const double ua_ellipsoid_y = 0.2;
//! z size of the player ellipsoid
const double ua_ellipsoid_z = 11.9;


   ellipsoid = ua_vector3d(ua_ellipsoid_x, ua_ellipsoid_y, ua_ellipsoid_z);
   fall_time = 0.0;
   fall_height_start = 0.0;


   
   move_factors[ua_move_walk] = 0.0;
   move_factors[ua_move_rotate] = 0.0;
   move_factors[ua_move_lookup] = 0.0;
   move_factors[ua_move_jump] = 0.0;
   move_factors[ua_move_slide] = 0.0;
   move_factors[ua_move_float] = 0.0;

   move_mode = 0;



   //! current movement mode
   unsigned int move_mode;

   //! movement factors map
   std::map<ua_player_movement_mode,double> move_factors;

   //! current fall time
   double fall_time;

   //! player height at fall start
   double fall_height_start;


  
   //! maximum pan angle
   double m_dMaxPanAngle;


   //! sets and delete movement mode values
   void set_movement_mode(unsigned int set,unsigned int del=0);

   //! sets movement factor for a given movement type; range is [-1.0; 1.0]
   void set_movement_factor(ua_player_movement_mode mode, double factor);


   //! returns movement mode
   unsigned int get_movement_mode() const;

   //! returns movement factor for given movement mode
   double get_movement_factor(ua_player_movement_mode mode);


   //! does rotation moves
   void rotate_move(double time_elapsed);



   // virtual methods from ua_physics_object
   virtual void set_new_elapsed_time(double time_elapsed);
   virtual ua_vector3d get_pos();
   virtual void set_pos(ua_vector3d& pos);
   virtual ua_vector3d get_dir();

   virtual void reset_gravity();
   virtual ua_vector3d get_gravity_force();
   virtual void hit_floor();


   max_panangle = underw.have_enhanced_features() ? 45.0 : 75.0;


void ua_player::set_movement_mode(unsigned int set,unsigned int del)
{
   move_mode = (move_mode | set) & (~del);
}


void ua_player::rotate_move(double time_elapsed)
{
   unsigned int mode = get_movement_mode();

   // player rotation
   if (mode & ua_move_rotate)
   {
      double angle = ua_player_max_rotate_speed * time_elapsed *
         get_movement_factor(ua_move_rotate);
      angle += get_angle_rot();

      // keep angle in range [0; 360]
      while(angle>360.0) angle -= 360.0;
      while(angle<0.0) angle += 360.0;

      set_angle_rot(angle);
   }

   // view up/down
   if (mode & ua_move_lookup)
   {
      double viewangle = get_angle_pan();
      viewangle -= ua_player_max_view_rotate_speed * time_elapsed *
         get_movement_factor(ua_move_lookup);

      // restrict up-down view angle
      if (viewangle < -max_panangle) viewangle = -max_panangle;
      if (viewangle > max_panangle) viewangle = max_panangle;

      set_angle_pan(viewangle);
   }
}

void ua_player::set_new_elapsed_time(double time_elapsed)
{
   fall_time += time_elapsed;
}

bool my_movement;

ua_vector3d ua_player::get_dir()
{
   double time_elapsed = 0.05;

   ua_vector3d dir;

   unsigned int mode = get_movement_mode();

   if ((mode & ua_move_walk) || (mode & ua_move_slide))
   {
      double speed = ua_player_max_walk_speed*time_elapsed;
      double angle = get_angle_rot();

my_movement = true;

      // adjust angle for sliding
      if (mode & ua_move_slide)
      {
         angle -= 90.0;
         speed *= get_movement_factor(ua_move_slide);
         speed *= ua_player_max_slide_speed/ua_player_max_walk_speed;
      }
      else
         speed *= get_movement_factor(ua_move_walk);

      // construct direction vector
      dir.x = 1.0;
      dir.rotate_z(angle);
      dir *= speed;
   }
   return dir;
}

void ua_player::reset_gravity()
{
   fall_time = 0.0;
   fall_height_start = height;
}

ua_vector3d ua_player::get_gravity_force()
{
   double gravity = 15.0*fall_time*fall_time;

   // limit gravity for falling
   if (gravity > 10.0)
      gravity = 10.0;

   return ua_vector3d(0.0, 0.0, -gravity);
}

/*! \todo hurt player when falling from too high */
void ua_player::hit_floor()
{
   double fall_height = fall_height_start - height;
}
