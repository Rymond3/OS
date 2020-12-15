#! /bin/bash

MPOINT="./mount-point"
TMP="./temp"
BLOCKSIZE=4096

#a
rm -rf temp
mkdir temp

cp src/fuseLib.c $MPOINT/file1.txt
cp src/fuseLib.c $TMP/file1.txt

cp src/myFS.h $MPOINT/file2.txt
cp src/myFS.h $TMP/file2.txt

#b
./my-fsck virtual-disk

diff -q src/fuseLib.c $MPOINT/file1.txt
diff -q src/myFS.h $MPOINT/file2.txt

truncate -s -$BLOCKSIZE $TMP/file1.txt
truncate -s -$BLOCKSIZE $MPOINT/file1.txt

#c
./my-fsck virtual-disk

diff -q src/fuseLib.c $MPOINT/file1.txt

#d
cp src/myFS.c $MPOINT/file3.txt

#e
./my-fsck virtual-disk

diff -q src/myFS.c $MPOINT/file3.txt

#f
truncate -s +$BLOCKSIZE $TMP/file2.txt
truncate -s +$BLOCKSIZE $MPOINT/file2.txt

#g
./my-fsck virtual-disk

diff -q src/myFS.h $MPOINT/file2.txt
