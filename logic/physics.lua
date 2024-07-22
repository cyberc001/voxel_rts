gravity_accel = vec3:new(0, -20, 0)
local elasticity = 0.3
local static_fric_coff = 0.4
local kinetic_fric_coff = 0.4

local static_mass = 1000000

function get_resolution_unit(resolution)
	return resolution:ln() > 0 and resolution:unit() or vec3:new(0, 1, 0)
end

-- Impulse-based collision response
-- https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics6collisionresponse/2017%20Tutorial%206%20-%20Collision%20Response.pdf

function static_collision_response(resolution, gobj)
	local resolution_unit = get_resolution_unit(resolution)
	local vel_n = -elasticity * gobj.vel:dot(resolution_unit)
	local impulse = (-(1 + elasticity) * gobj.vel:dot(resolution_unit)) / (resolution_unit:dot(resolution_unit*(1/gobj.mass + 1/static_mass)))
	if impulse ~= impulse then impulse = 0 end
	gobj.vel = gobj.vel + impulse / gobj.mass * resolution_unit
end

function collision_response(resolution, gobj1, gobj2)
	local resolution_unit = get_resolution_unit(resolution)
	local vel_diff = gobj1.vel - gobj2.vel
	local vel_n = -elasticity * vel_diff:dot(resolution_unit)
	local impulse = (-(1 + elasticity) * vel_diff:dot(resolution_unit)) / (resolution_unit:dot(resolution_unit*(1/gobj1.mass + 1/gobj2.mass)))
	if impulse ~= impulse then impulse = 0 end
	gobj1.vel = gobj1.vel + impulse / gobj1.mass * resolution_unit
	gobj2.vel = gobj2.vel + impulse / gobj2.mass * resolution_unit
end

function friction_deceleration(resolution, gobj)
	local resolution_unit = get_resolution_unit(resolution)
	local fric_n = gravity_accel:safe_unit():dot(resolution_unit)
	local normal_force = fric_n * gobj.mass
	local fric_static = normal_force * static_fric_coff

	local coff = gobj.force:ln() < fric_static and static_fric_coff or kinetic_fric_coff
	local decel = fric_n * gravity_accel:safe_unit():dot(resolution_unit) * coff

	local ln = gobj.vel:ln() > decel and gobj.vel:ln() - decel or 0
	gobj.vel = gobj.vel:safe_unit() * ln

	-- assume r = 1 for now
	decel = decel * gobj.rot_fric_mul
	ln = gobj.rot_vel:ln() > decel and gobj.rot_vel:ln() - decel or 0
	gobj.rot_vel = gobj.rot_vel:safe_unit() * ln
end
