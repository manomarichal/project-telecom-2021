#!/bin/sh

cd /home/student/click/scripts/
../userlevel/click glue.click &
cd /home/student/click-reference/solution/

sleep 1

./router.bin &
./server.bin &
./client21.bin &
./client22.bin &
./client31.bin &
./client32.bin &

wait