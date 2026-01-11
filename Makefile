.DEFAULT_GOAL := all
CFLAGS = -std=gnu11 -g -Og -Wall -Iinclude -Iinclude/core

# collect C sources from project `src/` and downloaded `src/core/`
# Use deferred expansion so the `download-core` step can populate `src/core/`
# before the wildcard is evaluated.
SRCS = $(wildcard src/*.c src/core/*.c)
TARGET = endpoint

# fetch core sources from IoTFoundry template and place them under `core/` and `include/core/`
# Repository to pull from (owner/repo) and branch
CORE_REPO=PICMG/iot-foundry-endpoint
CORE_BRANCH=main
CORE_URL=https://raw.githubusercontent.com/$(CORE_REPO)/$(CORE_BRANCH)

# Do not hardcode remote filenames; use wildcard-based retrieval.
# The download recipe clones the remote repo and copies
# `src/*` -> `src/core/` and `include/*` -> `include/core/`.
download-core:
	mkdir -p src/core
	mkdir -p include/core
	# Download repository tarball and extract relevant subfolders (wildcard behavior)
	TMPDIR=$$(mktemp -d) && \
	wget -q -O $$TMPDIR/repo.tar.gz https://github.com/$(CORE_REPO)/archive/refs/heads/$(CORE_BRANCH).tar.gz || (rm -rf $$TMPDIR; exit 1); \
	mkdir -p $$TMPDIR/repo && \
	tar -xzf $$TMPDIR/repo.tar.gz -C $$TMPDIR/repo --strip-components=1 || (rm -rf $$TMPDIR; exit 1); \
	cp -a $$TMPDIR/repo/src/. src/core/ 2>/dev/null || true; \
	cp -a $$TMPDIR/repo/include/. include/core/ 2>/dev/null || true; \
	rm -rf $$TMPDIR

.PHONY: download-core platform_build

platform_build: download-core $(TARGET)
	@echo "Built $(TARGET) from: $(SRCS)"
.PHONY: all build clean flash gdb

all: download-core platform_build

$(TARGET): download-core
	# expand sources at recipe time so downloaded core files are included
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(shell echo src/*.c src/core/*.c)

clean:
	rm -f $(TARGET) *.o 
