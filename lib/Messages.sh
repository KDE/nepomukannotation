#! /usr/bin/env bash
$EXTRACTRC gui/*.ui >> rc.cpp
$XGETTEXT  *.cpp gui/*.cpp -o $podir/nepomukannotation.pot
