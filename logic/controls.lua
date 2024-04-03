player_selected_objects = {}
player_selected_objects.__mode = "k"

function controls_tick()
	selection_queried = controls.get_selection_query()
	if selection_queried then
		player_selected_objects = {}
		for _,v in ipairs(game_object_arr) do
			if v.team == player_team and gmath.hexahedron_is_selected(v.hitbox) then
				player_selected_objects[v] = true
			end
		end
	end
end
