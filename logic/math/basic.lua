function sign(x)
	return x < 0 and -1 or 1
end

function random_range(_min, _max)
	return math.random() * (_max - _min) + _min
end
