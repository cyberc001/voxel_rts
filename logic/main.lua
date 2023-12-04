require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = {x = -5, y = -1, z = 1}, rot = {x = 15, y = 15},
	robj_arr = {
		render_object:new({model = game.model_find("kirov"), pos = {x = 10, y = 5, z = 5}, rot = {x = 45, y = 45}})
	}
}))

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
