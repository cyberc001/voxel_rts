-- function notations:
-- i - inline (result is written to 1st operand)

-- vec2
vec2 = { x = 0, y = 0 }

function vec2:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec2:iadd(v1, v2)
	v1.x = v1.x + v2.x
	v1.y = v1.y + v2.y
end
function vec2:isub(v1, v2)
	v1.x = v1.x - v2.x
	v1.y = v1.y - v2.y
end

-- vec3
vec3 = { x = 0, y = 0, z = 0 }

function vec3:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec3:iadd(v1, v2)
	v1.x = v1.x + v2.x
	v1.y = v1.y + v2.y
	v1.z = v1.z + v2.z
end

function vec3:isub(v1, v2)
	v1.x = v1.x - v2.x
	v1.y = v1.y - v2.y
	v1.z = v1.z - v2.z
end
