require "./logic/math/basic"

render_object = {
	model = nil
}

function render_object:create_tables(o)
	o.pos = o.pos or vec3:new()
	o.rot = o.rot or vec4:new(0, 0, 0, 1)
	o.size = o.size or vec3:new(1, 1, 1)
end
function render_object:new(o, model_name)
	if model_name then
		o.model = render.model_find(model_name)
		o.model_data = render.model_data_find(model_name)
	end
	o = o or {}
	render_object:create_tables(o)
	setmetatable(o, self)
	self.__index = self
	return o
end

function render_object:render(gobj)
	local pos = gobj.body.pos + gmath.vec3_quat_rot(self.pos, gobj.body.rot)
	local size = vec3:emul(self.size, gobj.body.scale)
	render.render_obj_draw(self.model, pos, gobj.body.rot * self.rot, size)
end


function render_object:create_debris(min_depth, max_depth, min_size)
	min_depth = min_depth or 2
	max_depth = max_depth or 6
	self.__min_debris_size = min_size or vec3:new(0.5, 0.5, 0.5)

	self.__tmp_debris_list = {}
	self:_r_create_debris(vec3:new(self.model_data.origin), vec3:new(self.model_data.size), random_range(min_depth, max_depth))
	return self.__tmp_debris_list
end
function render_object:_add_debris(origin, size)
	local d = render.render_obj_shallow_copy(self.model)
	render.render_obj_set_cut(d, origin, origin + size)
	table.insert(self.__tmp_debris_list, {
		model = d,
		pos = origin,
		size = size
	})
end

function render_object:_r_create_debris(origin, size, depth_left)
	if depth_left <= 0 then
		self:_add_debris(origin, size)
		return
	end

	local cut_dir = math.random()
	if cut_dir <= 0.33 then cut_dir = "x"
	elseif cut_dir <= 0.66 then cut_dir = "y"
	else cut_dir = "z" end

	if cut_dir == "x" and size.x <= self.__min_debris_size.x then
		if size.y >= self.__min_debris_size.y then cut_dir = "y"
		elseif size.z >= self.__min_debris_size.z then cut_dir = "z"
		else self:_add_debris(origin, size)
			return
		end
	end
	if cut_dir == "y" and size.y <= self.__min_debris_size.y then
		if size.x >= self.__min_debris_size.x then cut_dir = "x"
		elseif size.z >= self.__min_debris_size.z then cut_dir = "z"
		else self:_add_debris(origin, size)
			return
		end
	end
	if cut_dir == "z" and size.z <= self.__min_debris_size.z then
		if size.x >= self.__min_debris_size.x then cut_dir = "x"
		elseif size.y >= self.__min_debris_size.y then cut_dir = "y"
		else self:_add_debris(origin, size)
			return
		end
	end

	local amt = random_range(0.25, 0.75)

	local sz1 = vec3:new(size)
	sz1[cut_dir] = sz1[cut_dir] * amt
	self:_r_create_debris(vec3:new(origin), sz1, depth_left - 1)

	local sz2 = vec3:new(size)
	sz2[cut_dir] = sz2[cut_dir] * (1 - amt)
	local pt2 = vec3:new(origin)
	pt2[cut_dir] = pt2[cut_dir] + size[cut_dir] * amt
	self:_r_create_debris(pt2, sz2, depth_left - 1)
end
