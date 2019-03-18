# Based on latex, bibtex, makeindex, pandoc, dvips, ps2pdf13, sed, convert (ImageMagick)
TESEONAMEDOC=Teseo2UserManual
NAME=$(TESEONAMEDOC)

# make dvi is default
dvi :  $(NAME).eps.tex $(NAME).dvi

ps : $(NAME).ps

pdf : $(NAME).pdf

markdown : $(NAME).md

css : $(NAME).css

html : $(NAME).html

all : ps pdf markdown html

rebuild : clean_all all

$(NAME).dvi : $(NAME).eps.tex $(NAME).tex $(NAME).bbl $(NAME).ind
	latex -jobname=$(NAME) $(NAME).eps.tex

$(NAME).eps.tex : pngtoeps  $(NAME).tex
	sed -e "s/\.png/\.eps/g" $(NAME).tex > $(NAME).eps.tex

$(NAME).ps : $(NAME).dvi
	dvips $(NAME).dvi -o $(NAME).ps

$(NAME).pdf : $(NAME).ps
	ps2pdf13 $(NAME).ps $(NAME).pdf

$(NAME).bbl : $(NAME).bib
	latex -jobname=$(NAME) $(NAME).eps.tex
	latex -jobname=$(NAME) $(NAME).eps.tex
	bibtex $(NAME)
	latex -jobname=$(NAME) $(NAME).eps.tex

$(NAME).ind : $(NAME).idx
	makeindex $(NAME).idx

pngtoeps:
	cd images; ls *.png | sed -e "s/\(.*\)\.png/if [ ! -e \1\.eps ]; then convert -quality 100 \1\.png \1\.eps; fi/g" > script_convert.sh; /bin/sh --verbose script_convert.sh; cd ..

epstopng:
	cd images; ls *.eps | sed -e "s/\(.*\)\.eps/if [ ! -e \1\.png ]; then convert -quality 100 \1\.eps \1\.png; fi/g" > script_convert.sh; /bin/sh --verbose script_convert.sh; cd ..

$(NAME).md : $(NAME).tex
	sed -e "s/\.eps/\.png/g" $(NAME).tex > $(NAME).md.tex
	pandoc -f latex -t markdown -s $(NAME).md.tex -o $(NAME).tmp.md
	# Init output markdown file
	echo "# Teseo2: Turn the Eldest Seismograms into the Electronic Original Ones" > $(NAME).md
	# Some clean and fixes
	sed \
		-e 's/[[:space:]][[:space:]]*------[-]*/\`\`\`/' \
		-e 's/<[\/]*span>//g' \
		-e 's/{#[^}][^}]*}//g' \
		$(NAME).tmp.md >> $(NAME).md

$(NAME).css :
	wget https://cdnjs.cloudflare.com/ajax/libs/github-markdown-css/3.0.1/github-markdown.min.css -O $(NAME).css
	# Add custom .img-center
	echo '.img-center { display: block; margin-left: auto; margin-right: auto; max-width: 100%; }' >> $(NAME).css

$(NAME).html : $(NAME).css
	sed -e "s/\.eps/\.png/g" $(NAME).tex > $(NAME).html.tex
	pandoc -c $(NAME).css -f latex -t html -s $(NAME).html.tex -o $(NAME).tmp.html
	sed -e "s/<img /<img class="img-center" /g" $(NAME).tmp.html > $(NAME).html

clean_all : clean
	rm -f \
		$(NAME).ps \
		$(NAME).pdf \
		$(NAME).html \
		$(NAME).css \
		$(NAME).md

clean :
	rm -f \
		$(NAME).aux \
		$(NAME).idx \
		$(NAME).ilg \
		$(NAME).ind \
		$(NAME).eps.tex \
		$(NAME).lof \
		$(NAME).log \
		$(NAME).lot \
		$(NAME).toc \
		$(NAME).low \
		$(NAME).bbl \
		$(NAME).blg \
		$(NAME).dvi \
		$(NAME).lol \
		$(NAME)pdf.* \
		$(NAME).tmp.md \
		$(NAME).md.tex \
		$(NAME).html.tex \
		$(NAME).tmp.html \
		images/script_convert.sh \
		images/figure_III.2.eps \
		images/figure_III.3.eps \
		images/snapshot1.eps \
		images/snapshot12.eps \
		images/snapshot13.eps \
		images/snapshot14.eps \
		images/snapshot15.eps \
		images/snapshot16.eps \
		images/snapshot2.eps \
		images/snapshot3.eps \
		images/snapshot4.eps \
		images/snapshot6.eps \
		images/snapshot7.eps \
		images/snapshot8.eps \
		images/snapshot9.eps \
		*~

