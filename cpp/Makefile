# SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)
OUTPUT := build
CXX := clang++
INCLUDES := -Iinclude -Isrc
CXX_IGNORED_WARNINGS := -Wno-gnu-zero-variadic-macro-arguments -Wno-nested-anon-types \
			-Wno-gnu-anonymous-struct -Wno-variadic-macros \
			-Wno-zero-length-array -Wno-c99-designator
CFLAGS := -g -Wall
CXXFLAGS := $(CFLAGS) -Wextra -Wpedantic -Werror $(CXX_IGNORED_WARNINGS) -std=gnu++20
ALL_LDFLAGS := $(LDFLAGS) $(EXTRA_LDFLAGS)
TEST_LDFLAGS := $(ALL_LDFLAGS) -lgtest

TESTS = Test
OBJS =
TEST_OBJS = TestTask

ifeq ($(V),1)
	Q =
	msg =
else
	Q = @
	msg = @printf '  %-8s %s%s\n'					\
		      "$(1)"						\
		      "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))"	\
		      "$(if $(3), $(3))";
	MAKEFLAGS += --no-print-directory
endif

.PHONY: all
all: $(TESTS)

.PHONY: clean
clean:
	$(call msg,CLEAN)
	$(Q)rm -rf $(OUTPUT) $(TESTS)

$(OUTPUT) $(OUTPUT)/tests:
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@

$(patsubst %,$(OUTPUT)/%.o,$(OBJS)): $(wildcard src/*.cpp) $(wildcard src/*.hpp) \
				     $(wildcard include/*.hpp) | $(OUTPUT)
	$(call msg,CXX,$@)
	$(Q)$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(filter src/%.cpp,$^) -o $@

$(patsubst %,$(OUTPUT)/tests/%.o,$(TEST_OBJS)): $(wildcard tests/*.cpp) $(wildcard tests/*.hpp) \
						$(wildcard include/*.hpp) $(patsubst %,$(OUTPUT)/%.o,$(OBJS)) \
						| $(OUTPUT)/tests
	$(call msg,CXX,$@)
	$(Q)$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(filter tests/%.cpp,$^) -o $@

$(TESTS): %: $(patsubst %,$(OUTPUT)/tests/%.o,$(TEST_OBJS)) | $(OUTPUT)/tests
	$(call msg,BINARY,$@)
	$(Q)$(CXX) $(CXXFLAGS) $^ $(patsubst %,$(OUTPUT)/%.o,$(OBJS)) $(TEST_LDFLAGS) -o $@

.DELETE_ON_ERROR:
.SECONDARY:
