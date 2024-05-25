require "./logic/part"

weapon = part:new({})

function weapon:new(o)
	o = part:new(o)
	o.attack_timer = 0

	setmetatable(o, self)
	self.__index = self
	return o
end

function weapon:get_attack_interval()
	return 1
end

function weapon:tick(time_delta)
	part.tick(self, time_delta)

	if self.attack_timer < self:get_attack_interval() then
		self.attack_timer = self.attack_timer + time_delta
	elseif self.gobj.target then
		self.attack_timer = 0
		self:fire(self.gobj.target)
	end
end
function weapon:fire(target)
end
