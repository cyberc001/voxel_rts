render_hitboxes = true

require "./logic/game_object"
require "./logic/render_object"

local game_object_arr = {}

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(1, 1, 2), rot = vec3:new(0, 0, 0),
	--vel = vec3:new(0, -0.03, 0),
	hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))
--[[table.insert(game_object_arr, game_object:new({
	pos = vec3:new(4, 6, 2), rot = vec3:new(0, 0, 0),
	--vel = vec3:new(0, -0.03, 0),
	hitbox = gmath.hexahedron_from_cuboid(1, 2, 3),
	robj_arr = {
		render_object:new({model = render.model_find("kirov")})
	}
}))]]--

gravity = -0.05
goal = vec3:new(9.5, 1, 2.5)
p_i = 2

function _tick()
	if stop then return end
	-- handle velocity and collision
	for i,v in ipairs(game_object_arr) do
		-- PATHFINDING TEST
		if i == 1 then
			local center = gmath.hexahedron_get_center(v.hitbox)
			if p == nil then
				p = path.find_path(v.hitbox, goal)
				p_i = 1
			end

			if p[p_i] then
				--print("GOAL: ", p[p_i].x, p[p_i].y)
				local diff = vec3:new(p[p_i].x + 0.5, center.y, p[p_i].y + 0.5) - center
				--print("CURRENT: ", center.x, center.z, math.floor(center.x + (diff.x < 0 and 0.5 or -0.5)), math.floor(center.z + (diff.z < 0 and 0.5 or -0.5)))
				local new_rot = gmath.vec3_lookat_rot(v.rot, diff:unit())
				if v.vel:ln() <= diff:ln() and new_rot.y == new_rot.y then -- test for nan
					v.rot.y = v.rot.y + new_rot.y
				end

				if(diff:ln() > 0) then
					v.vel = diff:unit()*0.02
				else
					v.vel = vec3:new(0, 0, 0)
				end
				if(v.vel:ln() > diff:ln()) then
					v.vel = diff
					p_i = p_i + 1
				end
				if(p[p_i] and math.floor(center.x + (diff.x < 0 and 0.5 or -0.5)) == p[p_i].x and math.floor(center.z + (diff.z < 0 and 0.5 or -0.5)) == p[p_i].y) then
					p_i = p_i + 1
					if(not p[p_i]) then
						v.vel = vec3:new(0, 0, 0)
					end
				elseif(not p[p_i]) then
					v.vel = vec3:new(0, 0, 0)
				end
			end
		end

		vec3:iadd(v.pos, v.vel)
		v:update_hitbox()
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
			print("NEW ROT: ", new_rot.x, new_rot.y, new_rot.z)
			if new_rot.x == new_rot.x -- test for nan
				and (new_rot.x*new_rot.x + new_rot.y*new_rot.y + new_rot.z*new_rot.z) > 0.5 then -- TODO: properly convert vectors from C to objects, maybe do smth else about flickering
				v.rot.x = v.rot.x + (new_rot.x) * 0.1
				v.rot.y = v.rot.y + (new_rot.y) * 0.1
				v.rot.z = v.rot.z + (new_rot.z) * 0.1
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
