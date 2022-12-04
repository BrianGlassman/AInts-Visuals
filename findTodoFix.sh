#!/bin/bash

# grep "CHAMBERCONNECT" ./* -r -n | grep --invert-match "_reference" # Probably won't do this, but worth tracking anyway
# grep "NORELEASE" ./* -r -n | grep --invert-match "_reference" # Don't want it clogging up the count for now
grep "TODO" ./* -r -n | grep --invert-match "_reference"
grep "FIXME" ./* -r -n | grep --invert-match "_reference"
