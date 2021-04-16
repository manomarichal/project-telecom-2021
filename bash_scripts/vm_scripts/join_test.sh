echo "connecting to localhost 10003 and executing join on 244.4.4.4"

echo "write client21/igmp.join 244.4.4.4" | telnet localhost 10003

sleep 5

echo "write client21/igmp.leave 244.4.4.4" | telnet localhost 10003


