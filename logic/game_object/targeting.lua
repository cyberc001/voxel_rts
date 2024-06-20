local game_object_target_path_interval = 1

function game_object:get_targeting_range()
	return 10
end
function game_object:get_targeting_pitch_range()
	return vec2:new(-45, 45)
end

function game_object:set_target(target)
	self.target = target
	self.target_path_timer = game_object_target_path_interval
	self:set_goal(target.pos, self:get_targeting_range(), self:get_targeting_pitch_range())
	self:rotate_parts(target.pos)
end
function game_object:clear_target()
	self.target = nil
	self:clear_goal()
	self:reset_parts_rotation()
end

function game_object:targeting_tick(time_delta)
	if not self.target then
		return
	end
	if not self.target:exists() then
		self.target = nil
		return
	end

	local target = self.target

	self:rotate_parts(target.pos)
	if self.target_path_timer == nil then
		self.target_path_timer = game_object_target_path_interval
	end
	if self.target_path_timer > 0 then
		self.target_path_timer = self.target_path_timer - time_delta
	else
		self.target_path_timer = game_object_target_path_interval
		self:set_goal(target.pos, self:get_targeting_range(), self:get_targeting_pitch_range())
	end
end
