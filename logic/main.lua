require "./logic/combat/team"

render_hitboxes = true

game_object_arr = {}

all_teams[1] = team:new({name = "player"})
all_teams[2] = team:new({name = "enemy"})
player_team = all_teams[1]

require "./logic/game_object"
require "./logic/render_object"
require "./logic/controls"
require "./logic/pointer"
require "./logic/math/octree"
require "./logic/combat/weapons/cannon"
require "./logic/physics"
require "./logic/math/basic"

stop = false

local _o1 = shootable:new({
	body = body.new_cuboid(0.8),

	team = all_teams[1],
	robj_arr = {
		render_object:new({pos = vec3:new(0, -0.1, 0), size = vec3:new(0.7, 0.7, 0.7)}, "grizzly_tank_base"),
		render_object:new({pos = vec3:new(0, 0.2, 0), size = vec3:new(0.7, 0.7, 0.7)}, "grizzly_tank_turret"),
		render_object:new({pos = vec3:new(0, 0.2, 0), size = vec3:new(1, 1, 1)}, "grizzly_tank_barrel")
	}
})
_o1:add_part(part:new({rot_axis = part_rot_axis.horizontal}), 2)
_o1:add_part(cannon:new({rot_axis = part_rot_axis.vertical, parent = _o1.parts[1],
			proj_factory = function()
				return projectile:new({
					mass = 0.01,
					body = body.new_cuboid(0.2),
					robj_arr = {
						render_object:new({model = render.model_find("test_projectile")})
					}
				})
			end
		}), 3)
_o1.body.pos = vec3:new(1.5, 2, 2.5)
game_object_arr[_o1] = true

local _o2 = shootable:new({
	body = body.new_cuboid(0.8),
	team = all_teams[2],
	robj_arr = {
		render_object:new({pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)}, "harvester")
	}
})
_o2.body.pos = vec3:new(1.5, 4, 4.5)
game_object_arr[_o2] = true

function _first_tick()
end

function _tick(time_delta)
	if stop then return end
	controls_tick()
	pointer_tick(time_delta)

	-- handle velocity and collision
	
	cur_octree = octree:new(game_object_arr)

	for v,_ in pairs(game_object_arr) do
		v.body.pos = v.body.pos + v.vel*time_delta
		local v_total_rot_vel = v.rot_vel + v.resolution_rot_vel
		v.body.rot = (v.body.rot + 0.5*v.body.rot*vec4:new(v_total_rot_vel.x, v_total_rot_vel.y, v_total_rot_vel.z, 0)*time_delta):unit()
		v.vel = v.vel + gravity_accel*time_delta

		v.moves_this_tick = true

		local clusters = cur_octree.obj_clusters[v]
		local checked_objects = {}
		if clusters then
			for cluster_i in pairs(clusters) do -- for each cluster that intersects with the object
				for v2 in pairs(cur_octree.clusters[cluster_i]) do -- check collision with objects in these clusters
					if v2.only_receive_collision then
						checked_objects[v2] = true
					end
					if v ~= v2 and checked_objects[v2] == nil then
						checked_objects[v2] = true
						local collided, resolution = v.body:check_collision(v2.body)
						if collided and v:on_object_collision(v2, resolution) then
							v.body.pos = v.body.pos + resolution
							if resolution.x == resolution.x then
								resolution = vec3:new(resolution)
								collision_response(resolution, v, v2)
							end
						end
	
						collided = gmath.bbox_check_collision(v.body:get_bbox(), v2.body:get_bbox())
						if collided then
							game_object.decide_who_moves(v, v2)
						end
					end
				end
			end
		end

		local path_forward = v.path and v.path_forward or gmath.vec3_quat_rot(vec3:new(1, 0, 0), v.body.rot)
		if not v.path then
			path_forward.y = 0
			path_forward = path_forward:safe_unit()
		end

		local collided, resolution, new_rot = v.body:check_terrain_collision(path_forward)
		if collided then
			v.body.pos = v.body.pos + resolution
			v.last_resolution = resolution * (1/time_delta)

			if resolution.x == resolution.x then
				static_collision_response(resolution, v)
				friction_deceleration(resolution, v)
			end

			new_rot = vec4:new(new_rot)
			local new_rot_vel = vec3:new(gmath.axis_from_quat(vec4:new(v.body.rot):inv() * new_rot))
			new_rot_vel = new_rot_vel * v.max_resolution_rot_vel
			local rot_vel_diff = new_rot_vel - (v.path and v_total_rot_vel or v.resolution_rot_vel)
			if rot_vel_diff:ln() > v.resolution_rot_accel then
				rot_vel_diff = rot_vel_diff:safe_unit() * v.resolution_rot_accel
			end
			v.resolution_rot_vel = v.resolution_rot_vel + rot_vel_diff
		end
		v.vel = v.vel + v.force * (1/v.mass)
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
