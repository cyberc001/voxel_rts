require "./logic/math/vec"

game_object = {}

require "./logic/game_object/pathing"
require "./logic/game_object/targeting"

function game_object:create_tables(o)
	o.pos = o.pos or vec3:new()

	o.vel = vec3:new()
	o.path_vel = vec3:new()

	o.rot = o.rot or vec4:new(0, 0, 0, 1)
	o.rot_goal = o.rot or vec4:new(0, 0, 0, 1)
	o.path_forward = o.path_forward or vec3:new(1, 0, 0)

	o.size = o.size or vec3:new(1, 1, 1)
	o.base_hitbox = o.base_hitbox or gmath.hexahedron_from_cube(1)
	o.robj_arr = o.robj_arr or {}
end
function game_object:new(o)
	o = o or {}
	o.speed = o.speed or 0.01
	o.team = o.team or 1

	game_object:create_tables(o)
	setmetatable(o, self)
	self.__index = self
	o:update_hitbox()
	return o
end

function game_object:update_hitbox()
	self.hitbox = gmath.hexahedron_transform(self.base_hitbox, self.pos, self.rot, self.size)
	self.robj_hitbox = render.render_hexahedron(self.hitbox, player_selected_objects[self] == true and vec3:new(1, 0, 0) or nil)
	self.bbox = gmath.hexahedron_get_bbox(self.hitbox)
	self.interaction_box = gmath.hexahedron_get_interaction_box(self.hitbox)
end

-- callbacks for the engine

function game_object:tick()
	self:path_tick()
	self:targeting_tick()
end

function game_object:render()
	if render_hitboxes then
		render.render_obj_draw(self.robj_hitbox)
	end

	for _,v in ipairs(self.robj_arr) do
		v:render(self)
	end
end
