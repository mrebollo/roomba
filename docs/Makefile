# Makefile for Roomba Documentation
#
# Generates API reference (Doxygen) and PDF Manuals (LaTeX)

.PHONY: all api user developer clean help

all: api user developer
	@echo "All documentation generated successfully"

# API Documentation (HTML via Doxygen)
api:
	@echo "Generating API documentation (Doxygen)..."
	@if [ -f Doxyfile ]; then \
		doxygen Doxyfile > /dev/null 2>&1; \
		echo "[OK] API documentation generated in html/"; \
	else \
		echo "[WARN] Doxyfile not found"; \
	fi

# User Manual (PDF via LaTeX)
user:
	@echo "Generating User Manual PDF..."
	@if [ -f user/manual_usuario.tex ]; then \
		cd user && pdflatex manual_usuario.tex > /dev/null 2>&1 && \
		pdflatex manual_usuario.tex > /dev/null 2>&1 && \
		rm -f *.aux *.log *.out *.toc *.fdb_latexmk *.fls *.synctex.gz *.bbl *.blg *.idx *.ilg *.ind *.lof *.lot; \
		echo "[OK] User manual generated: user/manual_usuario.pdf"; \
	else \
		echo "[WARN] User manual .tex file not found"; \
	fi

# Developer Manual (PDF via LaTeX)
developer:
	@echo "Generating Developer Manual PDF..."
	@if [ -f developer/manual_desarrollador.tex ]; then \
		cd developer && pdflatex manual_desarrollador.tex > /dev/null 2>&1 && \
		pdflatex manual_desarrollador.tex > /dev/null 2>&1 && \
		rm -f *.aux *.log *.out *.toc *.fdb_latexmk *.fls *.synctex.gz *.bbl *.blg *.idx *.ilg *.ind *.lof *.lot; \
		echo "[OK] Developer manual generated: developer/manual_desarrollador.pdf"; \
	else \
		echo "[WARN] Developer manual .tex file not found"; \
	fi

# Clean all documentation artifacts
clean:
	@echo "Cleaning documentation artifacts..."
	rm -rf html/
	rm -f user/manual_usuario.pdf
	rm -f developer/manual_desarrollador.pdf
	# Clean any leftover LaTeX temp files just in case
	rm -f user/*.aux user/*.log user/*.out user/*.toc
	rm -f developer/*.aux developer/*.log developer/*.out developer/*.toc
	@echo "[OK] Documentation cleaned"

help:
	@echo "Roomba Documentation Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make all       - Generate everything (API + PDFs)"
	@echo "  make api       - Generate Doxygen HTML documentation"
	@echo "  make user      - Generate User Manual PDF"
	@echo "  make developer - Generate Developer Manual PDF"
	@echo "  make clean     - Remove generated files"
