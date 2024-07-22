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

function shootable:create_debris(min_depth, max_depth, min_size,
					min_vel, max_vel,
					min_rotvel, max_rotvel)
	min_vel = min_vel or 3
	max_vel = max_vel or 12
	min_rotvel = min_rotvel or 5
	max_rotvel = max_rotvel or 25

	local center = gmath.hexahedron_get_center(self.hitbox)
	for _,v in ipairs(self.robj_arr) do
		local debri_arr = v:create_debris()
		for _, d in pairs(debri_arr) do
			local model_off = (vec3:new(v.model_data.size) - d.size) * 0.5 - (d.pos - vec3:new(v.model_data.origin))
			local _rot = self.rot * v.rot
			local _pos = self.pos + v.pos + gmath.vec3_quat_rot(d.pos + d.size*0.5, _rot) * self.size * v.size

			local _o = game_object:new({
				only_terrain_collision = true,
				rot_fric_mul = 8,

				pos = _pos,
				rot = _rot,
				size = self.size * v.size,
				base_hitbox = gmath.hexahedron_from_cuboid_centered(d.size.x, d.size.y, d.size.z),
				robj_arr = {
					render_object:new({model = d.model,
					pos = model_off * self.size * v.size
				})
				}
			})

			local pos_diff = (_pos - center):safe_unit()
			_o.vel = pos_diff * random_range(min_vel, max_vel)
			_o.rot_vel = pos_diff:cross(gmath.vec3_quat_rot(vec3:new(0, 1, 0), _rot)):safe_unit() * random_range(min_rotvel, max_rotvel)
			game_object_arr[_o] = true
		end
	end
end
