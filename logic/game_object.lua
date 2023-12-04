require "./logic/math/vec"

game_object = {}

function game_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.rot = o.rot or vec2:new()
	o.size = o.size or vec3:new()
	o.hitbox = o.hitbox or math.hexahedron_from_cube(0)
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
	if render_hitboxes then
		if not self.robj_hitbox then
			self.robj_hitbox = render.render_hexahedron(self.hitbox)
		end
		render.render_obj_draw(self.robj_hitbox, self.pos)
	end

	for _,v in ipairs(self.robj_arr) do
		v:render(self)
	end
end
