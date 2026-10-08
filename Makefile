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

demo: all
	@cd $(BUILD) && ./parent < ../example/input.txt
	@echo "--- short.txt ---"; cat $(BUILD)/short.txt
	@echo "--- long.txt ---";  cat $(BUILD)/long.txt

clean:
	rm -rf $(BUILD)

.PHONY: all run demo clean
