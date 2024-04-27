bbox = { vec3:new(), vec3:new() }

function bbox:new(min, max)
	local o = {}
	o[1] = min or vec3:new()
	o[2] = max or vec3:new()
	setmetatable(o, self)
	self.__index = self
	return o
end
