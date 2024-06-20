require "./logic/combat/weapon"
require "./logic/combat/projectile"

cannon = weapon:new({})

function cannon:get_launch_offset()
	return 2
end
function cannon:get_launch_force()
	return 0.1
end
function cannon:get_launch_vel()
	if not self.impending_proj then
		self.impending_proj = self.proj_factory()
	end
	return self:get_launch_force() / self.impending_proj.mass
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
	if not self.impending_proj then
		self.impending_proj = self.proj_factory()
	end

	local dir = vec3:new(gmath.vec3_quat_rot(vec3:new(1, 0, 0), self:get_rot()))
	self.impending_proj.pos = self.gobj.pos + self:get_launch_offset() * dir
	self.impending_proj.rot = self.gobj.rot
	self.impending_proj.vel = self:get_launch_vel() * dir
	self.impending_proj.team = self.gobj.team

	game_object_arr[self.impending_proj] = true
	self.impending_proj = nil
end
