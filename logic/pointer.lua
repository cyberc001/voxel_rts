local pointer_pos = vec3:new(0, 0, 0)
local pointer_lifetime_left = 0
local pointer_lifetime = 7
local pointer_size = 0
local pointer_size_delta = 0.01

local pointer_rot = vec3:new(0, 0, 0)
local pointer_bob = 0 -- parameter in range (-1; 1) for animation formula
local pointer_bob_delta = 0.01
local pointer_dir_up = true

local pointer_models = {
	move = render.model_find("pointer_move"),
	attack = render.model_find("pointer_attack")
}
local cur_pointer_model = pointer_models.move

function pointer_flash(pos, model)
	pointer_pos = pos
	pointer_lifetime_left = pointer_lifetime
	pointer_size = 1
	if model then cur_pointer_model = pointer_models[model] end
end

function pointer_render()
	local cur_pos
	if pointer_pos.x then cur_pos = pointer_pos -- a vector
	else -- a game object that should be tracked
		if not pointer_pos:exists() then
			pointer_clear()
			cur_pos = vec3:new()
		else
			cur_pos = pointer_pos.body.pos
		end
	end
	render.render_obj_draw(cur_pointer_model,
				cur_pos + vec3:new(0, 0.5, 0) + vec3:new(0, 0.4, 0) * (pointer_bob*pointer_bob),
				gmath.quat_from_rot(pointer_rot),
				vec3:new(1, 1, 1) * pointer_size)
end
function pointer_tick(time_delta)
	if pointer_lifetime_left <= 0 and pointer_size > 0 then
		pointer_size = pointer_size - pointer_size_delta
	end
	if pointer_lifetime_left > 0 then
		pointer_lifetime_left = pointer_lifetime_left - time_delta
		pointer_bob = pointer_bob + pointer_bob_delta * (pointer_dir_up and 1 or -1)
		if math.abs(pointer_bob) >= 1 then
			pointer_dir_up = not pointer_dir_up
		end
		pointer_rot.x = pointer_rot.x + 1
		if pointer_rot.x > 360 then pointer_rot.x = pointer_rot.x - 360 end
	end
end
function pointer_clear()
	pointer_pos = vec3:new()
	pointer_size = 0
	pointer_lifetime_left = 0
end
