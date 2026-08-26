.PHONY: build clean flash size

BUILD_DIR = build

all: build

build:
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release .
	@cmake --build $(BUILD_DIR) --parallel $$(nproc)

clean:
	@rm -rf $(BUILD_DIR)

size: build
	@arm-none-eabi-size $(BUILD_DIR)/exarcia-demo-mcu-stm32cube.elf
