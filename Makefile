### Build Options ###
BASEROM	:= baserom.z64
TARGET := dukenukem
COMPARE ?= 1
VERBOSE ?= 0
BUILD_DIR := build

# Fail early if baserom does not exist
ifeq ($(wildcard $(BASEROM)),)
$(error Baserom `$(BASEROM)' not found.)
endif

ifeq ($(VERBOSE),0)
V := @
endif

### Output ###
ROM       := $(BUILD_DIR)/$(TARGET).z64
ELF       := $(BUILD_DIR)/$(TARGET).elf
LD_SCRIPT := $(TARGET).ld
LD_MAP    := $(BUILD_DIR)/$(TARGET).map

### Tools ###
PYTHON        := python3
N64CKSUM      := $(PYTHON) tools/n64cksum.py
ASM_PROCESSOR := $(PYTHON) tools/asm-processor/build.py
O32_TOOL      := $(PYTHON) tools/set_o32abi_bit.py
SPLAT_YAML    := $(TARGET).yaml
SPLAT         := splat split $(SPLAT_YAML)
DIFF          := diff

CROSS   := mips-linux-gnu-
AS      := $(CROSS)gcc -x assembler-with-cpp
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
CPP     := $(CROSS)cpp
CC      := tools/ido5.3/cc

PRINT := printf '
 ENDCOLOR := \033[0m
 WHITE     := \033[0m
 ENDWHITE  := $(ENDCOLOR)
 GREEN     := \033[0;32m
 ENDGREEN  := $(ENDCOLOR)
 BLUE      := \033[0;34m
 ENDBLUE   := $(ENDCOLOR)
 YELLOW    := \033[0;33m
 ENDYELLOW := $(ENDCOLOR)
 PURPLE    := \033[0;35m
 ENDPURPLE := $(ENDCOLOR)
ENDLINE := \n'

### Compiler Options ###
ASFLAGS   := -w -nostdinc -c -G 0 -march=vr4300 -mgp32 -mfp32 -mno-abicalls -mips3 -D_LANGUAGE_ASSEMBLY

CFLAGS    := -c -Wab,-r4300_mul -G 0 -nostdinc -Xcpluscomm -non_shared -D_LANGUAGE_C

CPPFLAGS  := -I include -I include/libultra -I include/libultra/PR \
		     -DTARGET_N64 -D_FINALROM -DF3DEX_GBI -DNDEBUG -D_MIPS_SZLONG=32

OPTFLAGS  := -O2 -mips2 -Olimit 2000

LDFLAGS   := -T undefined_funcs_auto.txt -T undefined_syms_auto.txt -T undefined_syms.txt -T $(LD_SCRIPT) -Map $(LD_MAP)

# Object files
OBJECTS := $(shell grep -E 'build.+\.o' $(LD_SCRIPT) -o)
OBJECTS := $(OBJECTS:build/%=$(BUILD_DIR)/%)

### Targets ###
$(BUILD_DIR)/src/libultra/io/%.c.o: OPTFLAGS := -O1 -mips2
$(BUILD_DIR)/src/libultra/os/%.c.o: OPTFLAGS := -O1 -mips2
$(BUILD_DIR)/src/libultra/libc/%.c.o: OPTFLAGS := -O3 -mips2
$(BUILD_DIR)/src/libultra/libc/ll.c.o: OPTFLAGS := -O1 -mips3 -o32
$(BUILD_DIR)/src/libultra/libc/llcvt.c.o: OPTFLAGS := -O1 -mips3 -o32
$(BUILD_DIR)/src/libultra/gu/%.c.o: OPTFLAGS := -O3 -mips2
$(BUILD_DIR)/src/libultra/audio/%.c.o: OPTFLAGS := -O3 -mips2

all: $(ROM)

clean:
	$(V)rm -rf $(BUILD_DIR)

distclean: clean
	$(V)rm -rf assets/
	$(V)rm -rf asm/
	$(V)rm -f $(LD_SCRIPT)
	$(V)rm -f undefined_funcs_auto.txt
	$(V)rm -f undefined_syms_auto.txt

split:
	$(V)$(SPLAT)

setup: distclean split

# Compile .c files
$(BUILD_DIR)/%.c.o: %.c
	@$(PRINT)$(GREEN)Compiling C file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(ASM_PROCESSOR) $(CC) -- $(AS) $(ASFLAGS) -- $(CFLAGS) $(CPPFLAGS) $(OPTFLAGS) -o $@ $<

$(BUILD_DIR)/src/libultra/%.c.o: src/libultra/%.c
	@$(PRINT)$(GREEN)Compiling C file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(CC) $(CFLAGS) $(CPPFLAGS) $(OPTFLAGS) -o $@ $<
	$(V)$(O32_TOOL) $@

$(BUILD_DIR)/asm/%.s.o: asm/%.s
	@$(PRINT)$(GREEN)Assembling asm file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(AS) $(ASFLAGS) $(CPPFLAGS) -o $@ $<

$(BUILD_DIR)/%.bin.o: %.bin
	@$(PRINT)$(GREEN)objcopying binary file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(LD) -r -b binary -o $@ $<

$(ELF): $(OBJECTS)
	@$(PRINT)$(GREEN)Linking elf file: $(ENDGREEN)$(BLUE)$@$(ENDBLUE)$(ENDLINE)
	$(V)$(LD) $(LDFLAGS) -o $@

$(ROM): $(ELF)
	$(V)$(OBJCOPY) -O binary --gap-fill 0x00 --pad-to 0x800000 $< $@
	$(V)$(N64CKSUM) $@
ifeq ($(COMPARE),1)
	@$(DIFF) $(BASEROM) $(ROM) && printf "OK\n" || (echo 'The build succeeded, but did not match the base ROM. This is expected if you are making changes to the game. To skip this check, use "make COMPARE=0".' && false)
endif

### Make Settings ###

.PHONY: all clean distclean setup split

# Remove built-in implicit rules to improve performance
MAKEFLAGS += --no-builtin-rules

# Print target for debugging
print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
