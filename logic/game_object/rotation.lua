require "./logic/math/trig"

function game_object:get_rotation_to(target)
	local diff = target - self.pos

	local rot = gmath.rot_from_quat(gmath.quat_rot_from_vec3(diff))
	local pitch = rot.y
	local pitch_range = self:get_targeting_pitch_range()
	if pitch > pitch_range.y then pitch = pitch_range.y end
	if pitch < pitch_range.x then pitch = pitch_range.x end
	pitch = ang_to_rad(pitch)
	local diff_y = rot_vec2(vec2:new(1, 0), pitch) -- rotate (1, 0, 0) to clamped pitch
	diff.y = diff_y.y * (diff.x / diff_y.x) -- rescale rotated vector to match diff

	return self.rot:inv() * gmath.quat_rot_from_vec3(diff)
end
function game_object:get_xz_rotation_to(target)
	local diff_xz = vec3:new(target.x, self.pos.y, target.z) - self.pos
	local inv = self.rot:inv()
	inv.z = 0
	local new_rot_xz = inv * gmath.quat_rot_from_vec3(diff_xz)
	return new_rot_xz
end
function game_object:get_y_rotation_to(target)
	local diff = self.pos - target

	local rot = gmath.rot_from_quat(gmath.quat_rot_from_vec3(diff))
	local pitch = rot.y
	local pitch_range = self:get_targeting_pitch_range()
	if pitch > pitch_range.y then pitch = pitch_range.y end
	if pitch < pitch_range.x then pitch = pitch_range.x end
	pitch = ang_to_rad(pitch)
	local diff_y = rot_vec2(vec2:new(1, 0), pitch) -- rotate (1, 0, 0) to clamped pitch
	local inv = self.rot:inv()
	inv.x = 0
	inv.y = 0
	local new_rot_y = inv * gmath.quat_rot_from_vec3(vec3:new(diff_y.x, diff_y.y, 0))
	return new_rot_y
end
