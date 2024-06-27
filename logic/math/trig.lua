require "./logic/math/vec"

function rad_to_ang(r)
	return r * 180 / math.pi
end
function ang_to_rad(a)
	return a * math.pi / 180
end

function ang_between(v1, v2)
	return math.acos(v1:dot(v2) / v1:ln() / v2:ln())
end

function rot_vec2(v, rad)
	return vec2:new(v.x*math.cos(rad) - v.y*math.sin(rad),
			v.x*math.sin(rad) + v.y*math.cos(rad))
end

function ang_norm180(a) -- normalize in range [-180; 180]
	a = a % 360
	if a > 180 then a = a - 360 end
	return a
end
function rad_norm_pi(a) -- normalize in range [-pi; pi]
	a = a % (math.pi * 2)
	if a > math.pi then a = a - math.pi end
	return a
end
