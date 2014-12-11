#!/bin/bash

PROCS=8

NUM_DIGITS=3

START_SMALL=1
END_SMALL=5

START_BIG=10
END_BIG=15

# Generate the small data
for i in `seq $START_SMALL $END_SMALL`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	../generator/InputGenerator $PROCS $NUM_INST $NUM $NUM_DIGITS 0
done

# Generate the big data
for i in `seq $START_BIG $END_BIG`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	../generator/InputGenerator $PROCS $NUM_INST $NUM $NUM_DIGITS 0
done

# Run grasp small
echo "Running GRASP small"
for i in `seq $START_SMALL $END_SMALL`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	/usr/bin/time -f "%e" sleep 2
done

# Run brkga small
echo "Running BRKGA small"
for i in `seq $START_SMALL $END_SMALL`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	/usr/bin/time -f "%e" sleep 3
done

# Run grasp big
echo "Running GRASP big"
for i in `seq $START_BIG $END_BIG`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	/usr/bin/time -f "%e" sleep 2
done

# Run brkga big
echo "Running BRKGA big"
for i in `seq $START_BIG $END_BIG`;
do
	NUM=$(./convert $i $NUM_DIGITS)
	NUM_INST=$NUM
	/usr/bin/time -f "%e" sleep 3
done
