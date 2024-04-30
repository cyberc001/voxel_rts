-- test unit for debugging purposes

grizzly_tank = game_object:new()

function grizzly_tank:tick()
	game_object.tick(self)

	if self.target then
		local diff = self.target.pos - self.pos
		local new_rot = self.rot:inv() * gmath.quat_rot_from_vec3(diff)
		self.robj_arr[2].rot = vec4:new(new_rot)
		self.robj_arr[3].rot = vec4:new(new_rot)
	else
		self.robj_arr[2].rot = vec4:new(0, 0, 0, 1)
		self.robj_arr[3].rot = vec4:new(0, 0, 0, 1)
	end
end
