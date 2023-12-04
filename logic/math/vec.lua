-- function notations:
-- i - inline (result is written to 1st operand)

-- vec2
vec2 = { x = 0, y = 0 }

function vec2:new(x, y)
	o = {}
	if type(x) == "table" then
		o = x
	else -- type(x, y) == "number"
		o = {x = x or 0, y = y or 0}
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec2.add(v1, v2)
	return vec2:new(v1.x + v2.x, v1.y + v2.y)
end
vec2.__add = vec2.add
function vec2.sub(v1, v2)
	return vec2:new(v1.x - v2.x, v1.y - v2.y)
end
vec2.__sub = vec2.sub
function vec2.mul(v1, v2)
	if type(v1) == "number" then return vec2:new(v2.x * v1, v2.y * v1)
	elseif type(v2) == "number" then return vec2:new(v1.x * v2, v1.y * v2) end
end
vec2.__mul = vec2.mul
function vec2.unm(v1)
	return v1 * -1
end
vec2.__unm = vec2.unm

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

function vec3:new(x, y, z)
	if type(x) == "table" then
		o = x
	else -- type(x, y, z) == "number"
		o = {x = x or 0, y = y or 0, z = z or 0}
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec3.add(v1, v2)
	return vec3:new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z)
end
vec3.__add = vec3.add
function vec3.sub(v1, v2)
	return vec3:new(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z)
end
vec3.__sub = vec3.sub
function vec3.mul(v1, v2)
	if type(v1) == "number" then return vec3:new(v2.x * v1, v2.y * v1, v2.z * v1)
	elseif type(v2) == "number" then return vec3:new(v1.x * v2, v1.y * v2, v1.z * v2) end
end
vec3.__mul = vec3.mul
function vec3.unm(v1)
	return v1 * -1
end
vec3.__unm = vec3.unm

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
