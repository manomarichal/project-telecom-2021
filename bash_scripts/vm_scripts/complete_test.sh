echo "======JOINS======"

echo "==connecting to localhost 10003 and executing join on 225.1.1.1 (client21)=="

echo "write client21/igmp.join 225.1.1.1" | telnet localhost 10003

sleep 5

echo "==connecting to localhost 10004 and executing join on 225.1.1.1 (client22)=="

echo "write client22/igmp.join 225.1.1.1" | telnet localhost 10004

sleep 5

echo "==connecting to localhost 10005 and executing join on 225.1.1.1 (client31)=="

echo "write client31/igmp.join 225.1.1.1" | telnet localhost 10005

sleep 5

echo "==connecting to localhost 10006 and executing join on 225.1.1.1 (client32)=="

echo "write client32/igmp.join 225.1.1.1" | telnet localhost 10006

sleep 4

echo "======LEAVES======"

echo "==connecting to localhost 10003 and executing leave on 225.1.1.1 (client21)=="

echo "write client21/igmp.leave 225.1.1.1" | telnet localhost 10003

sleep 5

echo "==connecting to localhost 10004 and executing leave on 225.1.1.1 (client22)=="

echo "write client22/igmp.leave 225.1.1.1" | telnet localhost 10004

sleep 5

echo "==connecting to localhost 10005 and executing leave on 225.1.1.1 (client31)=="

echo "write client31/igmp.leave 225.1.1.1" | telnet localhost 10005

sleep 5

echo "==connecting to localhost 10006 and executing leave on 225.1.1.1 (client32)=="

echo "write client32/igmp.leave 225.1.1.1" | telnet localhost 10006