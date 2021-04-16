echo "connecting to localhost 10003 and executing join on 225.1.1.1"

echo "write client21/igmp.join 225.1.1.1" | telnet localhost 10003

sleep 7

echo "write client21/igmp.leave 225.1.1.1" | telnet localhost 10003


