REALWINPATH = $(subst /,\,$(realpath $(1)))
CEDEV ?= ..\toolchain\CEdev
CEDEV := $(call REALWINPATH,$(CEDEV))
BIN = $(CEDEV)\bin
INCLUDE = $(CEDEV)\include
CEALGOS ?= ../CE-Algorithms/src

TARGET ?= scanln

CC = @ "$(BIN)\eZ80cc"
LD = @ "$(BIN)\eZ80link"
CV = @ "$(BIN)\convhex"
RM = del /f

SOURCES = $(wildcard *.c) $(subst /,\,$(wildcard $(CEALGOS)/*.c)) $(subst /,\,$(wildcard $(CEALGOS)/*/*.c))
ASMSOURCES = $(wildcard *.asm)
OBJECTS = $(SOURCES:%.c=%.obj) $(ASMSOURCES:%.asm=%.obj)

HEADERS = .;$(CEDEV)\include\ce;$(CEDEV)\include\ce\asm;$(CEDEV)\include;$(CEDEV)\include\std;$(CEDEV)\lib\ce;$(CEDEV)\lib\ce\asm
LIBRARIES = $(CEDEV)\lib\std\cdebug.lib $(CEDEV)\lib\std\chelp.lib $(CEDEV)\lib\std\crt.lib $(CEDEV)\lib\std\crtS.lib $(CEDEV)\lib\std\nokernel.lib $(CEDEV)\lib\std\fplib.lib $(CEDEV)\lib\std\fplibS.lib
STARTUPMODULE = $(CEDEV)\include\startup\cstartup.obj

ASM_FLAGS = \
	-name -define:_EZ80=1 -define:_SIMULATE=1 -include:$(HEADERS) -NOlist -NOlistmac \
	-pagelen:250 -pagewidth:132 -quiet -sdiopt -warn -NOdebug -NOigcase -cpu:eZ80190

CFLAGS =  \
	-quiet -define:_EZ80190 -define:_EZ80 -define:NDEBUG -define:_SIMULATE -NOlistinc -NOmodsect -cpu:eZ80190 -keepasm \
-optspeed -NOreduceopt -NOgenprintf -stdinc:"$(HEADERS)" -usrinc:".;$(call REALWINPATH,$(CEALGOS))" -NOdebug \
	-asmsw:"$(ASM_FLAGS)" -asm $(ASMSOURCES)

LDFLAGS += GROUP MEMORY = ROM, RAM

empty =
space = $(empty) $(empty)
comma = ,

all : $(TARGET).8xp

$(TARGET).hex : $(OBJECTS) $(STARTUPMODULE) $(LIBRARIES)
	$(LD) @Linkcmd $@ = $(subst $(space),$(comma),$^) $(LDFLAGS)

%.8xp : %.hex
	$(CV) $(@:%.8xp=%)

%.obj : %.c
	cd $(dir $@) && \
	$(CC) $(CFLAGS) $(notdir $<)

%.obj : %.asm
	$(CC) $(CFLAGS) $<
    
clean :
	$(RM) $(OBJECTS:%.obj=%.src) $(OBJECTS) $(TARGET).*)

.PHONY : all clean
