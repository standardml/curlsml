

all: smlcurl fetchurl

smlcurl:
	mlton \
	  -link-opt -lcurl \
	  -cc-opt -Wall \
	  -cc-opt -Wextra \
	  smlcurl.mlb \
	  smlcurl.c 

fetchurl:
	mlton \
	  -link-opt -lcurl \
	  -cc-opt -Wall \
	  -cc-opt -Wextra \
	  fetchurl.mlb \
	  smlcurl.c 

.PHONY: all smlcurl fetchurl
