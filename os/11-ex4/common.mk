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

USER_PROGRAMS := user1 user2  # 定义要编译的用户程序列表
USER_ELF_OBJS := $(foreach prog,$(USER_PROGRAMS),$(OUTPUT_PATH)/$(prog)_elf.o)  # 修改点1：更新为多用户ELF对象

USER_DIR     = user
USER_ASMS    = $(USER_DIR)/user_start.S $(filter-out $(USER_DIR)/user_start.S,$(wildcard $(USER_DIR)/*.S))
USER_SRCS    = $(wildcard $(USER_DIR)/*.c)

# ====================== 修改点2：将通用规则移到宏外部 ======================
# 编译用户程序的C文件（通用规则，所有用户程序共享）
$(USER_OUTPUT)/%.o: $(USER_DIR)/%.c
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

# 编译用户程序的汇编文件（通用规则，所有用户程序共享）
$(USER_OUTPUT)/%.o: $(USER_DIR)/%.S
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

# 生成usys.S（通用规则）
$(USER_OUTPUT)/usys.S: $(USER_DIR)/usys.pl
	@$(MKDIR) $(@D)
	perl $< > $@

# 编译usys.o（通用规则）
$(USER_OUTPUT)/usys.o: $(USER_OUTPUT)/usys.S
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<



d ====================== 修改点3：调整BUILD_USER_PROGRAM宏 ======================
define BUILD_USER_PROGRAM
$(1)_SRCS := user/$(1).c user/user_start.S  # 明确指定用户程序源文件
$(1)_OBJS := $$(patsubst $(USER_DIR)/%.c, $(USER_OUTPUT)/%.o, $$(filter %.c, $$($(1)_SRCS))) \
              $$(patsubst $(USER_DIR)/%.S, $(USER_OUTPUT)/%.o, $$(filter %.S, $$($(1)_SRCS)))
$(1)_ELF := $(USER_OUTPUT)/$(1).elf
$(1)_BIN := $(USER_OUTPUT)/$(1).bin

# 编译用户程序ELF
$$($(1)_ELF): $$($(1)_OBJS) $(USER_OUTPUT)/usys.o
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -Wl,--defsym=main=$(1)_main -o $$@ $$^

# 生成用户程序BIN
$$($(1)_BIN): $$($(1)_ELF)
	$(OBJCOPY) -O binary $$< $$@

# 生成内核可链接的ELF对象
$(OUTPUT_PATH)/$(1)_elf.o: $$($(1)_BIN)
	$(OBJCOPY) \
	  -I binary \
	  -O elf32-littleriscv \
	  -B riscv \
	  --rename-section .data=.user_elf_$(1) \
	  $$< \
	  $$@

# 添加依赖
all: $$($(1)_ELF)
endef

# 生成所有用户程序的规则
$(foreach prog,$(USER_PROGRAMS),$(eval $(call BUILD_USER_PROGRAM,$(prog))))

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
	@$(RM) $(OUTPUT_PATH)/user/*   # 仅删除目录内容
