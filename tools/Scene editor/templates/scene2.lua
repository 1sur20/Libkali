local scene = {
   name = "room-start",
   id = 2,
   background = "data/backgrounds/background2.png",
   --charmap = "data/csprite.png",
   sprites_path = "data/sprites/spritesheet.png",
   sprites = {
      {
         --ghost 1
         sel_x = 0,
         sel_y = 80,
         sel_w = 80,
         sel_h = 80,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 9,
         loop_frame = 6,
         frame_delay = 150,
         x = 100,
         y = 300,
         w = 240,
         h = 240,
         active_hover = 1,
         active = 1
      },
      {
         --ghost 2
         sel_x = 0,
         sel_y = 160,
         sel_w = 80,
         sel_h = 80,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 9,
         loop_frame = 9,
         frame_delay = 90,
         x = 480,
         y = 290,
         w = 160,
         h = 160,
         active_hover = 1,
         active = 1         
      },
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
         --to computer room (scene 3)
         x = 200,
         y = 200,
         w = 390,
         h = 200,
         type = 1,
         action_name = "new-scene",
         action_arg = "data/json/scene3.json",
         button = "left-click"
      },
      {
         --to bed room (scene 4)
         x = 700,
         y = 0,
         w = 260,
         h = 540,
         type = 1,
         action_name = "new-scene",
         action_arg = "data/json/scene4.json",
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
         --cursor to computer room (scene 3)
         {
            sel_x = 32,
            sel_y = 64,
            sel_w = 16,
            sel_h = 16,
            x = 200,
            y = 200,
            w = 390,
            h = 200
         },
         --cursor to bed room (scene 4)
         {
            sel_x = 32,
            sel_y = 64,
            sel_w = 16,
            sel_h = 16,
            x = 700,
            y = 0,
            w = 260,
            h = 540
         }
      }
   }
}


return scene