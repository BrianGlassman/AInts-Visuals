#!/bin/bash

# grep "CHAMBERCONNECT" ./* -r -n # Probably won't do this, but worth tracking anyway
# grep "NORELEASE" ./* -r -n # Don't want it clogging up the count for now
# grep "OPTIM" ./* -r -n
grep "TODO" ./* -r -n
grep "FIXME" ./* -r -n
