require "./logic/math/trig"

-- test unit for debugging purposes

grizzly_tank = game_object:new()

function grizzly_tank:tick(time_delta)
	game_object.tick(self, time_delta)

	if self.target then
		local new_rot, new_rot_xz = self:get_rotations_to(self.target.pos)
		self.robj_arr[2].rot = vec4:new(gmath.interp_quat(self.robj_arr[2].rot, new_rot_xz, self.part_rot_speed * time_delta))
		self.robj_arr[3].rot = vec4:new(gmath.interp_quat(self.robj_arr[3].rot, new_rot, self.part_rot_speed * time_delta))
	else
		self.robj_arr[2].rot = vec4:new(0, 0, 0, 1)
		self.robj_arr[3].rot = vec4:new(0, 0, 0, 1)
	end
end
