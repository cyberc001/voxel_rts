INCLUDE := -I. -I./math -I./nn
FLAGS := -fms-extensions -g -fsanitize=address -Wall -Wextra -Wno-parentheses -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-unused-function `pkg-config --cflags freetype2`
LIBS := -lm -lGL -lGLU -lglfw -lpng -lSDL2 -lSDL2_mixer `pkg-config --libs freetype2` -llua

CC := gcc $(INCLUDE) $(FLAGS) $(LIBS)
CCO := $(CC) -c
CLC := ar rcs

all: libvoxel_rts.a test

clean:
	-rm *.o
	-rm */*.o
	-rm */*/*.o
	-rm libvoxel_rts.a

libvoxel_rts.a: render/base.o render/shader.o render/terrain.o render/font.o render/texture.o render/primitive.o render/list.o
libvoxel_rts.a: formats/qb_vxl.o formats/lon.o formats/texture_atlas.o formats/texture.o formats/font.o
libvoxel_rts.a: game/terrain.o game/logic.o game/pathfinding.o game/logic/render.o game/logic/math.o game/logic/path.o game/logic/body.o game/logic/controls.o
libvoxel_rts.a: ui/ui_element.o ui/elements/label.o ui/elements/button.o ui/elements/textbox.o ui/elements/scrollbar.o ui/elements/listbox.o ui/elements/itembox.o ui/elements/hslider.o ui/elements/vslider.o ui/elements/checkbox.o ui/elements/radiobox.o ui/elements/selection.o
libvoxel_rts.a: more_math.o math/collision.o math/hexahedron.o math/vec.o math/mat.o math/quat.o math/body.o
libvoxel_rts.a: audio.o resources.o ticker.o utf.o object.o ui.o controls.o controls/camera.o controls/selection.o
	$(CLC) $@ $^
test: test.c libvoxel_rts.a
	$(CC) $< -o $@ -L. -lvoxel_rts

render/base.o: render/base.c render/base.h
	$(CCO) $< -o $@
render/shader.o: render/shader.c render/shader.h
	$(CCO) $< -o $@
render/terrain.o: render/terrain.c render/terrain.h game/terrain.h
	$(CCO) $< -o $@
render/font.o: render/font.c render/font.h formats/font.h
	$(CCO) $< -o $@
render/texture.o: render/texture.c render/texture.h
	$(CCO) $< -o $@
render/primitive.o: render/primitive.c render/primitive.h math/hexahedron.h
	$(CCO) $< -o $@
render/list.o: render/list.c render/list.h render/base.h
	$(CCO) $< -o $@

formats/qb_vxl.o: formats/qb_vxl.c formats/qb_vxl.h
	$(CCO) $< -o $@
formats/lon.o: formats/lon.c formats/lon.h
	$(CCO) $< -o $@
formats/texture_atlas.o: formats/texture_atlas.c formats/texture_atlas.h
	$(CCO) $< -o $@
formats/texture.o: formats/texture.c formats/texture.h
	$(CCO) $< -o $@
formats/font.o: formats/font.c formats/font.h
	$(CCO) $< -o $@

game/terrain.o: game/terrain.c game/terrain.h
	$(CCO) $< -o $@
game/logic.o: game/logic.c game/logic.h
	$(CCO) $< -o $@
game/pathfinding.o: game/pathfinding.c game/pathfinding.h
	$(CCO) $< -o $@
game/logic/render.o: game/logic/render.c game/logic/render.h game/logic.h render/base.h
	$(CCO) $< -o $@
game/logic/math.o: game/logic/math.c game/logic/math.h game/logic.h
	$(CCO) $< -o $@
game/logic/path.o: game/logic/path.c game/logic/path.h game/logic.h
	$(CCO) $< -o $@
game/logic/body.o: game/logic/body.c game/logic/body.h game/logic.h
	$(CCO) $< -o $@
game/logic/controls.o: game/logic/controls.c game/logic/controls.h game/logic.h
	$(CCO) $< -o $@

ui/ui_element.o: ui/ui_element.c ui/ui_element.h object.h
	$(CCO) $< -o $@
ui/elements/label.o: ui/elements/label.c ui/elements/label.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/button.o: ui/elements/button.c ui/elements/button.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/textbox.o: ui/elements/textbox.c ui/elements/textbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/scrollbar.o: ui/elements/scrollbar.c ui/elements/scrollbar.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/listbox.o: ui/elements/listbox.c ui/elements/listbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/itembox.o: ui/elements/itembox.c ui/elements/itembox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/hslider.o: ui/elements/hslider.c ui/elements/hslider.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/vslider.o: ui/elements/vslider.c ui/elements/vslider.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/checkbox.o: ui/elements/checkbox.c ui/elements/checkbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/radiobox.o: ui/elements/radiobox.c ui/elements/checkbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/elements/selection.o: ui/elements/selection.c ui/ui_element.h
	$(CCO) $< -o $@

ui/tickers/selection.o: ui/tickers/selection.c ui/tickers/selection.h ticker.h
	$(CCO) $< -o $@

more_math.o: more_math.c more_math.h
	$(CCO) $< -o $@
math/collision.o: math/collision.c math/collision.h math/hexahedron.h
	$(CCO) $< -o $@
math/hexahedron.o: math/hexahedron.c math/hexahedron.h math/body.h
	$(CCO) $< -o $@
math/vec.o: math/vec.c math/vec.h
	$(CCO) $< -o $@
math/mat.o: math/mat.c math/mat.h
	$(CCO) $< -o $@
math/quat.o: math/quat.c math/quat.h math/vec.h math/mat.h
	$(CCO) $< -o $@
math/body.o: math/body.c math/body.h

audio.o: audio.c audio.h
	$(CCO) $< -o $@
resources.o: resources.c resources.h
	$(CCO) $< -o $@
ticker.o: ticker.c ticker.h
	$(CCO) $< -o $@
utf.o: utf.c utf.h
	$(CCO) $< -o $@
object.o: object.c object.h
	$(CCO) $< -o $@
ui.o: ui.c ui.h
	$(CCO) $< -o $@

controls.o: controls.c controls.h
	$(CCO) $< -o $@
controls/camera.o: controls/camera.c controls/camera.h controls.h
	$(CCO) $< -o $@
controls/selection.o: controls/selection.c controls/selection.h controls.h
	$(CCO) $< -o $@
