
source := $(wildcard *.tex)

deps := $(wildcard *.tex)
target := $(patsubst %.tex,%.pdf,$(source))

all: $(deps)
	
	
	
	for i in `seq 1 5` ; do\
		/usr/local/texlive/2017/bin/x86_64-linux/pdflatex -halt-on-error -shell-escape $(source); \
	done
	
.PHONY: clean
clean:		
	        rm $(target)

.PHONY: show
show:		
			evince $(target)
