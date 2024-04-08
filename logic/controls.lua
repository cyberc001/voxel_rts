player_selected_objects = {}
player_selected_objects.__mode = "k"

function controls_tick()
	if controls.get_selection_query() then
		player_selected_objects = {}
		for _,v in ipairs(game_object_arr) do
			if v.team == player_team and gmath.hexahedron_is_selected(v.hitbox) then
				player_selected_objects[v] = true
			end
		end
	end
	order_pos = controls.get_order_query() 
	if order_pos then
		for k in pairs(player_selected_objects) do
			k:set_goal(order_pos)
		end
	end
end
