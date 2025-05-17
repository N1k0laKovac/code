# ====================== 通用定义与路径配置 ======================
include ../defines.mk
DEFS +=

# 输出目录定义
OUTPUT_PATH = out
USER_OUTPUT = $(OUTPUT_PATH)/user

# 交叉编译工具链
CROSS_COMPILE = riscv64-unknown-elf-
CC            = $(CROSS_COMPILE)gcc
OBJCOPY       = $(CROSS_COMPILE)objcopy
OBJDUMP       = $(CROSS_COMPILE)objdump
MKDIR         = mkdir -p
RM            = rm -rf

# ====================== 目录创建规则 ======================
$(OUTPUT_PATH):
	@$(MKDIR) $@

$(USER_OUTPUT): $(OUTPUT_PATH)
	@$(MKDIR) $@

# QEMU配置
QEMU   = qemu-system-riscv32
QFLAGS = -machine virt -nographic -bios none
GDB    = gdb-multiarch

# ====================== 内核部分 ======================
KERNEL_DIR     = kernel
KERNEL_SRCS_ASM= $(KERNEL_DIR)/start.S $(filter-out $(KERNEL_DIR)/start.S,$(wildcard $(KERNEL_DIR)/*.S))
KERNEL_SRCS_C  = $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_OBJS_ASM= $(patsubst $(KERNEL_DIR)/%.S,$(OUTPUT_PATH)/%.o,$(KERNEL_SRCS_ASM))
KERNEL_OBJS_C  = $(patsubst $(KERNEL_DIR)/%.c,$(OUTPUT_PATH)/%.o,$(KERNEL_SRCS_C))
KERNEL_OBJS    = $(KERNEL_OBJS_ASM) $(KERNEL_OBJS_C)

CFLAGS += -I$(KERNEL_DIR) -nostdlib -fno-builtin -g -Wall -march=rv32g -mabi=ilp32

KERNEL_LD = $(KERNEL_DIR)/os.ld
ELF       = $(OUTPUT_PATH)/os.elf
BIN       = $(OUTPUT_PATH)/os.bin

# 内核链接时也要把用户 ELF 对象拉进来
USER_ELF_OBJ= $(OUTPUT_PATH)/user_elf.o

$(ELF): $(KERNEL_OBJS) $(USER_ELF_OBJ)
	$(CC) -E -P -x c $(DEFS) $(CFLAGS) $(KERNEL_LD) > $(OUTPUT_PATH)/os.ld.generated
	$(CC) $(CFLAGS) -T $(OUTPUT_PATH)/os.ld.generated -o $@ $^
	$(OBJCOPY) -O binary $@ $(BIN)

$(OUTPUT_PATH)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(DEFS) $(CFLAGS) -c -o $@ $<

$(OUTPUT_PATH)/%.o: $(KERNEL_DIR)/%.S
	$(CC) $(DEFS) $(CFLAGS) -c -o $@ $<


# ====================== 用户程序部分 ======================
USER_CFLAGS  = $(CFLAGS) -Iuser -fno-pic -mno-relax
USER_LDFLAGS = -T user/user.ld -nostdlib -march=rv32g -mabi=ilp32

USER_DIR     = user
USER_ASMS    = $(USER_DIR)/user_start.S $(filter-out $(USER_DIR)/user_start.S,$(wildcard $(USER_DIR)/*.S))
USER_SRCS    = $(wildcard $(USER_DIR)/*.c)
USER_OBJS    = \
   $(patsubst $(USER_DIR)/%.c,$(USER_OUTPUT)/%.o,$(USER_SRCS)) \
   $(patsubst $(USER_DIR)/%.S,$(USER_OUTPUT)/%.o,$(USER_ASMS))
USER_ELF     = $(USER_OUTPUT)/user.elf
USER_BIN     = $(USER_OUTPUT)/user.bin

# 生成 usys.S
$(USER_OUTPUT)/usys.S: $(USER_DIR)/usys.pl
	@$(MKDIR) $(@D)
	perl $< > $@

# 编译 usys.S
$(USER_OUTPUT)/usys.o: $(USER_OUTPUT)/usys.S
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

# 编译 C 源文件
$(USER_OUTPUT)/%.o: $(USER_DIR)/%.c
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

# 编译汇编源文件
$(USER_OUTPUT)/%.o: $(USER_DIR)/%.S
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

# 生成 user.elf（只生成 ELF，不生成 bin）
$(USER_ELF): $(USER_OBJS) $(USER_OUTPUT)/usys.o
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ $^

# 显式从 user.elf 生成 user.bin
$(USER_BIN): $(USER_ELF)
	$(OBJCOPY) -O binary $< $@

# 显式从 user.bin 生成 ELF 对象 user_elf.o，嵌入到内核中
$(USER_ELF_OBJ): $(USER_ELF)
	$(OBJCOPY) \
	-I binary \
	-O elf32-littleriscv \
	-B riscv \
	--rename-section .data=.user_elf \
	$< \
	$@

# ====================== 运行与调试 ======================
.DEFAULT_GOAL := all
all: $(OUTPUT_PATH) $(USER_OUTPUT) $(ELF) $(USER_ELF)

run: all
	@echo "Press Ctrl-A then X to exit QEMU"
	@echo "----------------------------------"
	@$(QEMU) $(QFLAGS) \
	    -serial mon:stdio \
	    -kernel $(ELF)

debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@pkill qemu-system-riscv32 || true
	@$(QEMU) $(QFLAGS) \
	  -serial mon:stdio \
	  -kernel $(ELF) \
	  -gdb tcp::1234 -S &
	@sleep 1
	@$(GDB) $(ELF) -q \
	  -ex "set arch riscv:rv32" \
	  -ex "target remote :1234" \
	  -ex "b load_elf_image" \
	  -ex "c"

code: all
	@$(OBJDUMP) -S $(ELF) | less


# ====================== 清理 ======================
clean:
	@$(RM) $(OUTPUT_PATH)
