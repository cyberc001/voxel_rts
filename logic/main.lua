render_hitboxes = true

game_object_arr = {}

player_team = 1

require "./logic/game_object"
require "./logic/render_object"
require "./logic/controls"

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(1.5, 10.5, 6.5), --vec3:new(1.5, 1.8, 4.5), 
	hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("harvester"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)})
	}
}))
--[[table.insert(game_object_arr, game_object:new({
	pos = vec3:new(1.5, 1.8, 0.5), rot = vec3:new(0, 0, 0),
	hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))]]--

--gravity = 0
gravity = -0.03

function _first_tick()
	path.occupy_space(game_object_arr[1].hitbox)
	--path.occupy_space(game_object_arr[2].hitbox)
end

function _tick()
	controls_tick()

	-- handle velocity and collision
	for i,v in ipairs(game_object_arr) do
		path.deoccupy_space(v.hitbox)
		v:path_tick()

		vec3:iadd(v.pos, v.vel)
		v.pos.y = v.pos.y + gravity
		v.rot = gmath.interp_quat(v.rot, v.rot_goal, 0.1)
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
		local collided, resolution, new_rot = gmath.hexahedron_check_terrain_collision(v.hitbox, v.path_forward)
		if collided then
			resolution.y = resolution.y + gravity
			vec3:isub(v.pos, resolution)
			v.rot_goal = new_rot
			v:update_hitbox()
		end
		path.occupy_space(v.hitbox)
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
