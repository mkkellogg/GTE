#!/bin/bash

OUTPUTDIR=output
for SRC in "$@"
do
	FILENAME="${SRC##*/}"
	BASENAME="${FILENAME%.*}"
	DEPFILE=$OUTPUTDIR/$BASENAME.d
	OBJFILE=$OUTPUTDIR/$BASENAME.o
	if [ -a "$OBJFILE" ]; then
		if [ ! -a "$OBJFILE" ] || [ $DEP -ot $SRC ]; then
			g++ -MM -Isrc -std=c++11 -c $SRC | sed -e ':a' -e 'N' -e '$!ba' -e 's/\n//g' -e 's/\\//g' > $DEPFILE
		fi
		DEPS=`cat $DEPFILE`
		ENTRY=0
		for DEP in $DEPS
		do
			if((ENTRY > 0)); then
				if [ $OBJFILE -ot $DEP ]; then
					rm $OBJFILE
					break
				fi
			fi
			ENTRY=$((ENTRY+1))
		done
	fi
done

