require "./logic/math/vec"

game_object = {}

function game_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.vel = o.vel or vec3:new()

	o.rot = o.rot or vec4:new(0, 0, 0, 1)
	o.rot_goal = o.rot or vec4:new(0, 0, 0, 1)

	o.path_forward = o.path_forward or vec3:new(1, 0, 0)

	o.size = o.size or vec3:new(1, 1, 1)
	o.base_hitbox = o.base_hitbox or gmath.hexahedron_from_cube(1)
	o.robj_arr = o.robj_arr or {}
end
function game_object:new(o)
	o = o or {}
	o.speed = o.speed or 0.01
	o.team = o.team or 1

	game_object:create_tables(o)
	setmetatable(o, self)
	self.__index = self
	o:update_hitbox()
	return o
end

function game_object:update_hitbox()
	self.hitbox = gmath.hexahedron_transform(self.base_hitbox, self.pos, self.rot, self.size)
	self.robj_hitbox = render.render_hexahedron(self.hitbox, player_selected_objects[self] == true and vec3:new(1, 0, 0) or nil)
	self.bbox = gmath.hexahedron_get_bbox(self.hitbox)
	self.interaction_box = gmath.hexahedron_get_interaction_box(self.hitbox)
end

-- pathfinding

function game_object:set_goal(goal)
	self.goal = goal
	self.path, self.path_pieces = path.find_path(self.base_hitbox, self.pos, goal)
	self.path_i = 1
end
function game_object:clear_goal(goal)
	self.goal = nil
	self.path = nil
	self.path_pieces = nil
end

function game_object:path_tick()
	if not self.moves_this_tick then
		self.vel = vec3:new(0, 0, 0)
		return
	end

	if self.path then
		local center = gmath.hexahedron_get_center(self.hitbox)
		local immgoal = self.path[self.path_i] -- immediate goal
		if self.path and immgoal then
			local diff = vec3:new(immgoal.x + 0.5, self.pos.y, immgoal.y + 0.5) - self.pos

			if(diff:ln() > 0) then
				self.vel = diff:unit() * self.speed
			else
				self.vel = vec3:new(0, 0, 0)
			end

			if(self.vel:ln() > diff:ln()) then -- set immedate destination
				self.vel = diff
				self.path_i = self.path_i + 1
			else
				if diff:ln() > 0 then
					self.path_forward = diff:unit()
				end
			end

			if self.path[self.path_i] and math.abs(self.pos.x - (immgoal.x + 0.5)) < 0.1 and math.abs(self.pos.z - (immgoal.y + 0.5)) < 0.1 then -- set immedate destination
				self.path_i = self.path_i + 1
				if(not self.path[self.path_i]) then
					self.vel = vec3:new(0, 0, 0)
					self:clear_goal()
					return
				end
			elseif(not self.path[self.path_i]) then
				self.vel = vec3:new(0, 0, 0)
				self:clear_goal()
				return
			end
		else
			self.vel = vec3:new(0, 0, 0)
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

-- callbacks for the engine

function game_object:tick()
end

function game_object:render()
	if render_hitboxes then
		render.render_obj_draw(self.robj_hitbox)
	end

	for _,v in ipairs(self.robj_arr) do
		v:render(self)
	end
end
