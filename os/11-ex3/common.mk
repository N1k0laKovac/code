# ====================== 通用定义与路径配置 ======================
include ../defines.mk
DEFS +=

# 输出目录定义
OUTPUT_PATH = out
USER_OUTPUT = $(OUTPUT_PATH)/user

# 交叉编译工具链
CROSS_COMPILE = riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
MKDIR = mkdir -p
RM = rm -rf

# ====================== 目录创建规则 ======================
# 明确创建输出目录的规则
$(OUTPUT_PATH):
	@$(MKDIR) $@

$(USER_OUTPUT): $(OUTPUT_PATH)
	@$(MKDIR) $@

# QEMU配置
QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp 1 -machine virt -bios none
GDB = gdb-multiarch 

# ====================== 内核部分 ======================
KERNEL_DIR = kernel
# 确保 start.S 是第一个编译的文件
KERNEL_SRCS_ASM = $(KERNEL_DIR)/start.S $(filter-out $(KERNEL_DIR)/start.S, $(wildcard $(KERNEL_DIR)/*.S))
KERNEL_SRCS_C = $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_OBJS_C = $(patsubst $(KERNEL_DIR)/%.c, $(OUTPUT_PATH)/%.o, $(KERNEL_SRCS_C))
KERNEL_OBJS_ASM = $(patsubst $(KERNEL_DIR)/%.S, $(OUTPUT_PATH)/%.o, $(KERNEL_SRCS_ASM))
KERNEL_OBJS = $(KERNEL_OBJS_ASM) $(KERNEL_OBJS_C)

# 内核编译选项
CFLAGS += -I$(KERNEL_DIR) -nostdlib -fno-builtin -g -Wall -march=rv32g -mabi=ilp32

# 内核链接脚本
KERNEL_LD = $(KERNEL_DIR)/os.ld
ELF = $(OUTPUT_PATH)/os.elf
BIN = $(OUTPUT_PATH)/os.bin

# 内核编译规则
$(ELF): $(KERNEL_OBJS) $(USER_ELF_OBJ)
	$(CC) -E -P -x c $(DEFS) $(CFLAGS) $(KERNEL_LD) > $(OUTPUT_PATH)/os.ld.generated
	$(CC) $(CFLAGS) -T $(OUTPUT_PATH)/os.ld.generated -o $@ $^
	$(OBJCOPY) -O binary $@ $(BIN)

$(OUTPUT_PATH)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(DEFS) $(CFLAGS) -c -o $@ $<

$(OUTPUT_PATH)/%.o: $(KERNEL_DIR)/%.S
	$(CC) $(DEFS) $(CFLAGS) -c -o $@ $<

# ====================== 用户程序部分 ======================
# 用户程序专用编译选项
USER_CFLAGS = $(CFLAGS) -Iuser -fno-pic -mno-relax
USER_LDFLAGS = -T user/user.ld -nostdlib

# 用户程序源文件与对象文件
USER_DIR = user
USER_ASMS = $(USER_DIR)/user_start.S $(filter-out $(USER_DIR)/user_start.S, $(wildcard $(USER_DIR)/*.S))
USER_SRCS = $(wildcard $(USER_DIR)/*.c)
USER_OBJS = \
  $(patsubst $(USER_DIR)/%.c, $(USER_OUTPUT)/%.o, $(USER_SRCS)) \
  $(patsubst $(USER_DIR)/%.S, $(USER_OUTPUT)/%.o, $(USER_ASMS))
USER_ELF = $(USER_OUTPUT)/user.elf
USER_BIN = $(USER_OUTPUT)/user.bin

# 用户程序系统调用入口生成
$(USER_OUTPUT)/usys.S: $(USER_DIR)/usys.pl
	@$(MKDIR) $(@D)
	perl $< > $@

# 用户程序编译规则
$(USER_ELF): $(USER_OBJS) $(USER_OUTPUT)/usys.o
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ $^
	$(OBJCOPY) -O binary $@ $(USER_BIN)

$(USER_OUTPUT)/%.o: $(USER_DIR)/%.c
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

$(USER_OUTPUT)/usys.o: $(USER_OUTPUT)/usys.S
	$(CC) $(DEFS) $(USER_CFLAGS) -c -o $@ $<

$(USER_OUTPUT)/%.o: $(USER_DIR)/%.S
	$(CC) $(CFLAGS) $(USER_CFLAGS) -c -o $@ $<


# ====================== 这里用 objcopy -I binary 把二进制 ELF 文件转换成 ELF 对象文件。这样就能作为一个“对象模块”加入内核编译。 ======================
USER_ELF_OBJ = $(OUTPUT_PATH)/user_elf.o

$(USER_ELF_OBJ): $(USER_ELF)
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

# ====================== 运行与调试 ======================
.DEFAULT_GOAL := all
all: $(OUTPUT_PATH) $(USER_OUTPUT) $(ELF) $(USER_ELF)

run: all
	@echo "Press Ctrl-A then X to exit QEMU"
	@echo "----------------------------------"
	@qemu-system-riscv32 \
	    -machine virt \
	    -nographic \
	    -bios none \
	    -serial mon:stdio \
	    -kernel $(ELF) \
	    -device loader,file=$(USER_BIN),addr=0x80400000

debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@pkill qemu-system-riscv64 || true
	@$(QEMU) -nographic -machine virt -bios none -kernel $(ELF) \
		-device loader,file=$(USER_BIN),addr=0x80400000 \
		-gdb tcp::1234 -S &
	@sleep 1
	@$(GDB) $(ELF) -q -ex "set arch riscv:rv64" -ex "target remote :1234" -ex "b _start" -ex "c"


code: all
	@$(OBJDUMP) -S $(ELF) | less

# ====================== 清理 ======================
clean:
	@$(RM) $(OUTPUT_PATH)