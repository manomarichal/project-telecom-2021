echo "connecting to localhost 10003 and executing join on 225.1.1.1"

echo "write client31/igmp.join 225.1.1.1" | telnet localhost 10005
echo "write client21/igmp.join 225.1.1.1" | telnet localhost 10003
echo "write client22/igmp.join 225.1.1.1" | telnet localhost 10004

sleep 10

echo "write client21/igmp.leave 225.1.1.1" | telnet localhost 10003
echo "write client22/igmp.leave 225.1.1.1" | telnet localhost 10004

sleep 4

echo "write client31/igmp.leave 225.1.1.1" | telnet localhost 10005

