#Soubor:	Makefile
#Autor:		Marek Kuku�ka
#			xkukuc04@fit.vutbr.cz
#Datum:		03.03.2017
#Popis:		makefile pro proj1

NAME=proj2

all:
	latex $(NAME) .tex
	latex $(NAME) .tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log
