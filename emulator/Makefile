 ## ======================================================================
## partial Makefile provided to students
##

GTK_INCLUDE := `pkg-config --cflags gtk+-3.0`
GTK_LIBS := `pkg-config --libs gtk+-3.0` 

.PHONY: clean new style feedback submit1 submit2 submit

CFLAGS += -Wall -pedantic -g -fsanitize=address


# a bit more checks if you'd like to (uncomment
 #CFLAGS += -Wextra -Wfloat-equal -Wshadow                         \
 -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings \
 -Wconversion -Wunreachable-code

# uncomment if you want to add DEBUG flag
#CPPFLAGS += -DDEBUG
#CPPFLAGS += -DBLARGG 
#CPPFLAGS += -D_DEFAULT_SOURCE
#CPPFLAGS += -DWITH_PRINT

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -pthread -lsubunit -fsanitize=address


all:: gbsimulator

TARGETS := 
CHECK_TARGETS := unit-test-bit-vector unit-test-bit unit-test-alu unit-test-memory unit-test-component unit-test-bus unit-test-cpu unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 unit-test-cartridge unit-test-timer  test-cpu-week08 unit-test-alu_ext unit-test-cpu-dispatch unit-test-bit-vector
OBJS =
OBJS_NO_STATIC_TESTS =
OBJS_STATIC_TESTS = 
OBJS = $(OBJS_STATIC_TESTS) $(OBJS_NO_STATIC_TESTS)

# ----------------------------------------------------------------------
# This part is to make your life easier. See handouts how to make use of it.


clean::
	-@/bin/rm -f *.o *~ $(CHECK_TARGETS)

new: clean all

static-check:
	scan-build -analyze-headers --status-bugs -maxloop 64 make CC=clang new

style:
	astyle -n -o -A8 -xt0 *.[ch]

# all those libs are required on Debian, adapt to your box
$(CHECK_TARGETS): LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

check:: $(CHECK_TARGETS)
	$(foreach target,$(CHECK_TARGETS),./$(target) &&) true

# target to run tests
check:: all
	@if ls tests/*.*.sh 1> /dev/null 2>&1; then \
	  for file in tests/*.*.sh; do [ -x $$file ] || echo "Launching $$file"; ./$$file || exit 1; done; \
	fi

IMAGE=chappeli/feedback:latest
feedback:
	@docker pull $(IMAGE)
	@docker run -it --rm -v ${PWD}:/home/tester/done $(IMAGE)

SUBMIT_SCRIPT=../provided/submit.sh
submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit2: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 2

submit:
	@printf 'what "make submit"??\nIt'\''s either "make submit1" or "make submit2"...\n'

libsid.so: sidlib.c
	$(CC) -fPIC -shared $(CPPFLAGS) $(CFLAGS) $(GTK_INCLUDE) $^ -o $@


all-tests : unit-test-bit unit-test-alu unit-test-bus unit-test-memory unit-test-component unit-test-cpu unit-test-cpu-dispatch unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 unit-test-cartridge unit-test-alu_ext unit-test-cpu-dispatch unit-test-bit-vector test-gameboy test-image test-cpu-week08

libsid_demo.o: CFLAGS += $(GTK_INCLUDE)

libsid_demo: LDFLAGS += -L.
libsid_demo: LDLIBS += $(GTK_LIBS) -lsid
libsid_demo: libsid_demo.o libsid.so

gbsimulator.o: CFLAGS += $(GTK_INCLUDE)
gameboy: gameboy.o component.o error.o bus.o bit.o memory.o
gbsimulator: gbsimulator.o gameboy.o libcs212gbfinalext.so libsid.so image.o gameboy.o component.o cartridge.o bus.o bootrom.o timer.o cpu.o alu.o bit.o opcode.o cpu-storage.o cpu-registers.o memory.o cpu-alu.o error.o bit_vector.o
gbsimulator: LDLIBS += $(GTK_LIBS) -lsid
gbsimulator: LDFLAGS += -L.

unit-test-bit: unit-test-bit.o bit.o
unit-test-alu: unit-test-alu.o bit.o alu.o error.o
unit-test-bus: unit-test-bus.o bit.o component.o bus.o memory.o error.o
unit-test-memory: unit-test-memory.o bit.o component.o bus.o memory.o error.o
unit-test-component: unit-test-component.o component.o bus.o memory.o bit.o error.o
unit-test-cpu: unit-test-cpu.o cpu.o cpu-registers.o cpu-storage.o alu.o bit.o bus.o component.o memory.o cpu-alu.o opcode.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-cpu-dispatch-week08: unit-test-cpu-dispatch-week08.o cpu-registers.o cpu-storage.o alu.o bit.o bus.o component.o memory.o cpu-alu.o opcode.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-cpu-dispatch-week09: unit-test-cpu-dispatch-week09.o cpu-registers.o cpu-storage.o alu.o bit.o bus.o component.o memory.o cpu-alu.o opcode.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-cartridge: unit-test-cartridge.o cartridge.o component.o bus.o memory.o bit.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-timer: unit-test-timer.o timer.o bit.o cpu-storage.o cpu.o cpu-registers.o opcode.o alu.o bus.o component.o memory.o cpu-alu.o alu.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-bit-vector: unit-test-bit-vector.o bit_vector.o error.o bit.o
unit-test-cpu-dispatch: unit-test-cpu-dispatch.o cpu-storage.o cpu-registers.o cpu-alu.o opcode.o alu.o component.o memory.o bus.o bit.o error.o libcs212gbfinalext.so image.o bit_vector.o libcs212gbfinalext.so image.o bit_vector.o

unit-test-alu_ext.o: CFLAGS += $(GTK_INCLUDE)
unit-test-alu_ext: unit-test-alu_ext.o cpu.o opcode.o memory.o component.o cpu-storage.o cpu-registers.o cpu-alu.o alu.o bus.o bit.o error.o libcs212gbfinalext.so image.o bit_vector.o
unit-test-alu_ext: LDFLAGS += -L.
unit-test-alu_ext: LDLIBS += $(GTK_LIBS) -lsid

test-gameboy: test-gameboy.o gameboy.o component.o cartridge.o bus.o bootrom.o timer.o cpu.o alu.o bit.o opcode.o cpu-storage.o cpu-registers.o memory.o cpu-alu.o error.o libcs212gbfinalext.so image.o bit_vector.o opcode.o cpu-storage.o cpu-registers.o memory.o cpu-alu.o error.o

test-cpu-week08.o: CFLAGS += $(GTK_INCLUDE)
test-cpu-week08: opcode.o error.o cpu.o util.o cpu-storage.o bus.o cpu-alu.o alu.o bit.o cpu-registers.o component.o memory.o libcs212gbfinalext.so image.o bit_vector.o
test-cpu-week08: LDFLAGS += -L.
test-cpu-week08: LDLIBS += $(GTK_LIBS) -lsid


test-image.o: CFLAGS += $(GTK_INCLUDE)
test-image: test-image.o libsid.so bit_vector.o bit.o error.o image.o
test-image: LDFLAGS += -L.
test-image: LDLIBS += $(GTK_LIBS) -lsid



#################################################

alu.o: alu.c alu.h bit.h error.h
bit.o: bit.c bit.h
bit_vector.o: bit_vector.c bit.h bit_vector.h error.h myMacros.h cpu.h \
 alu.h bus.h memory.h component.h opcode.h
bootrom.o: bootrom.c bus.h memory.h component.h error.h gameboy.h \
 cartridge.h cpu.h alu.h bit.h timer.h lcdc.h image.h bit_vector.h \
 joypad.h bootrom.h
bus.o: bus.c bus.h memory.h component.h error.h bit.h myMacros.h cpu.h \
 alu.h opcode.h
cartridge.o: cartridge.c component.h error.h memory.h bus.h cartridge.h
component.o: component.c error.h component.h memory.h
cpu-alu.o: cpu-alu.c error.h bit.h alu.h cpu-alu.h opcode.h cpu.h bus.h \
 memory.h component.h cpu-storage.h cpu-registers.h myMacros.h
cpu.o: cpu.c error.h opcode.h bit.h cpu.h alu.h bus.h memory.h \
 component.h cpu-alu.h cpu-registers.h util.h cpu-storage.h myMacros.h
cpu-registers.o: cpu-registers.c cpu-registers.h cpu.h alu.h bit.h \
 error.h bus.h memory.h component.h myMacros.h opcode.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h cpu-registers.h gameboy.h \
 cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h util.h \
 myMacros.h
error.o: error.c
gameboy.o: gameboy.c error.h util.h bootrom.h bus.h memory.h component.h \
 gameboy.h cartridge.h cpu.h alu.h bit.h timer.h lcdc.h image.h \
 bit_vector.h joypad.h
gbsimulator.o: gbsimulator.c sidlib.h lcdc.h cpu.h alu.h bit.h error.h \
 bus.h memory.h component.h image.h bit_vector.h gameboy.h cartridge.h \
 timer.h joypad.h util.h
image.o: image.c error.h image.h bit_vector.h bit.h
libsid_demo.o: libsid_demo.c sidlib.h
memory.o: memory.c memory.h error.h
opcode.o: opcode.c opcode.h bit.h
sidlib.o: sidlib.c sidlib.h
test-cpu-week08.o: test-cpu-week08.c opcode.h bit.h cpu.h alu.h error.h \
 bus.h memory.h component.h cpu-storage.h util.h
test-gameboy.o: test-gameboy.c gameboy.h bus.h memory.h component.h \
 error.h cartridge.h cpu.h alu.h bit.h timer.h lcdc.h image.h \
 bit_vector.h joypad.h util.h
test-image.o: test-image.c error.h util.h image.h bit_vector.h bit.h \
 sidlib.h
timer.o: timer.c component.h error.h memory.h bit.h cpu.h alu.h bus.h \
 timer.h cpu-storage.h opcode.h gameboy.h cartridge.h lcdc.h image.h \
 bit_vector.h joypad.h
unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-alu_ext.o: unit-test-alu_ext.c tests.h error.h alu.h bit.h \
 alu_ext.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bit-vector.o: unit-test-bit-vector.c tests.h error.h \
 bit_vector.h bit.h image.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h memory.h \
 component.h util.h
unit-test-cartridge.o: unit-test-cartridge.c tests.h error.h cartridge.h \
 component.h memory.h bus.h cpu.h alu.h bit.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h \
 memory.h component.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h util.h cpu.h \
 bus.h memory.h component.h cpu-registers.h cpu-storage.h opcode.h \
 cpu-alu.h
unit-test-cpu-dispatch.o: unit-test-cpu-dispatch.c tests.h error.h alu.h \
 bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h \
 myMacros.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h \
 myMacros.h
unit-test-cpu-dispatch-week09.o: unit-test-cpu-dispatch-week09.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h \
 myMacros.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h memory.h \
 component.h
unit-test-timer.o: unit-test-timer.c util.h tests.h error.h timer.h \
 component.h memory.h bit.h cpu.h alu.h bus.h
util.o: util.c