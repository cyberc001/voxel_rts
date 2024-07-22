require "./logic/combat/shootable"

projectile = shootable:new({only_receive_collision = true})
projectile.class = projectile

function projectile:get_impact_damage()
	return 34
end

function projectile:on_object_collision(other, resolution)
	if other == self.shooter then
		return false
	end

	game_object.on_object_collision(self, other, resolution)

	if other.take_damage then -- if it's shootable
		other:take_damage(self, self:get_impact_damage())
		self:destroy()
	end

	return true
end
