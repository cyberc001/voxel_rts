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
