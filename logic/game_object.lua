require "./logic/math/vec"

game_object = {}

function game_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.vel = o.vel or vec3:new()
	o.rot = o.rot or vec3:new()
	o.size = o.size or vec3:new(1, 1, 1)
	o.base_hitbox = o.hitbox or gmath.hexahedron_from_cube(0)
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
end

-- pathfinding

function game_object:set_goal(goal)
	self.goal = goal
	self.path, self.path_pieces = path.find_path(self.hitbox, goal)
	self.path_i = 1
end
function game_object:clear_goal(goal)
	self.goal = nil
	self.path = nil
	self.path_pieces = nil
end

function game_object:path_tick()
	if self.path then
		local center = gmath.hexahedron_get_center(self.hitbox)
		if self.path and self.path[self.path_i] then
			if path.is_space_occupied(self.path_pieces[self.path_i]) then
				self.vel = vec3:new(0, 0, 0)
				return
			end

			local diff = vec3:new(self.path[self.path_i].x + 0.5, center.y, self.path[self.path_i].y + 0.5) - center
			local new_rot = gmath.vec3_lookat_rot(self.rot, diff:unit())
			if self.vel:ln() <= diff:ln() and new_rot.y == new_rot.y then -- test for nan
				self.rot.y = self.rot.y + new_rot.y * 0.1
			end

			if(diff:ln() > 0) then
				self.vel = diff:unit() * self.speed
			else
				self.vel = vec3:new(0, 0, 0)
			end
			if(self.vel:ln() > diff:ln()) then
				self.vel = diff
				self.path_i = self.path_i + 1
			end
			if(self.path[self.path_i] and math.floor(center.x) == self.path[self.path_i].x and math.floor(center.z) == self.path[self.path_i].y) then
				self.path_i = self.path_i + 1
				if(not self.path[self.path_i]) then
					self.vel = vec3:new(0, 0, 0)
				end
			elseif(not self.path[self.path_i]) then
				v.vel = vec3:new(0, 0, 0)
				self:clear_goal()
			end
		end
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
		--v:render(self)
	end
end
