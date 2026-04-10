#!/bin/bash
BASE=`dirname $0`

source ~/.venv/bin/activate
source ~/env.sh
python file_inspector.py $1 $2

rm -rf $BASE
