-- function notations:
-- i - inline (result is written to 1st operand)
-- e - element-wise (multiplication/division)

-- vec2
vec2 = { x = 0, y = 0 }

function vec2:new(x, y)
	local o = {}
	if type(x) == "table" then
		o = {x = x.x, y = x.y}
	else -- type(x, y) == "number"
		o = {x = x or 0, y = y or 0}
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec2.tostring(v1)
	return "{" .. tostring(v1.x) .. ", " .. tostring(v1.y) .. "}"
end
vec2.__tostring = vec2.tostring

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

function vec2:emul(v1, v2)
	v = vec2:new()
	v.x = v1.x * v2.x
	v.y = v1.y * v2.y
	return v
end
-- vec3
vec3 = { x = 0, y = 0, z = 0 }

function vec3:new(x, y, z)
	local o = {}
	if type(x) == "table" then
		o = {x = x.x, y = x.y, z = x.z}
	else -- type(x, y, z) == "number"
		o = {x = x or 0, y = y or 0, z = z or 0}
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec3.tostring(v1)
	return "{" .. tostring(v1.x) .. ", " .. tostring(v1.y) .. ", " .. tostring(v1.z) .. "}"
end
vec3.__tostring = vec3.tostring

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
	elseif type(v2) == "number" then return vec3:new(v1.x * v2, v1.y * v2, v1.z * v2)
	else return vec3:new(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z) end
end
vec3.__mul = vec3.mul
function vec3.unm(v1)
	return v1 * -1
end
vec3.__unm = vec3.unm
function vec3.div(v1, v2)
	if type(v1) == "number" then return vec3:new(v2.x / v1, v2.y / v1, v2.z / v1)
	elseif type(v2) == "number" then return vec3:new(v1.x / v2, v1.y / v2, v1.z / v2) end
end
vec3.__div = vec3.div

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
function vec3:emul(v1, v2)
	return vec3:new(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z)
end
function vec3:dot(v2)
	return self.x*v2.x + self.y*v2.y + self.z*v2.z
end

function vec3:cross(v2)
	return vec3:new(self.y*v2.z - self.z*v2.y, self.z*v2.x - self.x*v2.z, self.x*v2.y - self.y*v2.x)
end

function vec3:ln()
	return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)
end
function vec3:unit()
	return self * (1/self:ln())
end
function vec3:safe_unit()
	return self:ln() > 0 and self:unit() or vec3:new(0, 0, 0)
end

-- vec4 (quaternion)
vec4 = { x = 0, y = 0, z = 0 }

function vec4:new(x, y, z, w)
	local o = {}
	if type(x) == "table" then
		o = {x = x.x, y = x.y, z = x.z, w = x.w}
	else -- type(x, y, z, w) == "number"
		o = {x = x or 0, y = y or 0, z = z or 0, w = w or 0}
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec4.tostring(v1)
	return "{" .. tostring(v1.x) .. ", " .. tostring(v1.y) .. ", " .. tostring(v1.z) .. ", " .. tostring(v1.w) .. "}"
end
vec4.__tostring = vec4.tostring

function vec4.add(v1, v2)
	return vec4:new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w)
end
vec4.__add = vec4.add
function vec4.sub(v1, v2)
	return vec4:new(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w)
end
vec4.__sub = vec4.sub

function vec4.mul(v1, v2)
	if type(v1) == "number" then return vec4:new(v2.x * v1, v2.y * v1, v2.z * v1, v2.w * v1)
	elseif type(v2) == "number" then return vec4:new(v1.x * v2, v1.y * v2, v1.z * v2, v1.w * v2)
	elseif type(v1) == "table" and type(v2) == "table" then -- the result is both quaternions applied in order
		return vec4:new(v1.w * v2.x + v1.x * v2.w + v1.y * v2.z - v1.z * v2.y,
				v1.w * v2.y + v1.y * v2.w + v1.z * v2.x - v1.x * v2.z,
				v1.w * v2.z + v1.z * v2.w + v1.x * v2.y - v1.y * v2.x,
				v1.w * v2.w - v1.x * v2.x - v1.y * v2.y - v1.z * v2.z)
	end
end
vec4.__mul = vec4.mul

function vec4:ln()
	return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z + self.w*self.w)
end
function vec4:unit()
	return self * (1/self:ln())
end

function vec4:conj()
	return vec4:new(-self.x, -self.y, -self.z, self.w)
end
function vec4:inv()
	-- https://proofwiki.org/wiki/Multiplicative_Inverse_of_Quaternion
	local lambda = 1 / (self.x*self.x + self.y*self.y + self.z*self.z + self.w*self.w)
	lambda = vec4:new(0, 0, 0, 1) * lambda
	return lambda * self:conj()
end

function vec4:ang()
	return 2 * math.acos(self.w)
end

function vec4:exp()
	local ea = math.exp(self.w)
	local v = vec3:new(self.x, self.y, self.z)
	local v_ln = v:ln()
	v = v:unit() * math.sin(v_ln)
	return vec4:new(v.x, v.y, v.z, ea * math.cos(v_ln))
end
function vec4:log() -- natural logarithm
	local ln = self:ln()
	local v = vec3:new(self.x, self.y, self.z):unit() * math.acos(self.w / ln)
	return vec4:new(v.x, v.y, v.z, math.log(ln))
end
