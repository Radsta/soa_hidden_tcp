TEXFILES = user-stat.tex
HIGHLIGHT_OPTS = -L -l -f -t 8 -K tiny -S bash -c highlight.sty

all: $(TEXFILES)

user-stat.tex: user-stat.txt
	highlight $(HIGHLIGHT_OPTS) -i $< -o $@

.PHONY: clean
clean:
	rm -f $(TEXFILES) highlight.sty *~
