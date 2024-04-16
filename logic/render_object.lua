render_object = {
	model = nil
}

function render_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.trmat = o.trmat or mat4:new()
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
	local size = vec3:emul(self.size, gobj.size)
	render.render_obj_draw(self.model, self.pos, self.trmat * gobj.trmat, size)
	vec3:isub(self.pos, gobj.pos)
end
