render_hitboxes = true

require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(2, 2, 5), rot = vec2:new(45, 45),
	hitbox = math.hexahedron_from_cuboid(1, 1, 1),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(0, 2, 5), rot = vec2:new(-45, 45),
	hitbox = math.hexahedron_from_cuboid(1, 1, 1),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))

print(math.hexahedron_check_collision(game_object_arr[1].hitbox, game_object_arr[2].hitbox))

function _tick()
	for _,v in ipairs(game_object_arr) do
		v:tick()
	end
end
function _render()
	for _,v in ipairs(game_object_arr) do
		v:render()
	end
end
