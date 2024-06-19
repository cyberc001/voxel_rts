require "./logic/weapon"
require "./logic/weapons/projectile"

cannon = weapon:new({})

function cannon:get_launch_offset()
	return 2
end
function cannon:get_launch_force()
	return 0.1
end
function cannon:get_launch_vel()
	return self:get_launch_force() / self.proj.mass
end

function cannon:rotate_to(target)
	-- correct for gravity
	local dir = vec3:new(gmath.vec3_quat_rot(vec3:new(1, 0, 0), self:get_rot()))
	local diff = (self.gobj.pos + self:get_launch_offset() * dir) - target

	local travel_time = diff:ln() / self:get_launch_vel()
	local gravity_off = gravity_accel * travel_time*travel_time / 2

	weapon.rotate_to(self, target - gravity_off)
end
function cannon:fire(target)
	local dir = vec3:new(gmath.vec3_quat_rot(vec3:new(1, 0, 0), self:get_rot()))
	local created_proj = self.proj.class:new(self.proj)
	created_proj.pos = self.gobj.pos + self:get_launch_offset() * dir
	created_proj.rot = self.gobj.rot
	created_proj.vel = self:get_launch_vel() * dir
	created_proj.team = self.gobj.team

	game_object_arr[created_proj] = true
end
