local game_object_target_path_interval = 350

function game_object:get_target_range()
	return 2
end

function game_object:set_target(target)
	self.target = target
	self.target_path_timer = game_object_target_path_interval
	self:set_goal(target.pos, self:get_target_range())
end
function game_object:clear_target()
	self.target = nil
	self:clear_goal()
end

function game_object:targeting_tick()
	if not self.target then
		return
	end

	if self.target_path_timer == nil then
		self.target_path_timer = game_object_target_path_interval
	end
	if self.target_path_timer > 0 then
		self.target_path_timer = self.target_path_timer - 1
	else
		self.target_path_timer = game_object_target_path_interval
		self:set_goal(self.target.pos, self:get_target_range())
	end

end
