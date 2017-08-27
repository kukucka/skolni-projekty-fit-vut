#Soubor:	Makefile
#Autor:		Marek Kukuèka
#			xkukuc04@fit.vutbr.cz
#Datum:		16.04.2017
#Popis:		makefile pro proj4

NAME=proj4

all:
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t -sPAPERSIZE=a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
clean:
	rm -f $(NAME).aux $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg $(NAME).log
