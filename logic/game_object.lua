game_object = {
	pos_x = 0, pos_y = 0, pos_z = 0,
	angle = 0, pitch = 0,
	size_x = y, size_y = 0, size_z = 0
}

function game_object:create_tables(o)
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
		v:render()
	end
end
