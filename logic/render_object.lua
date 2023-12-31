render_object = {
	model = nil
}

function render_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.rot = o.rot or vec2:new()
	o.size = o.size or vec3:new(1, 1, 1)
end
function render_object:new(o)
	o = o or {}
	render_object:create_tables(o)
	setmetatable(o, self)
	self.__index = self
	return o
end

function render_object:render(gobj)
	vec3:iadd(self.pos, gobj.pos)
	vec2:iadd(self.rot, gobj.rot)
	local size = vec3:emul(self.size, gobj.size)
	render.render_obj_draw(self.model, self.pos, self.rot, size)
	vec3:isub(self.pos, gobj.pos)
	vec2:isub(self.rot, gobj.rot)
end
