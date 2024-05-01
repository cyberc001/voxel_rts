require "./logic/math/trig"

-- test unit for debugging purposes

grizzly_tank = game_object:new()

function grizzly_tank:tick()
	game_object.tick(self)

	if self.target then
		local diff_xz = vec3:new(self.target.pos.x, self.pos.y, self.target.pos.z) - self.pos
		local diff = self.target.pos - self.pos
		print("diff", diff)

		local pitch = rad_to_ang(ang_between(vec3:new(1, 0, 0), vec3:new(1, self.target.pos.y - self.pos.y, 0)))
		local pitch_range = self:get_targeting_pitch_range()
		if pitch > pitch_range.y then pitch = pitch_range.y end
		if pitch < pitch_range.x then pitch = pitch_range.x end
		pitch = ang_to_rad(pitch)
		local diff_y = rot_vec2(vec2:new(1, 0), pitch) -- rotate (1, 0, 0) to clamped pitch
		diff.y = diff_y.y * (diff.x / diff_y.x) -- rescale rotated vector to match diff

		local new_rot_xz = self.rot:inv() * gmath.quat_rot_from_vec3(diff_xz)
		new_rot_xz.z = 0
		local new_rot = self.rot:inv() * gmath.quat_rot_from_vec3(diff)

		self.robj_arr[2].rot = vec4:new(new_rot_xz)
		self.robj_arr[3].rot = vec4:new(new_rot)
	else
		self.robj_arr[2].rot = vec4:new(0, 0, 0, 1)
		self.robj_arr[3].rot = vec4:new(0, 0, 0, 1)
	end
end
