require "./logic/math/bbox"

octree = {}

function octree:new(objects)
	local o = {}
	setmetatable(o, self)
	self.__index = self

	o.depths = {0}
	o.obj_clusters = {} -- maps object to a set of clusters
	o.clusters = {{}} -- maps cluster to a set of objects

	local mincr = vec3:new(math.huge, math.huge, math.huge)
	local maxcr = -mincr
	for _,v in ipairs(objects) do
		if v.bbox[1].x < mincr.x then mincr.x = v.bbox[1].x end
		if v.bbox[1].y < mincr.y then mincr.y = v.bbox[1].y end
		if v.bbox[1].z < mincr.z then mincr.z = v.bbox[1].z end
		if v.bbox[2].x > maxcr.x then maxcr.x = v.bbox[2].x end
		if v.bbox[2].y > maxcr.y then maxcr.y = v.bbox[2].y end
		if v.bbox[2].z > maxcr.z then maxcr.z = v.bbox[2].z end

		o:add_object_to_cluster(v, 1)
	end

	o.bboxes = {bbox:new(mincr, maxcr)}

	o:divide(1)
	return o
end

function octree:add_object_to_cluster(obj, cluster_i, old_cluster_i)
	self.clusters[cluster_i][obj] = true
	if self.obj_clusters[obj] == nil then
		self.obj_clusters[obj] = {}
	end
	self.obj_clusters[obj][cluster_i] = true
	if old_cluster_i ~= nil then
		self.obj_clusters[obj][old_cluster_i] = nil
	end
end

function octree:divide(cluster_i)
	local mincr, maxcr = self.bboxes[cluster_i][1], self.bboxes[cluster_i][2] 
	local center = (mincr + maxcr) * 0.5

	if (maxcr - mincr):ln() < 2 then
		return false -- cluster is too small diagonally, terminate
	end

	local had_child_clusters = false
	for i = 1, 8 do
		local corner = vec3:new()
		corner.x = (i & 1 == 0 and mincr.x or maxcr.x)
		corner.y = (i & 2 == 0 and mincr.y or maxcr.y)
		corner.z = (i & 4 == 0 and mincr.z or maxcr.z)

		local new_cluster_i = #self.clusters + 1
		self.clusters[new_cluster_i] = {}
		self.bboxes[new_cluster_i] = bbox:new()

		self.bboxes[new_cluster_i][1].x = math.min(corner.x, center.x)
		self.bboxes[new_cluster_i][1].y = math.min(corner.y, center.y)
		self.bboxes[new_cluster_i][1].z = math.min(corner.z, center.z)
		self.bboxes[new_cluster_i][2].x = math.max(corner.x, center.x)
		self.bboxes[new_cluster_i][2].y = math.max(corner.y, center.y)
		self.bboxes[new_cluster_i][2].z = math.max(corner.z, center.z)

		for k in pairs(self.clusters[cluster_i]) do
			if gmath.bbox_check_collision(self.bboxes[new_cluster_i], k.bbox) then
				self:add_object_to_cluster(k, new_cluster_i, cluster_i)
			end
		end

		if next(self.clusters[new_cluster_i]) == nil then
			-- this cluster doesn't intersect with any objects; don't go deeper
			self.clusters[new_cluster_i] = nil
			self.bboxes[new_cluster_i] = nil
		else
			self.depths[new_cluster_i] = self.depths[cluster_i] + 1
			self:divide(new_cluster_i)
			had_child_clusters = true
		end
	end
	if had_child_clusters then -- leave only leafs in the tree
		self.clusters[cluster_i] = nil
		self.bboxes[cluster_i] = nil
		self.depths[cluster_i] = nil
	end
	return true
end
