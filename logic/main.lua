render_hitboxes = true

require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(6, 2, 6), rot = vec2:new(30, 0),
	--vel = vec3:new(0, 0, -0.01),
	hitbox = math.hexahedron_from_cuboid(1, 2, 3),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(6, 2, 2), rot = vec2:new(0, -15),
	vel = vec3:new(0, 0.01, 0.03),
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
		local vel_subbed = false
		for i2,v2 in ipairs(game_object_arr) do
			if v == v2 then goto continue end
			local collided = math.hexahedron_check_collision(v.hitbox, v2.hitbox, v.vel)
			if collided then
				if not vel_subbed then
					vec3:isub(v.pos, v.vel)
					vel_subbed = true
				end
				v:update_hitbox()
			end
			::continue::
		end
		local collided = math.hexahedron_check_terrain_collision(v.hitbox, v.vel)
		if collided then
			if not vel_subbed then
				vec3:isub(v.pos, v.vel)
				vel_subbed = true
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
