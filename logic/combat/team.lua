all_teams = {}

team = {}

function team:new(o)
	o = o or {}
	o.name = o.name or "unknown"
	o.allies = {}

	setmetatable(o, self)
	self.__index = self
	return o
end

function team:add_ally(ally)
	self.allies[ally] = true
	ally.allies[self] = true
end
function team:is_hostile_to(other)
	return not self.allies[other]
end
