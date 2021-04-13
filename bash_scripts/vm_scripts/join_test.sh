echo "connecting to localhost 10003 and executing join on 244.4.4.4"

echo "write client21/igmpclient.client_join 244.4.4.4" | telnet localhost 10003

sleep 20

echo "write client21/igmpclient.client_leave 244.4.4.4" | telnet localhost 10003


