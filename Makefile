################################################################################
################################################################################
#
# TRS-80 Empire game makefile.
#
################################################################################
################################################################################

#
# Top-level make targets.
#

all: empire


#
# Build rules.
#

empire: empire.c
	gcc -lncurses -o empire empire.c

