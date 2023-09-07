BINARY := pic
DEFS := -DSTM32F1 -DBLUEPILL
LDSCRIPT := stm32f103x8.ld
OPENCM3_DIR := ../libopencm3
OPENCM3_LIB := opencm3_stm32f1
SRC_DIRS := sources
INCLUDE_DIR := sources
ADD_FLAGS := -fpic -mpic-register=r10 -msingle-pic-base -mno-pic-data-is-text-relative

include Makefile.inc
