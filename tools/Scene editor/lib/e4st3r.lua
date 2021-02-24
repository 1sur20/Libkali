--[[ Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>   
   This program is free software. It comes without any warranty, to
	the extent permitted by applicable law. You can redistribute it
	and/or modify it under the terms of the Do What The Fuck You Want
	To Public License, Version 2, as published by Sam Hocevar. See
	the LICENSE file for more details.
]]

local m = {}


local hex_rep = {
   [0] = "0",
   [1] = "1",
   [2] = "2",
   [3] = "3",
   [4] = "4",
   [5] = "5",
   [6] = "6",
   [7] = "7",
   [8] = "8",
   [9] = "9",
   [10] = "a",
   [11] = "b",
   [12] = "c",
   [13] = "d",
   [14] = "e",
   [15] = "f"
}


local function xor (a, b)
  return (a | b) & ~(a & b)
end



function m.encode_to_hex (str)
   local hex_str = ""

   for i = 1, #str do
      local byte = str:byte(i)
      local h = math.floor(byte/16)
      local l = byte - (h*16)

      hex_str = hex_str .. "\\x" .. hex_rep[h] .. hex_rep[l]
   end

   return hex_str
end



function m.encrypt (message, st)
   local enc_message = ""
   for i = 1, #message do
      local key = (i-1+st)%255
      enc_message = enc_message .. string.char(xor(message:byte(i), key))
   end

   return enc_message
end



return m