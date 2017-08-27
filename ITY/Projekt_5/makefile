#Soubor:	Makefile
#Autor:		Marek Kukuèka
#Email:		xkukuc04@fit.vutbr.cz
#Datum:		01.05.2017
#Popis:		makefile pro proj5

NAME=proj5

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps

clean:
	rm -f $(NAME).aux $(NAME).log $(NAME).dvi $(NAME).ps $(NAME).out $(NAME).snm $(NAME).toc $(NAME).out $(NAME).nav
