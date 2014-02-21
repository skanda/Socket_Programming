NAME: GURUANAND, SKANDA
ID	: 7842 6022 19

About Assignment
------------------
In this assignment I have used the concepts of Socket Programming to implement a Logistics System which has a set of 4 outlet stores 
and one central warehouse.
The warehouse manages the inventory. In order to do that, each outlet store reports its inventory to the warehouse using TCP. 
Based on the collected information, the central-warehouse computes the excess supply or shortage of each good and dispatches a 
truck to visit the stores successively to collect the excessive goods or replenish any shortages. Sending the truck uses a UDP Connection.
All communications take place over TCP and UDP sockets in client-server architecture.

I have used socket programming system calls like - getaddrinfo(), socket(), bind(), connect(), listen(), accept() send() , recv(), sendto(), recvfrom(), close() 
to connect to other server machines

Code Files
----------
1. warehouse.c

This file has the code which opens a TCP port and listens for the outlet vector from the store outlet vector. Once it receives the outlet 
vector from all the stores, it computes the truck vector and sends the truck vector to store1 through
a UDP port. In the end, when all the outlets are replinished with their inventory i.e. after 2 rounds
of iteration between the stores, the truck vector returns back to the warehouse.

2. store1.c/store2.c/store3.c/store4.c

Each of the stores initially will create a TCP connection to the server and transmit the outlet vector to the warehouse by reading the 
data from the file given. Upon receving the truck vector from warehouse, in the 1st round, store1 computes the shortage/ excess, modifies the 
outlet vector and the truck vector and sneds the truck vector to store2 through UDP sockets. Similar operations occur in store2,store3 and store4.
The same procedure is followed in 2nd round. At the end of 2nd round, truck vector is sent to warehouse from store4 over a UDP Connection.


Steps to be taken by the TA
-----------------------------
1. A Makefile is provided which compiles all the code and generates the executables.
2. Execute make -s to generate the executables.
3. Open 5 terminals -> 1 for warehouse and 1 for each of the 4 stores. Keep the execution command (ex. ./warehouse) 
ready in each of the terminal.
4. The stores text files should be in the same directory as the executables.
5. Run ./warehouse first followed by ./store1,./store2,./store3,./store4 in thier respective terminals. The store terminals can be executed
in any order.
6. Each of the the store has a delay of 2seconds and the warehouse has a delay of 2seconds.
7. Please allow sufficient time for the communication system to be completed.


FORMAT OF MESSAGES DISPLAYED
------------------------------
Central Warehouse
------------------
Phase 1: The central warehouse has TCP port number 21319 and IP address 127.0.0.1
Phase 1: The central warehouse received information from Store1
Phase 1: The central warehouse received information from Store2
Phase 1: The central warehouse received information from Store3
Phase 1: The central warehouse received information from Store4
Phase 1: End of Phase1 for the central warehouse
Phase 2: The central warehouse has UDP port number 31319 and IP address 127.0.0.1
Phase 2: Sending the truck-vector to outlet store store_1. The truck vector value is <5,0,0>
Phase 2: The central warehouse has UDP port 32319 and IP address 127.0.0.1
Phase 2: The final truck-vector is received from the outlet store store 4, the truck vector value is <0,3,0>
Phase 2: End of Phase 2 for the central warehouse


Store1
---------
Phase 1: store_1 has TCP port number 49391 and IP address 127.0.0.1
Phase 1: The outlet vector <-5,2,3> for store_1 has been sent to the central warehouse
End of Phase1 for store1
Phase 2: Store_1 has UDP port 5319 and IP address 127.0.0.1
Phase 2: Store_1 received the truck-vector<5,0,0> from the central warehouse
Phase 2: The Store1 has UDP port number 6319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<0,2,3> has been sent to store_2
Phase 2: Store_1 updated outlet-vector is <0,0,0>
Phase 2: Store_1 has UDP port 7319 and IP address 127.0.0.1
Phase 2: truck-vector <1,11,0> has been received from Store_4
Phase 2: The Store1 has UDP port number 8319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<1,11,0> has been sent to store_2
Phase 2: Store_1 updated outlet-vector is <0,0,0>
End of Phase 2 for store 1



Store2
---------
Phase 1: store_2 has TCP port number 49392 and IP address 127.0.0.1
Phase 1: The outlet vector <2,-10,5> for store_2 has been sent to the central warehouse
End of Phase1 for store2
Phase 2: Store_2 has UDP port 9319 and IP address 127.0.0.1
Phase 2: Store_2 received the truck-vector<0,2,3> from the store1
Phase 2: The Store2 has UDP port number 10319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<2,0,8> has been sent to store_3
Phase 2: Store_2 updated outlet-vector is <0,-8,0>
Phase 2: Store_2 has UDP port 11319 and IP address 127.0.0.1
Phase 2: truck-vector <1,11,0> has been received from Store_2
Phase 2: The Store2 has UDP port number 12319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<1,3,0> has been sent to store_3
Phase 2: Store_2 updated outlet-vector is <0,0,0>
End of Phase 2 for store 2



Store3
-------
Phase 1: store_3 has TCP port number 49403 and IP address 127.0.0.1
Phase 1: The outlet vector <-3,5,-7> for store_3 has been sent to the central warehouse
End of Phase1 for store3
Phase 2: Store_3 has UDP port 13319 and IP address 127.0.0.1
Phase 2: Store_3 received the truck-vector<2,0,8> from store 2
Phase 2: The Store3 has UDP port number 14319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<0,5,1> has been sent to store_4
Phase 2: Store_3 updated outlet-vector is <-1,0,0>
Phase 2: Store_3 has UDP port 15319 and IP address 127.0.0.1
Phase 2: truck-vector <1,3,0> has been received from Store_2
Phase 2: The Store3 has UDP port number 16319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<0,3,0> has been sent to store_4
Phase 2: Store_3 updated outlet-vector is <0,0,0>
End of Phase 2 for store 3



Store4
---------
e 1: store_4 has TCP port number 49405 and IP address 127.0.0.1
Phase 1: The outlet vector <1,6,-1> for store_4 has been sent to the central warehouse
End of Phase1 for store4
Phase 2: Store_4 has UDP port 17319 and IP address 127.0.0.1
Phase 2: Store_4 received the truck-vector<0,5,1> from store 3
Phase 2: The Store4 has UDP port number 18319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<1,11,0> has been sent to store_1
Phase 2: Store_4 updated outlet-vector is <0,0,0>
Phase 2: Store_4 has UDP port 19319 and IP address 127.0.0.1
Phase 2: truck-vector <0,3,0> has been received from Store_3
Phase 2: The Store4 has UDP port number 20319 and IP address 127.0.0.1
Phase 2: The updated truck-vector<0,3,0> has been sent to WareHouse
Phase 2: Store_4 updated outlet-vector is <0,0,0>
End of Phase 2 for store 4


IDIOSYNCRASIES
-----------------
The project may fail under following conditions

1. If all the stores try to send the outlet vectors at the same time to the warehouse, project may fail
2. Any change in text file format, the program behavior is unknown.


Code Reuse
-----------
1.I have used the code from Beej's guide online tutorial. All the system calls like socket,bind,accept,listen,send ,receive, recievefrom,sendto
has been copied from beej's guide.
	
2.Some of the File handling system calls are googled and used.
