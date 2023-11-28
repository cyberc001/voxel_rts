render_object = {
	pos_x = 0, pos_y = 0, pos_z = 0,
	angle = 0, pitch = 0,
	scale_x = 1, scale_y = 1, scale_z = 1,
	model = nil
}

function render_object:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function render_object:render(gobj)
	game.render_obj_draw(self.model, self.pos_x, self.pos_y, self.pos_z, self.angle, self.pitch, self.scale_x, self.scale_y, self.scale_z)
end
