.PHONY: all clean
PARENT_DIR := $(notdir $(realpath ..))
OUTPUT_DIR := build
MAIN_TEX   := kirb3.tex
MAIN_PDF   := ../Kirb-3_User_Manual.pdf

all: $(MAIN_PDF)

$(MAIN_PDF): $(MAIN_TEX)
	@set -e; \
	echo "PARENT_DIR=$(PARENT_DIR)"; \
	echo "Building $< -> $(MAIN_PDF)"; \
	mkdir -p "$(OUTPUT_DIR)"; \
	latexmk -outdir="$(OUTPUT_DIR)" -pdf -jobname="$(PARENT_DIR)" -use-make "$<"; \
	test -f "$(OUTPUT_DIR)/$(PARENT_DIR).pdf"; \
	mv "$(OUTPUT_DIR)/$(PARENT_DIR).pdf" "$(MAIN_PDF)"; \
	echo "Wrote $(MAIN_PDF)"

clean:
	@latexmk -C -outdir="$(OUTPUT_DIR)"
	@rm -rf "$(OUTPUT_DIR)"
