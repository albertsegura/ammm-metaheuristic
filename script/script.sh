#!/bin/bash

PROCS=8

NUM_DIGITS=4

START_SMALL=0
END_SMALL=750

START_BIG=200
END_BIG=200

GENERATE_SMALL=1
GENERATE_BIG=0

GRASP_SMALL=1
BRKGA_SMALL=1
GRASP_BIG=0
BRKGA_BIG=0

# Generate the small data
if [ $GENERATE_SMALL -eq 1 ]
then
	echo "Generating small data"
	for i in `seq $START_SMALL 10 $END_SMALL`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		../generator/InputGenerator $PROCS $NUM $NUM $NUM_DIGITS 0
	done
fi

# Run grasp small
if [ $GRASP_SMALL -eq 1 ]
then
	echo "Running GRASP small"
	for i in `seq $START_SMALL 10 $END_SMALL`;
	do
		NUM=$(./convert $i $NUM_DIGITS)
		{ /usr/bin/time -f "%e" ../grasp/greedy < Input$NUM.mh >> temp1.txt ; } 2>> temp1.txt
	done
	sed 'N;s/\n/ /' temp1.txt > grasp_small.txt
fi

# Run brkga small
if [ $BRKGA_SMALL -eq 1 ]
then
	echo "Running BRKGA small"
	g++ -c ../brkga/src/SampleDecoder.cpp
	for i in `seq $START_SMALL 10 $END_SMALL`;
	do
		sed -i '16s/.*/const unsigned n = '$i';/' ../brkga/src/samplecode.cpp
		g++ SampleDecoder.o -o samplecode_$i ../brkga/src/samplecode.cpp
		NUM=$(./convert $i $NUM_DIGITS)
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
		../generator/InputGenerator $PROCS $NUM $NUM $NUM_DIGITS 0
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
		{ /usr/bin/time -f "%e" ../grasp/greedy < Input$NUM.mh >> temp3.txt ; } 2>> temp3.txt
	done
	sed 'N;s/\n/ /' temp3.txt > grasp_big.txt
fi

# Run brkga big
if [ $BRKGA_BIG -eq 1 ]
then
	echo "Running BRKGA big"
	g++ -c ../brkga/src/SampleDecoder.cpp
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
