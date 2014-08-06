#!/bin/sh

SO=./libfaketime.so
TEST=t/faketimetest
BACKWARD_OFFSET=1209600
ABSOLUTE_EPOCH=1356069600
ABSOLUTE_DATETIME="2013-1-21 14:00:01"
POSITIVE_OFFSET=2592000
NEGATIVE_OFFSET=604800
START_AT="2013-05-27 14:00:02"
DOMESTIC_CONFIG_FILE=$HOME/.faketime
DOMESTIC_CONFIG="+2068400"
GLOBAL_CONFIG_FILE=/tmp/.faketime
GLOBAL_CONFIG="@2014-1-1 13:59:24"

echo phase 0. backward compatibility test: offset should be $BACKWARD_OFFSET
echo env LD_PRELOAD=\"$SO\" FAKE_TIME=\"$BACKWARD_OFFSET\" $TEST 3
env LD_PRELOAD="$SO" FAKE_TIME="$BACKWARD_OFFSET" $TEST 3
echo

echo phase 1. absolute epoch test: should always be $ABSOLUTE_EPOCH
echo env LD_PRELOAD=\"$SO\" FAKE_TIME=\"$ABSOLUTE_EPOCH\" $TEST 3
env LD_PRELOAD="$SO" FAKE_TIME="$ABSOLUTE_EPOCH" $TEST 3
echo

echo phase 2. absolute date time test: should always be $ABSOLUTE_DATETIME
echo env LD_PRELOAD=\"$SO\" FAKE_TIME=\"$ABSOLUTE_DATETIME\" $TEST 3
env LD_PRELOAD="$SO" FAKE_TIME="$ABSOLUTE_DATETIME" $TEST 3
echo

echo phase 3. positive offset test: should be $POSITIVE_OFFSET seconds ahead
echo env LD_PRELOAD=\"$SO\" FAKE_TIME=\"+$POSITIVE_OFFSET\" $TEST 3
env LD_PRELOAD="$SO" FAKE_TIME="+$POSITIVE_OFFSET" $TEST 3
echo

echo phase 4. negative offset test: should be $NEGATIVE_OFFSET seconds behind
echo env LD_PRELOAD=\"$SO\" FAKE_TIME=\"-$NEGATIVE_OFFSET\" $TEST 3
env LD_PRELOAD="$SO" FAKE_TIME="-$NEGATIVE_OFFSET" $TEST 3
echo

echo phase 5. start at \"@\" test: system should start at $START_AT
echo env LD_PRELOAD="$SO" FAKE_TIME=\"@$START_AT\" $TEST 1
env LD_PRELOAD="$SO" FAKE_TIME="@$START_AT" $TEST 5
echo

echo phase 6. fake time from global config file $GLOBAL_CONFIG_FILE, whose contents is $GLOBAL_CONFIG
echo -n "$GLOBAL_CONFIG" > $GLOBAL_CONFIG_FILE
echo -n 'file contents '
cat "$GLOBAL_CONFIG_FILE"
echo
echo env LD_PRELOAD=\"$SO\" $TEST 15
env LD_PRELOAD="$SO" $TEST 15
rm -f "$GLOBAL_CONFIG_FILE"
echo

echo phase 7. fake time from domestic config file $DOMESTIC_CONFIG_FILE, whose contents is $DOMESTIC_CONFIG
echo -n "$DOMESTIC_CONFIG" > $DOMESTIC_CONFIG_FILE
echo -n 'domestic file contents '
cat "$DOMESTIC_CONFIG_FILE"
echo
echo env LD_PRELOAD=\"$SO\" $TEST 15
env LD_PRELOAD="$SO" $TEST 15
rm -f "$DOMESTIC_CONFIG_FILE"
echo

echo all tests done, but make sure with worker intelligence testing results are correct
