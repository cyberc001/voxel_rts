require "./logic/combat/team"

render_hitboxes = true

game_object_arr = {}

all_teams[1] = team:new()
all_teams[2] = team:new()
player_team = all_teams[1]

require "./logic/game_object"
require "./logic/render_object"
require "./logic/controls"
require "./logic/pointer"
require "./logic/math/octree"
require "./logic/combat/weapons/cannon"
require "/.logic/physics"

local _o1 = shootable:new({
	pos = vec3:new(2.5, 3.8, 0.5), 
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	team = all_teams[1],
	robj_arr = {
		render_object:new({model = render.model_find("grizzly_tank_base"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.7, 0.7, 0.7)}),
		render_object:new({model = render.model_find("grizzly_tank_turret"), pos = vec3:new(0, 0.2, 0), size = vec3:new(0.7, 0.7, 0.7)}),
		render_object:new({model = render.model_find("grizzly_tank_barrel"), pos = vec3:new(0, 0.2, 0), size = vec3:new(1, 1, 1)})
	}
})
_o1:add_part(part:new({rot_axis = part_rot_axis.horizontal}), 2)
_o1:add_part(cannon:new({rot_axis = part_rot_axis.vertical, parent = _o1.parts[1],
			proj_factory = function()
				return projectile:new({
					mass = 0.01,
					base_hitbox = gmath.hexahedron_from_cuboid_centered(0.1, 0.1, 0.1),
					robj_arr = {
						render_object:new({model = render.model_find("test_projectile")})
					}
				})
			end
		}), 3)
game_object_arr[_o1] = true

local _o2 = shootable:new({
	pos = vec3:new(6.5, 3.8, 2.5),
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	team = all_teams[2],
	robj_arr = {
		render_object:new({model = render.model_find("harvester"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)})
	}
})
game_object_arr[_o2] = true

local elasticity = 0.3
local static_fric_coff = 0.2
local kinetic_fric_coff = 0.2

function _first_tick()
end

function _tick(time_delta)
	controls_tick()
	pointer_tick(time_delta)

	-- handle velocity and collision
	
	cur_octree = octree:new(game_object_arr)

	for v,_ in pairs(game_object_arr) do
		vec3:iadd(v.pos, v.vel*time_delta)
		vec3:iadd(v.vel, gravity_accel*time_delta)
		v.rot = vec4:new(gmath.interp_quat(v.rot, v.rot_goal, 10*time_delta))
		v:update_hitbox()

		v.moves_this_tick = true

		local clusters = cur_octree.obj_clusters[v]
		local checked_objects = {}
		for cluster_i in pairs(clusters) do -- for each cluster that intersects with the object
			for v2 in pairs(cur_octree.clusters[cluster_i]) do -- check collision with objects in these clusters
				if v ~= v2 and checked_objects[v2] == nil then
					checked_objects[v2] = true
					local collided, resolution = gmath.hexahedron_check_collision(v.hitbox, v2.hitbox, v.vel)
					if collided then
						vec3:isub(v.pos, resolution)
						v:update_hitbox()
						if resolution.x == resolution.x then
							resolution = vec3:new(resolution)
							collision_response(resolution, v, v2)
						end

						v:on_object_collision(v2, resolution)
					end
		
					collided = gmath.bbox_check_collision(v.interaction_box, v2.interaction_box)
					if collided then
						game_object.decide_who_moves(v, v2)
					end
				end
			end
		end
	
		local collided, resolution, new_rot = gmath.hexahedron_check_terrain_collision(v.hitbox, v.path_forward)
		if collided then
			resolution = vec3:new(resolution)
			vec3:isub(v.pos, resolution)
			v.last_resolution = -resolution * (1/time_delta)

			if resolution.x == resolution.x then
				static_collision_response(resolution, v)
				friction_deceleration(resolution, v)
			end

			v.rot_goal = new_rot
			v:update_hitbox()
		end
		vec3:iadd(v.vel, v.force * (1/v.mass))
		v.force = vec3:new()
	end

	for v,_ in pairs(game_object_arr) do
		v:tick(time_delta)
	end

	-- prepare render objects; actual rendering is done on a separate thread
	for v,_ in pairs(game_object_arr) do
		v:render()
	end
	pointer_render()
	render.swap_buffers()
end
