

AddressInfo(router_server_network_address 192.168.1.254/24 00:50:BA:85:84:A1);
AddressInfo(multicast_server_address 192.168.1.1/24 00:50:BA:85:84:A2);

AddressInfo(router_client_network1_address 192.168.2.254/24 00:50:BA:85:84:B1);
AddressInfo(client21_address 192.168.2.1/24 00:50:BA:85:84:B2);
AddressInfo(client22_address 192.168.2.2/24 00:50:BA:85:84:B3);

AddressInfo(router_client_network2_address 192.168.3.254/24 00:50:BA:85:84:C1);
AddressInfo(client31_address 192.168.3.1/24 00:50:BA:85:84:C2);
AddressInfo(client32_address 192.168.3.2/24 00:50:BA:85:84:C3);


RatedSource("data", 1, -1, true)
	-> DynamicUDPIPEncap(multicast_server_address:ip, 1234, 224.0.0.22, 1234)
	// The MAC addresses here should be from the multicast_server to get past the HostEtherFilter.
	// This way we can reuse the input from the network for the applications.
	-> EtherEncap(0x0800, multicast_server_address:eth, multicast_server_address:eth)
	-> IPPrint("multicast_server -- transmitted a UDP packet")
	-> [0]igmpclient::IGMPClientSide(CADDR 224.0.0.22, MADDR 224.0.0.22, ASMADDR 224.0.0.1);

RatedSource("data", 1, -1, true)
	-> DynamicUDPIPEncap(multicast_server_address:ip, 1234, 224.0.0.22, 1234)
	// The MAC addresses here should be from the multicast_server to get past the HostEtherFilter.
	// This way we can reuse the input from the network for the applications.
	-> EtherEncap(0x0800, multicast_server_address:eth, multicast_server_address:eth)
	-> IPPrint("multicast_server -- transmitted a UDP packet")
	-> [1]igmpclient;

igmpclient[0]->Discard;