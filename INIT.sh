#!bin/bash

DIRECTORY1="Gammas"
DIRECTORY2="Stored"
DIRECTORY3="Stored/Bad_Gammas"
DIRECTORY4="Gamma_Single_Cs"
DIRECTORY5="Gamma_Double_Cs"
DIRECTORY6="OutputFolder"
DIRECTORY7="d0s"
DIRECTORY8="d0s_4"
DIRECTORY9="d0_Folder"
DIRECTORY10="ComptonHists"


if [ ! -d "$DIRECTORY1" ]
then
	mkdir $DIRECTORY1
fi

if [ ! -d "$DIRECTORY2" ]
then
	mkdir $DIRECTORY2
fi

if [ ! -d "$DIRECTORY3" ]
then
	mkdir $DIRECTORY3
fi

if [ ! -d "$DIRECTORY4" ]
then
	mkdir $DIRECTORY4
fi

if [ ! -d "$DIRECTORY5" ]
then
	mkdir $DIRECTORY5
fi
if [ ! -d "$DIRECTORY6" ]
then
    mkdir $DIRECTORY6
fi


if [ ! -d "$DIRECTORY7" ]
then
    mkdir $DIRECTORY7

    for i in {0..599}
    do
        mkdir d0s/d0_${i}
        echo d0s/d0_${i} created
    done
fi

if [ ! -d "$DIRECTORY8" ]
then
    mkdir $DIRECTORY8
    for i in {0..599}
    do
        if [ $(($i % 4)) -eq 0 ]
        then
            mkdir d0s_4/d0_${i}
            echo d0s_4/d0_${i} created
        fi
    done
fi

if [ ! -d "$DIRECTORY9" ]
then
    mkdir $DIRECTORY9
    for k in {1..5}
    do
        mkdir d0_Folder/d0s_${k}_4
        for i in {0..599}
        do
            if [ $(($i % 4)) -eq 0 ]
            then
                mkdir d0_Folder/d0s_${k}_4/d0_${i}
                echo d0_Folder/d0s_${k}_4/d0_${i} created
            fi
        done
    done
fi


if [ ! -d "$DIRECTORY10" ]
then
	mkdir $DIRECTORY10
fi