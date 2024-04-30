require "./logic/team"

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

require "./logic/unit/grizzly_tank"

--test
table.insert(game_object_arr, grizzly_tank:new({
	pos = vec3:new(1.5, 2.8, 1.5), 
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	team = all_teams[1],
	robj_arr = {
		render_object:new({model = render.model_find("grizzly_tank_base"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.7, 0.7, 0.7)}),
		render_object:new({model = render.model_find("grizzly_tank_turret"), pos = vec3:new(0, 0.2, 0), size = vec3:new(0.7, 0.7, 0.7)}),
		render_object:new({model = render.model_find("grizzly_tank_barrel"), pos = vec3:new(0, 0.2, 0), size = vec3:new(1, 1, 1)})
	}
}))
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(5.5, 3.8, 2.5),
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	team = all_teams[2],
	robj_arr = {
		render_object:new({model = render.model_find("harvester"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)})
	}
}))

gravity = -0.03

function _first_tick()
end

function _tick()
	controls_tick()
	pointer_tick()

	-- handle velocity and collision
	
	cur_octree = octree:new(game_object_arr)

	for _,v in ipairs(game_object_arr) do
		vec3:iadd(v.pos, v.vel)
		vec3:iadd(v.pos, v.path_vel)
		v.pos.y = v.pos.y + gravity
		v.rot = vec4:new(gmath.interp_quat(v.rot, v.rot_goal, 0.1))
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
			resolution.y = resolution.y + gravity
			vec3:isub(v.pos, resolution)
			v.rot_goal = new_rot
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
	pointer_render()
end
