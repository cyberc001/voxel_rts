-- mat4
mat4 = {}

function mat4:new(fill)
	o = {}
	for i = 1, 16 do
		o[i] = type(fill) == "number" and fill or 0
	end
	if fill == nil then
		o[1] = 1
		o[6] = 1
		o[11] = 1
		o[16] = 1
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function mat4.tostring(m)
	local str = ""
	for i = 1, 16 do
		str = str .. tostring(m[i]) .. " "
		if math.fmod(i, 4) == 0 then
			str = str .. "\n"
		end
	end
	return str
end
mat4.__tostring = mat4.tostring

function mat4.mul(m1, m2)
	local m = mat4:new(0)
	for i = 0, 3 do
		for j = 0, 3 do
			for k = 0, 3 do
				m[i*4 + j + 1] = m[i*4 + j + 1] + m1[i*4 + k + 1] * m2[k*4 + j + 1]
			end
		end
	end
	return m
end
mat4.__mul = mat4.mul
