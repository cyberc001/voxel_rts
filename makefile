INCLUDE := -I. -I./math -I./nn
FLAGS := -fms-extensions -g -fsanitize=address -Wall -Wextra -Wno-parentheses -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-unused-function `pkg-config --cflags freetype2`
LIBS := -lm -lGL -lGLU -lglfw -lpng -lSDL2 -lSDL2_mixer `pkg-config --libs freetype2`

CC := gcc $(INCLUDE) $(FLAGS) $(LIBS)
CCO := $(CC) -c
CLC := ar rcs

all: libvoxel_rts.a test

clean:
	-rm *.o
	-rm */*.o
	-rm libvoxel_rts.a

libvoxel_rts.a: render/base.o render/shader.o render/terrain.o render/font.o render/texture.o  formats/qb_vxl.o formats/lon.o formats/texture_atlas.o formats/texture.o formats/font.o  game/terrain.o ui/ui_element.o ui/label.o ui/button.o ui/textbox.o ui/scrollbar.o ui/listbox.o ui/itembox.o ui/hslider.o ui/vslider.o audio.o resources.o controls.o ticker.o utf.o object.o more_math.o ui.o
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

ui/ui_element.o: ui/ui_element.c ui/ui_element.h object.h
	$(CCO) $< -o $@
ui/label.o: ui/label.c ui/label.h ui/ui_element.h
	$(CCO) $< -o $@
ui/button.o: ui/button.c ui/button.h ui/ui_element.h
	$(CCO) $< -o $@
ui/textbox.o: ui/textbox.c ui/textbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/scrollbar.o: ui/scrollbar.c ui/scrollbar.h ui/ui_element.h
	$(CCO) $< -o $@
ui/listbox.o: ui/listbox.c ui/listbox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/itembox.o: ui/itembox.c ui/itembox.h ui/ui_element.h
	$(CCO) $< -o $@
ui/hslider.o: ui/hslider.c ui/hslider.h ui/ui_element.h
	$(CCO) $< -o $@
ui/vslider.o: ui/vslider.c ui/vslider.h ui/ui_element.h
	$(CCO) $< -o $@

audio.o: audio.c audio.h
	$(CCO) $< -o $@
resources.o: resources.c resources.h
	$(CCO) $< -o $@
controls.o: controls.c controls.h
	$(CCO) $< -o $@
ticker.o: ticker.c ticker.h
	$(CCO) $< -o $@
utf.o: utf.c utf.h
	$(CCO) $< -o $@
object.o: object.c object.h
	$(CCO) $< -o $@
ui.o: ui.c ui.h
	$(CCO) $< -o $@
more_math.o: more_math.c more_math.h
	$(CCO) $< -o $@
