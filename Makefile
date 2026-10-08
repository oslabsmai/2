CC = cc
CFLAGS = -Wall -Wextra
BUILD = build

all: $(BUILD)/parent $(BUILD)/child

$(BUILD)/parent: parent/parent.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD)/child: child/child.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

run: all
	cd $(BUILD) && ./parent

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
