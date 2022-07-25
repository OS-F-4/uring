#!/bin/sh
./web &

nc  127.0.0.1 8000 < ./test.txt
