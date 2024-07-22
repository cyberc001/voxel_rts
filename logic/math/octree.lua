require "./logic/math/bbox"

octree = {}

function octree:new(objects)
	local o = {}
	setmetatable(o, self)
	self.__index = self

	o.obj_clusters = {} -- maps object to a set of clusters
	o.clusters = {{}} -- maps cluster to a set of objects
	o.children = {}
	o.depth = {}

	local mincr = vec3:new(math.huge, math.huge, math.huge)
	local maxcr = -mincr
	for v,_ in pairs(objects) do
		if not v.only_terrain_collision then
			if v.interaction_box[1].x < mincr.x then mincr.x = v.interaction_box[1].x end
			if v.interaction_box[1].y < mincr.y then mincr.y = v.interaction_box[1].y end
			if v.interaction_box[1].z < mincr.z then mincr.z = v.interaction_box[1].z end
			if v.interaction_box[2].x > maxcr.x then maxcr.x = v.interaction_box[2].x end
			if v.interaction_box[2].y > maxcr.y then maxcr.y = v.interaction_box[2].y end
			if v.interaction_box[2].z > maxcr.z then maxcr.z = v.interaction_box[2].z end

			o:add_object_to_cluster(v, 1, nil, 1)
		end
	end

	o.bboxes = {bbox:new(mincr, maxcr)}

	o:divide(1)
	return o
end

function octree:add_object_to_cluster(obj, cluster_i, old_cluster_i, depth)
	self.clusters[cluster_i][obj] = true
	if self.obj_clusters[obj] == nil then
		self.obj_clusters[obj] = {} -- create a set of clusters
	end
	self.obj_clusters[obj][cluster_i] = true
	if old_cluster_i ~= nil then -- remove old cluster, it's not a leaf
		self.obj_clusters[obj][old_cluster_i] = nil
	end
	self.depth[cluster_i] = depth
end

function octree:divide(cluster_i)
	local mincr, maxcr = self.bboxes[cluster_i][1], self.bboxes[cluster_i][2] 
	local center = (mincr + maxcr) * 0.5

	if (maxcr - mincr):ln() < 2 or self.depth[cluster_i] > 4 then
		return false -- cluster is too small diagonally or it's too deep, terminate
	end

	self.children[cluster_i] = {} -- create a set of cluster's children

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
			if gmath.bbox_check_collision(self.bboxes[new_cluster_i], k.interaction_box) then
				self:add_object_to_cluster(k, new_cluster_i, cluster_i, self.depth[cluster_i] + 1)
			end
		end

		if next(self.clusters[new_cluster_i]) == nil then
			-- this cluster doesn't intersect with any objects; don't go deeper
			self.clusters[new_cluster_i] = nil
			self.bboxes[new_cluster_i] = nil
		else
			table.insert(self.children[cluster_i], new_cluster_i)
			self:divide(new_cluster_i)
		end
	end
	return true
end

function _check_bbox_octree_collision(bb, cluster_i) -- is to be called by C engine
	cluster_i = cluster_i or 1
	if cur_octree.children[cluster_i] ~= nil then
		for _, new_cluster_i in ipairs(cur_octree.children[cluster_i]) do
			if gmath.bbox_check_collision(cur_octree.bboxes[new_cluster_i], bb) then
				if _check_bbox_octree_collision(bb, new_cluster_i) then
					return true
				end
			end
		end
	else -- it's a leaf cluster, check collisions between bb and all other objects inside it
		for k in pairs(cur_octree.clusters[cluster_i]) do
			if gmath.bbox_check_collision(bb, k.bbox) then
				return true
			end
		end
	end
	return false
end
