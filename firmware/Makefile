test_port:
	@if [ -z "$(PORT)" ]; then echo "PORT is not set, please set PORT using export, example: 'export PORT=/dev/ttyACM0'"; exit 1; fi

test_minino_path:
	@if [ -z "$(MININO_PATH)" ]; then echo "MININO_PATH is not set, please set MININO_PATH using export, example: 'export MININO_PATH=/path/to/minino/firmware'"; exit 1; fi

setup:
	make test_minino_path
	idf.py set-target esp32c6
	pre-commit install

compile:
	make test_minino_path
	idf.py build

flash:
	make test_port
	idf.py -p $(PORT) flash

format:
	make test_port
	idf.py -p $(PORT) erase-flash

monitor:
	make test_port
	idf.py -p $(PORT) monitor

clean:
	rm -rf build/ managed_components/
	rm sdkconfig
	make setup

all: flash monitor

.PHONY: test_port test_minino_path setup compile flash monitor clean all

ff:	format all