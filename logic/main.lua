require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({robj_arr = {
							render_object:new({model = game.model_find("kirov"), pos_x = 10, pos_y = 5, pos_z = 5, angle = 45, pitch = 45})
						}}))

function tick()
	for _,v in ipairs(game_object_arr) do
		v:tick()
	end
end
function render()
	for _,v in ipairs(game_object_arr) do
		v:render()
	end
end
