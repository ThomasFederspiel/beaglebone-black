NOTSILENT := 1
#DEBUG := 1

ifeq ($(OS), Windows_NT)
    WINDOWS := 1
else
    LINUX := 1
endif

ifdef NOTSILENT
xS :=
else
S := @
endif

MODULE_NAME := $(lastword $(subst /, , $(CURDIR)))

OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src

ifdef DEBUG
	OBJ_DIR := $(OBJ_DIR)/debug
	BIN_DIR := $(BIN_DIR)/debug
	CXXFLAGS += -g
else
	OBJ_DIR := $(OBJ_DIR)/release
	BIN_DIR := $(BIN_DIR)/release
endif

LIB_DIR := $(BIN_DIR)/lib

MODULE_LIB_DIR := $(BASE_DIR)/$(LIB_DIR)
MODULE_SRC_DIR := $(BASE_DIR)/$(SRC_DIR)
MODULE_OBJ_DIR := $(BASE_DIR)/$(OBJ_DIR)/$(MODULE_NAME)

ifneq ($(APPL),)
	MODULE_BIN_DIR := $(BASE_DIR)/$(BIN_DIR)/$(MODULE_NAME)
else ifneq ($(PRU_APPL),)
	MODULE_BIN_DIR := $(BASE_DIR)/$(BIN_DIR)/$(MODULE_NAME)
endif



	