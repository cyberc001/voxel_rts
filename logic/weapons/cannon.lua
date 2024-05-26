require "./logic/weapon"

cannon = weapon:new({})

function cannon:get_proj_offset()
	return 2
end
function cannon:get_proj_vel()
	return 10
end

function cannon:rotate_to(target)
	-- correct for gravity
	local dir = vec3:new(gmath.vec3_quat_rot(vec3:new(1, 0, 0), self:get_rot()))
	local diff = (self.gobj.pos + self:get_proj_offset() * dir) - target

	local travel_time = diff:ln() / self:get_proj_vel()
	local gravity_off = gravity_accel * travel_time*travel_time / 2

	weapon.rotate_to(self, target - gravity_off)
end
function cannon:fire(target)
	local dir = vec3:new(gmath.vec3_quat_rot(vec3:new(1, 0, 0), self:get_rot()))
	table.insert(game_object_arr, game_object:new({
		pos = self.gobj.pos + self:get_proj_offset() * dir,
		rot = self.gobj.rot,
		vel = self:get_proj_vel() * dir,

		mass = 0.01,
		base_hitbox = gmath.hexahedron_from_cuboid_centered(0.1, 0.1, 0.1),
		team = self.gobj.team,
		robj_arr = {
			render_object:new({model = render.model_find("test_projectile")})
		}
	}))
	print("FIRE!!!")
end
