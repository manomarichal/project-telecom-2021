echo "connecting to localhost 10003 and executing join on 225.1.1.1 (client21)"

echo "write client21/igmpclient.client_join 225.1.1.1" | telnet localhost 10003

sleep 5

echo "connecting to localhost 10004 and executing join on 225.1.1.1 (client22)"

echo "write client22/igmpclient.client_join 225.1.1.1" | telnet localhost 10004

sleep 5

echo "connecting to localhost 10005 and executing join on 225.1.1.1 (client31)"

echo "write client31/igmpclient.client_join 225.1.1.1" | telnet localhost 10005

sleep 5

echo "connecting to localhost 10006 and executing join on 225.1.1.1 (client32)"

echo "write client32/igmpclient.client_join 225.1.1.1" | telnet localhost 10006
