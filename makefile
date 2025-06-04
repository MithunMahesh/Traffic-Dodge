# Makefile

NAME = CarGame
Descrption = "Car Game"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz
include $(shell cedev-config --makefile)
