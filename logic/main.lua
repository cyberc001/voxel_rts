render_hitboxes = true

require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(1.5, 1.8, 2.5), rot = vec3:new(0, 0, 0),
	--vel = vec3:new(0, -0.03, 0),
	hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))

gravity = -0.05

function _first_tick()
	game_object_arr[1]:set_goal(vec3:new(1, 10, 6))
end

function _tick()
	if stop then return end
	-- handle velocity and collision
	for i,v in ipairs(game_object_arr) do
		v:path_tick()

		vec3:iadd(v.pos, v.vel)
		v.pos.y = v.pos.y + gravity
		v:update_hitbox()

		for i2,v2 in ipairs(game_object_arr) do
			if v == v2 then goto continue end
			local collided, resolution = gmath.hexahedron_check_collision(v.hitbox, v2.hitbox, v.vel)
			if collided then
				vec3:isub(v.pos, resolution)
				v:update_hitbox()
			end
			::continue::
		end
		local collided, resolution, new_rot = gmath.hexahedron_check_terrain_collision(v.hitbox, v.rot)
		if collided then
			resolution.y = resolution.y + gravity
			vec3:isub(v.pos, resolution)
			local center = gmath.hexahedron_get_center(v.hitbox)
			--print("NEW ROT: ", new_rot.x, new_rot.y, new_rot.z)
			if new_rot.x == new_rot.x -- test for nan
				then -- TODO: properly convert vectors from C to objects
				v.rot.x = v.rot.x + new_rot.x * 0.1
				v.rot.y = v.rot.y + new_rot.y * 0.1
				v.rot.z = v.rot.z + new_rot.z * 0.1
			end
			v:update_hitbox()
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
