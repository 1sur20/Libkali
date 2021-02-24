local m = {}



function m.get_template ()
   return [[
local scene = {
   name = "menu",
   id = 1,
   background = "data/backgrounds/background.png",
   charmap = "data/sprites/csprite.png",
   sprites_path = "data/sprites/spritesheet.png",
   sprites = {
      {
         --ghost 1
         sel_x = 0,
         sel_y = 240,
         sel_w = 80,
         sel_h = 80,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 9,
         loop_frame = 6,
         frame_delay = 150,
         x = 10,
         y = 100,
         w = 240,
         h = 240,
         active_hover = 1,
         active = 1
      },
      {
         --ghost 2
         sel_x = 0,
         sel_y = 0,
         sel_w = 80,
         sel_h = 80,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 9,
         loop_frame = 6,
         frame_delay = 150,
         x = 715,
         y = 100,
         w = 240,
         h = 240,
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
      },
      {
         --start button
         sel_x = 0,
         sel_y = 320,
         sel_w = 96,
         sel_h = 32,
         start_frame = 0,
         curr_frame = 0,
         end_frame = 1,
         loop_frame = 1,
         click_frame = 2,
         frame_delay = 150,
         x = 384,
         y = 465,
         w = 192,
         h = 64,
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
         x = 384,
         y = 465,
         w = 192,
         h = 64,
         type = 1,
         action_name = "new-scene",
         action_arg = "data/json/scene2.json",
         button = "left-click"
      }
   },
   flags = CONSOLE_FLAG,
   cursor = {
      sprites_path = "data/sprites/cursor.png",
      sel_x = 0, --default cursor select
      sel_y = 0,
      sel_w = 16,
      sel_h = 16,
      --[==[
      cursor_events = {
         {
            sel_x = 16,
            sel_y = 0,
            sel_w = 15,
            sel_h = 16,
            x = 400,
            y = 400,
            w = 400,
            h = 150
         }
      }]==]
   }
}
      
      
return scene
]]
end



return m