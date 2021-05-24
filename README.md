# Project Telecom 2020 (Mano Marichal & Robbe Van de Velde)

This is our implementation of the IGMPv3 protocol in click. The IGMPv3 protocol is described in RFC 3776.

## Installing and running on the provided VM.

#### Clone git repository   

Clone the repository in `/home/student/Desktop/project-telecom-2021`, the provided bash scripts will not work if the project is cloned anywhere else. 

	git clone https://github.com/manomarichal/project-telecom-2021.git /home/student/Desktop/project-telecom-2021

then navigate to the scripts folder
	
	 cd /home/student/Desktop/project-telecom-2021/bash_scripts/vm_scripts
 
#### Setup

    chmod +x ./setup.sh
    ./setup.sh

This sets up the other bash scripts and the taps
    
#### Running our implementation
    
    sudo ./run_student_implementation.sh 


#### Running the reference implementation
    
    sudo ./run_reference_implementation.sh 

#### Other run scripts:

- `test_client.sh`: runs our clients, and the server and router from the reference implementation
- `test_router.sh`: runs our router, and the server and clients from the reference implementation
- `test_router+client.sh`: runs our client and router, and the server from the reference implementation

## Using Click 

#### Connecting
You can connect to the following network members:

- `telnet localhost 10001`: the multicast router
- `telnet localhost 10002`: the multicast server
- `telnet localhost 10003`: client21
- `telnet localhost 10004`: client21
- `telnet localhost 10005`: client31
- `telnet localhost 10006`: client32

use `read list` to check which handlers are available

#### Joining and leaving
For example, you can join a multicast group `1.2.3.4` with client21 with 

    write client21/igmp.client_join 1.2.3.4

And leave the same group with

    write client21/igmp.client_leave 1.2.3.4

#### Click test scripts
These can be found in `project-telecom-2021/bash_scripts/vm_scripts`

- `complete_test.sh`: each client joins, and leaves after 30 seconds
- `join_test`: client21 joins and leaves after 30 seconds
- `join_test_2`: client21 joins twice
- `leave_test`: client21 leaves twice

## Parameters
IGMPv3 uses a lot of different parameters that can impact how the router works, in `library/router.click` on line 33 you can change them. Only natural numbers are supported, and values are in tenths of seconds.
- `RV`: the robustness variable
- `QI`: the query interval
- `SQI`: the startup query interval
- `SQC`:  the startup count
- `LMQI`: the last member query interval
- `LMQC`: the last member query count
- `QRI`: the query response interval
