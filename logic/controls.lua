player_selected_objects = {}
player_selected_objects.__mode = "k"

function controls_tick()
	local query = controls.get_selection_query()
	if query == 0 then
		return
	end
	local order_pos = controls.get_order_pos() 

	local selected_something = false
	if query == 1 or query == 2 then
		for v in pairs(game_object_arr) do
			if v.body:is_selected() and v.team == player_team then
				if not selected_something then
					player_selected_objects = {}
					selected_something = true
				end
				player_selected_objects[v] = true
			end
		end
		if not selected_something then
			player_selected_objects = {}
		end
	elseif query == 3 then
		for v in pairs(game_object_arr) do
			if v.body:is_selected() and not v.team.allies[player_team] then
				print("attack", v)
				selected_something = true
				pointer_flash(v, "attack")
				for k in pairs(player_selected_objects) do
					k:set_target(v)
				end
				break
			end
		end
		if not selected_something then
			if order_pos then
				for k in pairs(player_selected_objects) do
					print("move", order_pos)
					k:clear_target()
					k:set_goal(order_pos)
					pointer_flash(order_pos, "move")
				end
			end
		end
	end

	--[[local active_selection = controls.has_active_selection()
	local selected_something = false
	if active_selection then
		for v,_ in pairs(game_object_arr) do
			if v.body:is_selected() then
				if v.team == player_team then
					if not selected_something then
						player_selected_objects = {}
						selected_something = true
					end
					player_selected_objects[v] = true
				elseif selection_query == 2 and not v.team.allies[player_team] then -- check for an attack order
					selected_something = true
					pointer_flash(v, "attack")
					for k in pairs(player_selected_objects) do
						k:set_target(v)
					end
					break
				end
			end
		end
	end

	if selection_query == 2 and not selected_something then
		if order_pos then
			for k in pairs(player_selected_objects) do
				k:clear_target()
				k:set_goal(order_pos)
				pointer_flash(order_pos, "move")
			end
		elseif active_selection then
			player_selected_objects = {}
		end
	elseif selection_query == 1 and not selected_something and active_selection then
		player_selected_objects = {}
	end]]--
end
