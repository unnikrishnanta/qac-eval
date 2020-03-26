#!/bin/bash
for FILE in *times.csv
do
    NAME=${FILE%.*}
    EXT=${FILE#*.} 
    DATE=`date +%d%m%y_%H%M`         
    NEWFILE=${NAME}_${DATE}.${EXT}
    mv -n ${FILE} ${NEWFILE}
done
