-- mat4
mat4 = {}

function mat4:new()
	o = {}
	for i = 1, 16 do
		o[i] = 0
	end
	o[1] = 1
	o[6] = 1
	o[11] = 1
	o[16] = 1
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