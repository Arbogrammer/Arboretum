CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter $(shell pkg-config --cflags gtk4)
LDLIBS += $(shell pkg-config --libs gtk4) -lm
SANITIZER_FLAGS := -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all clean test test-core test-visual visual-reference security-parser-test parser-fuzz parser-fuzz-prepare bdg-testdata deep-test-file install-user uninstall-user

APP_ID := arboretum
MIME_TYPE := application-x-arboretum-bdg
USER_DATA_DIR ?= $(HOME)/.local/share

all: arboretum

test: arboretum editing-test layout-test security-parser-test-bin deep-tree-generator bdg-testdata visual-test
	xvfb-run -a -s "-screen 0 1280x1024x24 -dpi 96" python3 packaging/test-suite.py

test-core: arboretum editing-test layout-test security-parser-test-bin deep-tree-generator bdg-testdata
	xvfb-run -a python3 packaging/test-suite.py --core-only

visual-test: visual_test.c baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) visual_test.c -o $@ $(LDLIBS)

test-visual: visual-test
	xvfb-run -a -s "-screen 0 1280x1024x24 -dpi 96" python3 packaging/test-visual.py

visual-reference: visual-test
	xvfb-run -a -s "-screen 0 1280x1024x24 -dpi 96" python3 packaging/test-visual.py --update

editing-test: editing_test.c baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) editing_test.c -o $@ $(LDLIBS)

layout-test: layout_test.c baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) layout_test.c -o $@ $(LDLIBS)

arboretum: baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) baumg.c -o $@ $(LDLIBS)

security-parser-test: security-parser-test-bin bdg-testdata
	ASAN_OPTIONS=detect_leaks=0 ./security-parser-test-bin

security-parser-test-bin: security_parser_test.c baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) security_parser_test.c -o $@ $(LDLIBS) $(SANITIZER_FLAGS)

parser-fuzz: parser_fuzz.c
	clang-21 $(CFLAGS) -O1 -g -fsanitize=fuzzer,address,undefined \
		-fno-omit-frame-pointer parser_fuzz.c -o parser-fuzz $(LDLIBS)

parser-fuzz-prepare: parser-fuzz deep-tree-generator
	mkdir -p /tmp/arboretum-fuzz-corpus /tmp/arboretum-fuzz-findings
	./deep-tree-generator /tmp/arboretum-fuzz-corpus/minimal.bdg 1

bdg-testdata: bdg_testdata_generator.c
	$(CC) $(CFLAGS) bdg_testdata_generator.c -o bdg-testdata-generator $(shell pkg-config --libs glib-2.0)
	./bdg-testdata-generator testdaten/bdg

deep-tree-generator: deep_tree_generator.c
	$(CC) $(CFLAGS) deep_tree_generator.c -o deep-tree-generator $(shell pkg-config --libs glib-2.0)

deep-test-file: deep-tree-generator
	./deep-tree-generator /tmp/arboretum-deep-5000.bdg 5000

clean:
	$(RM) arboretum security-parser-test security-parser-test-bin layout-test editing-test visual-test parser-fuzz deep-tree-generator bdg-testdata-generator

install-user: arboretum arboretum.desktop arboretum-icon.png $(MIME_TYPE).xml
	install -Dm644 arboretum.desktop $(USER_DATA_DIR)/applications/$(APP_ID).desktop
	install -Dm644 $(MIME_TYPE).xml $(USER_DATA_DIR)/mime/packages/$(MIME_TYPE).xml
	install -Dm644 arboretum-icon.png $(USER_DATA_DIR)/icons/hicolor/512x512/apps/$(APP_ID).png
	install -Dm644 arboretum-icon.png $(USER_DATA_DIR)/icons/hicolor/512x512/mimetypes/$(MIME_TYPE).png
	update-mime-database $(USER_DATA_DIR)/mime
	update-desktop-database $(USER_DATA_DIR)/applications
	gtk-update-icon-cache -f -t $(USER_DATA_DIR)/icons/hicolor
	xdg-mime default $(APP_ID).desktop application/x-arboretum-bdg

uninstall-user:
	$(RM) $(USER_DATA_DIR)/applications/$(APP_ID).desktop
	$(RM) $(USER_DATA_DIR)/mime/packages/$(MIME_TYPE).xml
	$(RM) $(USER_DATA_DIR)/icons/hicolor/512x512/apps/$(APP_ID).png
	$(RM) $(USER_DATA_DIR)/icons/hicolor/512x512/mimetypes/$(MIME_TYPE).png
	update-mime-database $(USER_DATA_DIR)/mime
	update-desktop-database $(USER_DATA_DIR)/applications
	gtk-update-icon-cache -f -t $(USER_DATA_DIR)/icons/hicolor
