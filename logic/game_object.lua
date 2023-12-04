require "./logic/math/vec"

game_object = {}

function game_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.rot = o.rot or vec2:new()
	o.size = o.size or vec3:new()
	o.robj_arr = o.robj_arr or {}
end
function game_object:new(o)
	o = o or {}
	game_object:create_tables(o)
	setmetatable(o, self)
	self.__index = self
	return o
end

-- callbacks for the engine

function game_object:tick()
end

function game_object:render()
	for _,v in ipairs(self.robj_arr) do
		v:render(self)
	end
end
