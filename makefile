# ----------------------------
# Makefile Options
# ----------------------------

NAME = COOKIECE
ICON = icon.png
DESCRIPTION = "A Cookie Clicker port"
COMPRESSED = NO
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
