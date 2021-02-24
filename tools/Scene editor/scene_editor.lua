--[[ Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>   
   This program is free software. It comes without any warranty, to
	the extent permitted by applicable law. You can redistribute it
	and/or modify it under the terms of the Do What The Fuck You Want
	To Public License, Version 2, as published by Sam Hocevar. See
	the LICENSE file for more details.
]]

package.path = "./lib/?.lua;" .. package.path
package.cpath = "./lib/?.so;" .. package.cpath
local json = require("json")
local e4st3r = require("e4st3r")
local gentemp = require("gentemp")


MAX_SCENES = 128
NOFLAG = 0
CONSOLE_FLAG = 1
BUBBLE_FLAG = 2



--[[ Display help ]]

local function display_help ()
	print("Usage : " .. arg[0] .. " [help|build|clean|encrypt|gen_template]")
end


--[[ Remove files from output directory ]]

local function clean_output_dir (output_dir)
	for i = 1, MAX_SCENES do
		local f = io.open(output_dir .. "scene" .. i .. ".json")
		if not f then break end
		f:close()

		print("Removing json file of scene #" .. i .. " ...")
		os.remove(output_dir .. "scene" .. i .. ".json")
	end
end



--[[ Create a template default file example ]]

local function gen_default_temp (default_template)
	local f = assert(io.open(default_template, "w+"))
	f:write(gentemp.get_template())
	f:close()
end



--[[ if template exist, create json file ]]

local function json_encode_templates (templates_dir, output_dir)
	for i = 1, MAX_SCENES do
		local f = io.open(templates_dir .. "scene" .. i .. ".lua")
		if not f then break end
		f:close()

		local scene = assert(loadfile(templates_dir .. "scene" .. i .. ".lua"))()
		scene.flags = scene.flags or NOFLAG

		local output = json.encode(scene)
		print("Writing json file of scene #" .. i .. " : ")
		print(output)
		
		local f = assert(io.open(output_dir .. "scene" .. i .. ".json", "w+"))
		f:write(output)
		f:close()
	end
end


if arg[1] == "help" or arg[1] == "--help" then
	display_help()
elseif arg[1] == "clean" or arg[1] == "--clean" then
	clean_output_dir("output/")
elseif (arg[1] == "encrypt" or arg[1] == "--encrypt") and arg[2] then
	local st = tonumber(arg[3]) or 1337
	print(e4st3r.encode_to_hex(e4st3r.encrypt(arg[2], st)))
elseif (arg[1] == "build" or arg[1] == "--build") then
	json_encode_templates("templates/", "output/")
elseif (arg[1]) == "gen_template" or arg[1] == "--gen-template" then
	gen_default_temp("templates/default.lua")
else
	error("Invalid parameters, see " .. arg[0] .. " help")
end
