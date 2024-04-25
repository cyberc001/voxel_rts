octree = {}

function octree:new(objects)
	local o = {}

	o.depths = {0}
	o.parents = {0}
	o.obj_clusters = {} -- maps objects to their clusters
	o.obj_clusters.__mode = "k"

	local mincr = vec3:new(math.huge, math.huge, math.huge)
	local maxcr = -mincr
	for _,v in ipairs(objects) do
		if v.bbox[1].x > maxcr.x then maxcr.x = v.bbox[1].x end
		if v.bbox[1].x < mincr.x then mincr.x = v.bbox[1].x end
		if v.bbox[1].y > maxcr.y then maxcr.y = v.bbox[1].y end
		if v.bbox[1].y < mincr.y then mincr.y = v.bbox[1].y end
		if v.bbox[1].z > maxcr.z then maxcr.z = v.bbox[1].z end
		if v.bbox[1].z < mincr.z then mincr.z = v.bbox[1].z end

		o.obj_clusters[v] = 1
	end

	o.min_corners = {mincr}
	o.max_corners = {maxcr}

	setmetatable(o, self)
	self.__index = self
	return o
end

function octree:divide(cluster_i)
	if not self.clusters[cluster_i] or #self.clusters[cluster_i] == 0 then
		return -- no points in current cluster, terminate
	end

	local mincr, maxcr = o.min_corners[cluster_i], o.max_corners[cluster_i]
	local center = vec3:new((mincr.x + maxcr.x) / 2,
				(mincr.y + maxcr.y) / 2,
				(mincr.z + maxcr.z) / 2)
	for i = 1, 8 do
		local corner = vec3:new()
		corner.x = (bit.band(i, 1) == 0 and mincr.x or maxcr.x)
		corner.y = (bit.band(i, 2) == 0 and mincr.y or maxcr.y)
		corner.z = (bit.band(i, 3) == 0 and mincr.z or maxcr.z)
		print("corner: ", corner)
	end
end
