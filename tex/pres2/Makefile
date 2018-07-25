empty :=
space := $(empty) $(empty)

svgs := $(wildcard *.svg)
pngs := $(patsubst %.svg,%.png,$(svgs))
pngs := $(subst $(space),_,$(pngs))
pngs := $(subst png_,png ,$(pngs))
target := presentation_1.pdf
ttext := $(patsubst %.pdf,%.tex,$(target))
all: $(target)

.PHONY: show
show:
	evince $(target) 2> /dev/null &

.PHONY: clean
clean:		
	        rm $(target)
%.png: %.svg
	./convert.sh

.PHONY: echo
echo:
	echo $(pngs)

$(target): $(ttext)
	for i in `seq 1 2` ; do\
	 	pdflatex -shell-escape $(ttext); \
	done
