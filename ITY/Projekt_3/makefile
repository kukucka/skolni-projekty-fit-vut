#Soubor:	Makefile
#Autor:		Marek Kukuèka
#			xkukuc04@fit.vutbr.cz
#Datum:		03.03.2017
#Popis:		makefile pro proj3

NAME=proj3

all:
	latex $(NAME) .tex
	latex $(NAME) .tex
	dvips -sPAPERSIZE=a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log
