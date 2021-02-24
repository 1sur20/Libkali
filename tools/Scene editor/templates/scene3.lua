local scene = {
   name = "room-computer",
   id = 2,
   background = "data/backgrounds/background3.png",
   --charmap = "data/csprite.png",
   sprites_path = "data/sprites/spritesheet.png",
   sprites = {
      {
         --fullscreen button
         sel_x = 320,
         sel_y = 320,
         sel_w = 16,
         sel_h = 16,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 1,
         loop_frame = 1,
         frame_delay = 150,
         x = 896,
         y = 0,
         w = 64,
         h = 64,
         active_hover = 1,
         active = 1
      }, 
      {
         --menu button
         sel_x = 352,
         sel_y = 320,
         sel_w = 16,
         sel_h = 16,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 1,
         loop_frame = 1,
         frame_delay = 150,
         x = 10,
         y = 10,
         w = 32,
         h = 32,
         active_hover = 1,
         active = 1
      }
   },
   hitboxes = {
      {
         x = 905,
         y = 5,
         w = 50,
         h = 50,
         type = 0,
         action_name = "fullscreen",
         action_arg = nil,
         button = "left-click"
      },
      {
         x = 10,
         y = 10,
         w = 32,
         h = 32,
         type = 1,
         action_name = "new-scene",
         action_arg = "data/json/scene1.json",
         button = "left-click"
      },
      {
         --to main room (scene 2)
         x = 0,
         y = 440,
         w = 960,
         h = 100,
         type = 1,
         action_name = "new-scene",
         action_arg = "data/json/scene2.json",
         button = "left-click"
      }
   },
   flags = NOFLAG,
   cursor = {
      sprites_path = "data/sprites/cursor.png",
      sel_x = 0, --default cursor select
      sel_y = 0,
      sel_w = 16,
      sel_h = 16,
      cursor_events = {
         --cursor to main room (scene 2)
         {
            sel_x = 48,
            sel_y = 64,
            sel_w = 16,
            sel_h = 16,
            x = 0,
            y = 440,
            w = 960,
            h = 100,
         }
      }
   }
}


return scene