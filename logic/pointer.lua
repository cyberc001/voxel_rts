local pointer_pos = vec3:new(0, 0, 0)
local pointer_alpha = 0

local pointer_bob = 0 -- parameter in range (-1; 1) for animation formula
local pointer_bob_delta = 0.01
local pointer_dir_up = true

local pointer_model = render.model_find("pointer")

function pointer_flash(pos)
	pointer_pos = pos
	pointer_pos.x = pointer_pos.x - 0.1
end

function pointer_render()
	render.render_obj_draw(pointer_model, pointer_pos + vec3:new(0, 0.4, 0) * (pointer_bob*pointer_bob))
end
function pointer_tick()
	pointer_bob = pointer_bob + pointer_bob_delta * (pointer_dir_up and 1 or -1)
	if math.abs(pointer_bob) >= 1 then
		pointer_dir_up = not pointer_dir_up
	end
end
