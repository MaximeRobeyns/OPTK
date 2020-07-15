##
# Parameter Optimisation
# Copyright (c) 2020 Maxime Robeyns
#
# @file
# @version 0.1

PDF_ARTIFACTS = $(shell find . -maxdepth 1 -name 'docs.*' ! -name 'docs.texi' -type f)

pdf: clean
	@makeinfo --pdf -o build/pdf/docs.pdf docs.texi

html:
	@makeinfo --html --split section -o build/html/ docs.texi

clean:
	@rm -f ${PDF_ARTIFACTS}

# end
