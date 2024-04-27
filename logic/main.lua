render_hitboxes = true

game_object_arr = {}

player_team = 1

require "./logic/game_object"
require "./logic/render_object"
require "./logic/controls"
require "./logic/pointer"
require "./logic/math/octree"

--test
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(4.5, 1.8, 4.5), 
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("harvester"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)})
	}
}))
table.insert(game_object_arr, game_object:new({
	pos = vec3:new(1.5, 1.8, 0.5),
	base_hitbox = gmath.hexahedron_from_cuboid_centered(0.8, 0.8, 0.8),
	robj_arr = {
		render_object:new({model = render.model_find("harvester"), pos = vec3:new(0, -0.1, 0), size = vec3:new(0.5, 0.5, 0.5)})
	}
}))

gravity = -0.03

function _first_tick()
	--[[for i,cluster in pairs(oc.clusters) do
		print("cluster", i, "depth", oc.depths[i])
		print("min corner", oc.bboxes[i][1])
		print("max corner", oc.bboxes[i][2])
		for k in pairs(cluster) do
			print("", k)
			print("", vec3.tostring(k.bbox[1]), vec3.tostring(k.bbox[2]))
		end
	end
	print(",,,,,,,,,,,,obj_cluster,,,,,,,,,,,,,,")
	for obj,clusters in pairs(oc.obj_clusters) do
		print("obj", obj)
		for k in pairs(clusters) do
			print("", k)
		end
	end]]--
end

function _tick()
	controls_tick()
	pointer_tick()

	-- handle velocity and collision
	
	local oc = octree:new(game_object_arr)

	for _,v in ipairs(game_object_arr) do
		vec3:iadd(v.pos, v.vel)
		v.pos.y = v.pos.y + gravity
		v.rot = gmath.interp_quat(v.rot, v.rot_goal, 0.1)
		v:update_hitbox()

		v.moves_this_tick = true

		local clusters = oc.obj_clusters[v]
		for cluster_i in pairs(clusters) do -- for each cluster that intersects with the object
			for v2 in pairs(oc.clusters[cluster_i]) do -- check collision with objects in these clusters
				if v ~= v2 then
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
		v:path_tick()
		v:tick()
	end
end
function _render()
	for _,v in ipairs(game_object_arr) do
		v:render()
	end
	pointer_render()
end
