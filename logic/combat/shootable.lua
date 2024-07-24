shootable = game_object:new({
	health = 100
})

function shootable:take_damage(inflictor, dmg)
	self.health = self.health - dmg
	if self.health <= 0 then
		self:on_death(inflictor)
	end
end
function shootable:on_death(killer)
	self:destroy()
	self:create_debris()
end

debris = game_object:new({})

function debris:tick(time_delta)
	-- omit game_object tick to save time
	self.lifetime = self.lifetime - time_delta
	if self.lifetime <= 0 then
		self:destroy()
	end
end

function shootable:create_debris(min_lifetime, max_lifetime,
					min_depth, max_depth, min_size,
					min_vel, max_vel,
					min_rotvel, max_rotvel)
	min_lifetime = min_lifetime or 5
	max_lifetime = max_lifetime or 20
	min_vel = min_vel or 3
	max_vel = max_vel or 12
	min_rotvel = min_rotvel or 5
	max_rotvel = max_rotvel or 25

	local center = self.body:get_center()
	for _,v in ipairs(self.robj_arr) do
		local debri_arr = v:create_debris()
		for _, d in pairs(debri_arr) do
			local model_off = (vec3:new(v.model_data.size) - d.size) * 0.5 - (d.pos - vec3:new(v.model_data.origin))

			local _rot = self.body.rot * v.rot
			local _pos = self.body.pos + v.pos + gmath.vec3_quat_rot(d.pos + d.size*0.5, _rot) * self.body.scale * v.size

			local _o = debris:new({
				lifetime = random_range(min_lifetime, max_lifetime),

				only_terrain_collision = true,
				rot_fric_mul = 8,

				size = self.body.scale * v.size,
				body = body.hexahedron_from_cuboid_centered(d.size.x, d.size.y, d.size.z),
				robj_arr = {
					render_object:new({model = d.model,
					pos = model_off * self.body.scale * v.size
				})
				}
			})

			_o.body.pos = _pos
			_o.body.rot = _rot

			local pos_diff = (_pos - center):safe_unit()
			_o.vel = pos_diff * random_range(min_vel, max_vel)
			_o.rot_vel = pos_diff:cross(gmath.vec3_quat_rot(vec3:new(0, 1, 0), _rot)):safe_unit() * random_range(min_rotvel, max_rotvel)
			game_object_arr[_o] = true
		end
	end
end
