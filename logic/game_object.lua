require "./logic/math/vec"

game_object = {}

require "./logic/game_object/pathing"
require "./logic/game_object/targeting"
require "./logic/game_object/rotation"
require "./logic/part"

function game_object:create_tables(o)
	o.mass = o.mass or 1
	o.force = vec3:new()

	o.vel = vec3:new()
	o.rot_vel = vec3:new() -- in axis angle

	o.path_vel = vec3:new()
	o.path_forward = vec3:new(1, 0, 0)
	o.resolution_rot_vel = vec3:new()

	o.parts = o.parts or {}
	o.robj_arr = o.robj_arr or {}
end
function game_object:new(o)
	o = o or {}

	o.max_speed = o.speed or 2

	-- physics parameters
	o.rot_fric_mul = o.rot_fric_mul or 1

	o.max_resolution_rot_vel = o.max_resolution_rot_vel or 7
	o.resolution_rot_accel = o.resolution_rot_accel or 3

	-- o.only_terrain_collision - only collide with terrain, not added to octree
	-- o.only_receive_collision - only collide (o, other) and not (other, o)

	o.team = o.team or all_teams[0]

	game_object:create_tables(o)
	if o.body ~= nil then
		self:set_body(o.body)
	end

	setmetatable(o, self)
	self.__index = self
	return o
end

function game_object:destroy()
	game_object_arr[self] = nil
end
function game_object:exists()
	return game_object_arr[self] ~= nil
end

function game_object:set_body(body)
	self.body = body
	self.body_robj = body:render()
	self.body_robj_sel = body:render(vec3:new(1, 0, 0))
end
function game_object:apply_force(force)
	self.force = self.force + force
end

-- event callbacks

function game_object:tick(time_delta)
	self:path_tick()
	self:targeting_tick(time_delta)
	self:tick_parts(time_delta)
end

function game_object:render()
	if render_hitboxes and self.body_robj then
		render.render_obj_draw(player_selected_objects[self] and self.body_robj_sel or self.body_robj,
					self.body.pos, self.body.rot, self.body.scale)
	end

	for _,v in ipairs(self.robj_arr) do
		v:render(self)
	end
end


function game_object:on_object_collision(other, resolution)
	return true
end
