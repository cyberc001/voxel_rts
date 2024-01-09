render_hitboxes = true

require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(6, 2, 6), rot = vec2:new(45, 45),
	vel = vec3:new(-0.015, 0, 0),
	hitbox = math.hexahedron_from_cuboid(1, 2, 3),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(0, 2, 5), rot = vec2:new(-45, -45),
	vel = vec3:new(0.007, 0, 0),
	hitbox = math.hexahedron_from_cuboid(1, 2, 3),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))

--print(math.hexahedron_check_collision(game_object_arr[1].hitbox, game_object_arr[2].hitbox))

function _tick()
	-- handle velocity and collision
	for i,v in ipairs(game_object_arr) do
		vec3:iadd(v.pos, v.vel)
		v:update_hitbox()
		for i2,v2 in ipairs(game_object_arr) do
			if v == v2 then goto continue end
			local collided, resolution = math.hexahedron_check_collision(v.hitbox, v2.hitbox, v.vel)
			if collided then
				vec3:isub(v.pos, v.vel)
				vec3:iadd(v.pos, resolution)
				v:update_hitbox()
			end
			::continue::
		end
	end

	for _,v in ipairs(game_object_arr) do
		v:tick()
	end
end
function _render()
	for _,v in ipairs(game_object_arr) do
		v:render()
	end
end
