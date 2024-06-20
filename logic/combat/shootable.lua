shootable = game_object:new({
	health = 100
})

function shootable:take_damage(inflictor, dmg)
	self.health = self.health - dmg
	if self.health <= 0 then
		self:on_death(inflictor)
	end
end
function shootable:on_death(killer)
	self:destroy()
end
