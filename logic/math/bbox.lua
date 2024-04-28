bbox = { vec3:new(), vec3:new() }

function bbox:new(min, max)
	local o = {}
	o[1] = min or vec3:new()
	o[2] = max or vec3:new()
	setmetatable(o, self)
	self.__index = self
	return o
end
function bbox.tostring(v1)
	return "[ min: " .. vec3.tostring(v1[1]) .. " max: " .. vec3.tostring(v1[2]) .. " ]"
end
bbox.__tostring = bbox.tostring
