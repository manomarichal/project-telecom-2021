echo "connecting to localhost 10003 and executing join on 225.1.1.1"

echo "write client21/igmpclient.client_join 225.1.1.1" | telnet localhost 10003

sleep 5

echo "write client21/igmpclient.client_join 225.1.1.1" | telnet localhost 10003

sleep 5

echo "write client21/igmpclient.client_leave 225.1.1.1" | telnet localhost 10003


