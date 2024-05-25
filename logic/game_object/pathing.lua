function game_object:set_goal(goal, distance, pitch_range)
	self.goal = goal

	-- temporarily remove object from octree
	local clusters = cur_octree.obj_clusters[self]
	for cluster_i in pairs(clusters) do
		cur_octree.clusters[cluster_i][self] = nil
	end
	self.path, self.path_pieces = path.find_path(self.base_hitbox, self.pos, goal, distance, pitch_range)
	-- restore object in octree
	for cluster_i in pairs(clusters) do
		cur_octree.clusters[cluster_i][self] = true
	end

	local immgoal = self.path[1]
	local diff = immgoal and vec3:new(self.pos.x - immgoal.x, 0, self.pos.z - immgoal.y) or vec3:new(math.huge, math.huge, math.huge)
	self.path_i = diff:ln() <= 1 and 2 or 1 -- skip first path point if the object is already on it (or less than a block away)
end
function game_object:clear_goal(goal)
	self.goal = nil
	self.path = nil
	self.path_pieces = nil
end


function game_object:get_movement_force()
	return 30
end
function game_object:accel_to_path()
	if not self.last_resolution then self.last_resolution = vec3:new() end
	local diff = (self.path_vel - self.last_resolution) - self.vel
	diff.y = 0
	local force = self:get_movement_force() * diff:safe_unit()
	if math.abs(force.x / self.mass) > math.abs(diff.x) then force.x = diff.x end
	if math.abs(force.y / self.mass) > math.abs(diff.y) then force.y = diff.y end
	if math.abs(force.z / self.mass) > math.abs(diff.z) then force.z = diff.z end
	self:apply_force(force)
end

function game_object:path_tick()
	if self.path then self:accel_to_path() end
	self.path_vel = vec3:new(0, 0, 0)
	if not self.moves_this_tick then return end

	if self.path then
		local center = gmath.hexahedron_get_center(self.hitbox)
		local immgoal = self.path[self.path_i] -- immediate goal
		if self.path and immgoal then
			local diff = vec3:new(immgoal.x + 0.5, self.pos.y, immgoal.y + 0.5) - self.pos

			if(diff:ln() > 0) then
				self.path_vel = diff:unit() * self.max_speed
			end
			if diff:ln() > 0 then
				self.path_forward = diff:unit()
			end

			if self.path[self.path_i] and math.abs(self.pos.x - (immgoal.x + 0.5)) < 0.1 and math.abs(self.pos.z - (immgoal.y + 0.5)) < 0.1 then -- set immedate destination	
				self.path_i = self.path_i + 1
				if(not self.path[self.path_i]) then
					self.path_vel = vec3:new(0, 0, 0)
					self:clear_goal()
					return
				end
			end
		end
	end
end

function game_object:path_remaining()
	if self.path == nil then
		return math.huge
	end
	return #self.path - self.path_i
end
function game_object.decide_who_moves(o1, o2)
	if not o1.moves_this_tick or not o2.moves_this_tick then
		return
	end

	local rem1, rem2 = o1:path_remaining(), o2:path_remaining()
	if rem1 < rem2 then
		o2.moves_this_tick = false
	else
		o1.moves_this_tick = false
	end
end
