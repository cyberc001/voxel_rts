player_selected_objects = {}
player_selected_objects.__mode = "k"

function controls_tick()
	local selected_something = false
	local selection_query = controls.get_selection_query()
	if selection_query ~= 0 then
		for _,v in ipairs(game_object_arr) do
			if gmath.hexahedron_is_selected(v.hitbox) then
				if v.team == player_team then
					if not selected_something then
						player_selected_objects = {}
						selected_something = true
					end
					player_selected_objects[v] = true
				elseif selection_query == 2 and not v.team.allies[player_team] then -- check for an attack order
					selected_something = true
					for k in pairs(player_selected_objects) do
						k:set_target(v)
					end
					break
				end
			end
		end
	end

	local order_pos = controls.get_order_query() -- quering needs to be done on every tick regardless to reset query status
	if selection_query ~= 1 and order_pos and not selected_something then
		for k in pairs(player_selected_objects) do
			k:clear_target()
			k:set_goal(order_pos)
			pointer_flash(vec3:new(order_pos.x, order_pos.y + 0.5, order_pos.z))
		end
	end
end
