render_object = {
	model = nil
}

function render_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.rot = o.rot or vec4:new(0, 0, 0, 1)
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
	local pos = gobj.pos + gmath.vec3_quat_rot(self.pos, gobj.rot)
	local size = vec3:emul(self.size, gobj.size)
	render.render_obj_draw(self.model, pos, self.rot * gobj.rot, size)
end
