#! /bin/bash

DIRTMP=tmp
DIROUT=out
FILE1=file1.txt
FILE2=file2.txt
FILE3=file3.dat
TAR_FILE=filetar.mtar

if [ -e ./mytar ]; then 
	if [ -x ./mytar ]; then
		if [ -d $DIRTMP ]; then
			rm -rf $DIRTMP
		fi
		mkdir $DIRTMP 
		cd $DIRTMP
		
		if [ ! -e $FILE1 ]; then
			touch $FILE1
			echo "Hello World!" > $FILE1
		fi

		if [ ! -e $FILE2 ]; then
			touch $FILE2
			head -10 /etc/passwd > $FILE2
		fi

		if [ ! -e $FILE3 ]; then
			touch $FILE3
			head -c 1024 /dev/urandom > $FILE3
		fi

		../mytar -cf $TAR_FILE $FILE1 $FILE2 $FILE3


		if [ -d $DIROUT ]; then
			rm -rf $DIROUT
		fi
		mkdir $DIROUT 

		cp $TAR_FILE $DIROUT

		cd $DIROUT
		../../mytar -xf $TAR_FILE

		diff ../$FILE1 $FILE1

		if [ $? = 0 ]; then
			echo "$FILE1 identical"
			diff ../$FILE2 $FILE2
			
			if [ $? = 0 ]; then
				echo "$FILE2 identical"
				diff ../$FILE3 $FILE3

				if [ $? = 0 ]; then
					echo "$FILE3 identical"
					cd ../..
					echo "Success"
					exit 0
				else
					echo "$FILE3 different"
					    cd ../..
					exit 1
                		fi
			else
				echo "$FILE2 is different"
				cd ../..
				exit 1
			fi
		else
			echo "$FILE1 is different"
			cd ../..
			exit 1
		fi
			
	else
		echo "mytar file is not an executable"
		exit 1
	fi
else
	echo "mytar file does not exist."
	exit 1
fi
