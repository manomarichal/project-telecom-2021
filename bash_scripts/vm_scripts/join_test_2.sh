echo "connecting to localhost 10003 and executing join on 225.1.1.1"

echo "write client31/igmp.join 225.1.1.1" | telnet localhost 10005

sleep 10

echo "write client31/igmp.leave 225.1.1.1" | telnet localhost 10005

