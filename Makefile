TARGET_EXEC := main

CFLAGS := -std=c17 -D_POSIX_C_SOURCE=200809L -O3 -Werror -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -fno-stack-clash-protection -ftrapv -fstack-check -fvisibility=hidden

SRCS := main.c

BUILD_DIR := ./build
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
