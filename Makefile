TARGET_EXEC := get_vcore

CFLAGS := -std=c17 -D_POSIX_C_SOURCE=200809L -O3 -Werror -Wall -Wextra -Wpedantic -Wconversion -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -fno-stack-clash-protection -ftrapv -fstack-check -fvisibility=hidden

SRCS := main.c

BUILD_DIR := ./build
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
BIN := $(BUILD_DIR)/$(TARGET_EXEC)
SUM1 := $(BUILD_DIR)/$(TARGET_EXEC).sha1
SUM256 := $(BUILD_DIR)/$(TARGET_EXEC).sha256

all: $(SUM1) $(SUM256)

$(SUM1): $(BIN)
	cd $(dir $@) && sha1sum $(notdir $(BIN)) > $(notdir $(SUM1))

$(SUM256): $(BIN)
	cd $(dir $@) && sha256sum $(notdir $(BIN)) > $(notdir $(SUM256))

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
