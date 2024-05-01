local pointer_pos = vec3:new(0, 0, 0)
local pointer_lifetime_left = 0
local pointer_lifetime = 900
local pointer_size = 0
local pointer_size_delta = 0.01

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
	local cur_pos = pointer_pos.x and pointer_pos or pointer_pos.pos -- if it's not a vector, it's a game object and it's position should be tracked
	render.render_obj_draw(cur_pointer_model, cur_pos + vec3:new(-0.1, 0.5, 0) + vec3:new(0, 0.4, 0) * (pointer_bob*pointer_bob), nil, vec3:new(1, 1, 1) * pointer_size)
end
function pointer_tick()
	if pointer_lifetime_left == 0 and pointer_size > 0 then
		pointer_size = pointer_size - pointer_size_delta
	end
	if pointer_lifetime_left > 0 then
		pointer_lifetime_left = pointer_lifetime_left - 1
		pointer_bob = pointer_bob + pointer_bob_delta * (pointer_dir_up and 1 or -1)
		if math.abs(pointer_bob) >= 1 then
			pointer_dir_up = not pointer_dir_up
		end
	end
end
