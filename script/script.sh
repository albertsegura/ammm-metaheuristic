#!/bin/bash

PROCS=8

NUM_DIGITS=3

START_SMALL=1
END_SMALL=150

START_BIG=500
END_BIG=500

BIN_GRASP='echo '
BIN_BRKGA='echo '

GENERATE_SMALL=0
GENERATE_BIG=1

GRASP_SMALL=0
BRKGA_SMALL=0
GRASP_BIG=0
BRKGA_BIG=1

# Generate the small data
if [ $GENERATE_SMALL -eq 1 ]
then
	echo "Generating small data"
	for i in `seq $START_SMALL $END_SMALL`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		../generator/InputGenerator $PROCS $NUM_INST $NUM $NUM_DIGITS 0
	done
fi

# Run grasp small
if [ $GRASP_SMALL -eq 1 ]
then
	echo "Running GRASP small"
	for i in `seq $START_SMALL $END_SMALL`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		{ /usr/bin/time -f "%e" $BIN_GRASP $NUM >> temp1.txt ; } 2>> temp1.txt
	done
	sed 'N;s/\n/ /' temp1.txt > grasp_small.txt
fi

# Run brkga small
if [ $BRKGA_SMALL -eq 1 ]
then
	echo "Running BRKGA small"
	g++ -c ../brkga/src/SampleDecoder.cpp
	for i in `seq $START_SMALL $END_SMALL`;
	do
		sed -i '16s/.*/const unsigned n = '$i';/' ../brkga/src/samplecode.cpp
		g++ SampleDecoder.o -o samplecode_$i ../brkga/src/samplecode.cpp
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		{ /usr/bin/time -f "%e" ./samplecode_$i < Input$NUM.mh >> temp2.txt ; } 2>> temp2.txt
	done
	sed 'N;s/\n/ /' temp2.txt > brkga_small.txt
fi

# Generate the big data
if [ $GENERATE_BIG -eq 1 ]
then
	echo "Generating big data"
	for i in `seq $START_BIG $END_BIG`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		../generator/InputGenerator $PROCS $NUM_INST $NUM $NUM_DIGITS 0
	done
fi

# Run grasp big
if [ $GRASP_BIG -eq 1 ]
then
	echo "Running GRASP big"
	for i in `seq $START_BIG $END_BIG`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		{ /usr/bin/time -f "%e" $BIN_GRASP $NUM >> temp3.txt ; } 2>> temp3.txt
	done
	sed 'N;s/\n/ /' temp3.txt > grasp_big.txt
fi

# Run brkga big
if [ $BRKGA_BIG -eq 1 ]
then
	echo "Running BRKGA big"
	for i in `seq $START_BIG $END_BIG`;
	do
		sed -i '16s/.*/const unsigned n = '$i';/' ../brkga/src/samplecode.cpp
		g++ SampleDecoder.o -o samplecode_$i ../brkga/src/samplecode.cpp
		NUM=$(./convert $i $NUM_DIGITS)
		NUM_INST=$NUM
		{ /usr/bin/time -f "%e" ./samplecode_$i < Input$NUM.mh >> temp2.txt ; } 2>> temp4.txt
	done
	sed 'N;s/\n/ /' temp4.txt > brkga_big.txt
fi
