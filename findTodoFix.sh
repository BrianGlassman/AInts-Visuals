#!/bin/bash

grep "TODO" ./* -r -n | grep --invert-match "_reference"
grep "FIXME" ./* -r -n | grep --invert-match "_reference"
