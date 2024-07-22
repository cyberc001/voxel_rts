require "./logic/math/trig"

part = {}

part_rot_axis = {
	horizontal = 0x1,
	vertical = 0x2,
	all = 0x3
}

function part:new(o)
	o = o or {}
	o.rot = vec4:new(0, 0, 0, 1)
	o.rot_speed = o.rot_speed or 5
	o.rot_axis = o.rot_axis or part_rot_axis.all
	
	setmetatable(o, self)
	self.__index = self

	o:reset_rotation()
	return o
end

function part:get_rot()
	return self.gobj.rot * self.robj.rot
end
function part:is_rotated_to_target()
	local diff = gmath.axis_from_quat(self.rot:inv() * self.rot_goal)
	return vec3:new(diff):ln() < 0.01
end

function part:rotate_to(target)
	local prot = self.parent and self.parent.rot_goal:inv() or vec4:new(0, 0, 0, 1)
	if self.rot_axis == part_rot_axis.horizontal then
		self.rot_goal = prot * self.gobj:get_xz_rotation_to(target)
	elseif self.rot_axis == part_rot_axis.vertical then
		prot.x = 0
		prot.y = 0
		self.rot_goal = prot * self.gobj:get_y_rotation_to(target)
	else
		prot.z = 0
		self.rot_goal = prot * self.gobj:get_rotation_to(target)
	end
end
function part:reset_rotation()
	self.rot_goal = vec4:new(0, 0, 0, 1)
end

function part:tick(time_delta)
	self.rot = vec4:new(gmath.interp_quat(self.rot, self.rot_goal, self.rot_speed * time_delta))
	self.robj.rot = (self.parent and self.parent.rot or vec4:new(0, 0, 0, 1)) * self.rot
end

-- game_object methods
function game_object:add_part(p, robj_idx)
	table.insert(self.parts, p)
	p.robj = self.robj_arr[robj_idx]
	p.gobj = self
end

function game_object:rotate_parts(target)
	for _,v in ipairs(self.parts) do
		v:rotate_to(target)
	end
end
function game_object:reset_parts_rotation()
	for _,v in ipairs(self.parts) do
		v:reset_rotation()
	end
end

function game_object:tick_parts(time_delta)
	for _,v in ipairs(self.parts) do
		v:tick(time_delta)
	end
end
