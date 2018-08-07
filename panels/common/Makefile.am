# This is used in PANEL_CFLAGS
cappletname = common

noinst_LTLIBRARIES = liblanguage.la libdevice.la
noinst_PROGRAMS = list-languages

AM_CPPFLAGS =						\
	$(WARN_CFLAGS)					\
	$(PANEL_CFLAGS)					\
	$(LIBLANGUAGE_CFLAGS)				\
	$(DEVICES_CFLAGS)           \
	-DDATADIR=\""$(datadir)"\"			\
	-DUIDIR=\""$(pkgdatadir)/ui"\"			\
	-DLIBLOCALEDIR=\""$(prefix)/lib/locale"\"       \
	-DLOCALE_DIR=\""$(datadir)/locale"\"        \
	-DUM_PIXMAP_DIR=\""$(pkgdatadir)/pixmaps"\"

liblanguage_la_SOURCES =		\
	gdm-languages.h 		\
	gdm-languages.c 		\
	locarchive.h			\
	cc-common-language.c		\
	cc-common-language.h		\
	cc-language-chooser.c		\
	cc-language-chooser.h

liblanguage_la_LIBADD = 		\
	$(PANEL_LIBS)			    \
	$(LIBLANGUAGE_LIBS)

liblanguage_la_LDFLAGS = $(WARN_LDFLAGS) $(PANEL_LDFLAGS)

#libdevice
CSD_COMMON_ENUM_FILES = csd-common-enums.c csd-common-enums.h
csd-common-enums.h: csd-device-manager.h Makefile
	$(AM_V_GEN)($(GLIB_MKENUMS) \
			--fhead "#ifndef CSD_COMMON_ENUMS_H\n#define CSD_COMMON_ENUMS_H\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define CSD_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "G_END_DECLS\n\n#endif /* !CSD_COMMON_ENUMS_H */" \
		$(srcdir)/csd-device-manager.h > $@)
csd-common-enums.c: csd-device-manager.h Makefile csd-common-enums.h
	$(AM_V_GEN)($(GLIB_MKENUMS) \
			--fhead "#include \"csd-device-manager.h\"\n#include \"csd-common-enums.h\"\n" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(srcdir)/csd-device-manager.h > $@)
libdevice_la_SOURCES =          \
	csd-common-enums.c      \
	csd-common-enums.h      \
	csd-input-helper.c      \
	csd-input-helper.h      \
	csd-device-manager.c        \
	csd-device-manager.h        \
	csd-device-manager-x11.c    \
	csd-device-manager-x11.h
libdevice_la_LIBADD =           \
	$(DEVICES_LIBS)
if HAVE_UDEV
AM_CPPFLAGS += $(UDEV_CFLAGS)
libdevice_la_LIBADD += $(UDEV_LIBS)
libdevice_la_SOURCES +=         \
	csd-device-manager-udev.c   \
	csd-device-manager-udev.h
endif


list_languages_SOURCES = list-languages.c
list_languages_LDADD = liblanguage.la
list_languages_CFLAGS = $(LIBLANGUAGE_CFLAGS)

uidir = $(pkgdatadir)/ui

dist_ui_DATA = \
	language-chooser.ui

rulesdir = $(datadir)/polkit-1/rules.d/
rules_DATA = cinnamon-control-center.rules

EXTRA_DIST = $(rules_DATA)

-include $(top_srcdir)/git.mk
