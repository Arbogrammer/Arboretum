CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter $(shell pkg-config --cflags gtk4)
LDLIBS += $(shell pkg-config --libs gtk4) -lm

.PHONY: all clean install-user uninstall-user

APP_ID := arboretum
MIME_TYPE := application-x-arboretum-bdg
USER_DATA_DIR ?= $(HOME)/.local/share

all: arboretum

arboretum: baumg.c baumg.h gtk4_compat.h $(wildcard *.c)
	$(CC) $(CFLAGS) baumg.c -o $@ $(LDLIBS)

clean:
	$(RM) arboretum

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
