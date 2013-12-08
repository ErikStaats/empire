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

empire: empire.c grain.c investments.c population.c
	gcc -g -lncurses -o empire empire.c grain.c investments.c population.c

