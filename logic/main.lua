require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(-5, -1, 1), rot = vec2:new(15, 15),
	robj_arr = {
		render_object:new({model = game.model_find("kirov"), pos = vec3:new(10, 5, 5), rot = vec2:new(45, 45)})
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
