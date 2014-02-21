/*
** warehouse.c 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "21319"  // the port users will be connecting to
#define UDPSTORE1 "5319" //UDP Port to send truck vector to store1
#define UDPWAREHOUSE "31319" //UDP Port for sending to store1

#define UDPWAREHOUSEEND "32319" //UDP Port for listening from store 4 (end)

#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define BACKLOG 10     // how many pending connections queue will hold

int udp_phase2(char truck_vector[]);

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    struct sockaddr_in server_addr;
    socklen_t sin_size;
    struct sigaction sa;
    char recv_buf1[100]="";
    char truck_vector[100]="";
    int temp[10]={0};
    int store1_data[10][10]={{0}};
    char *num_items; 
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;
    int num_stores=4; //testing
    int bytes_recv=0;
    int i=0;
    int k,j=0;
    int store_id=0;
    char *pch;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
  
    struct hostent *host;
    host = gethostbyname("localhost");
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
 
 /*----Receive the outlet vector from the stores*/
 
    // print information about this host:

    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    printf("Phase 1: The central warehouse has TCP port number %s and IP address %s\n", PORT, inet_ntoa(server_addr.sin_addr));
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }


   while(1) 
   {  // main accept() loop
     memset(&recv_buf1, 0, sizeof recv_buf1);
     if(num_stores==0)
	break;

        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1) 
        {
            perror("accept");
        }

	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

	bytes_recv=recv(new_fd,recv_buf1,99,0);
	recv_buf1[bytes_recv]='\0';
	printf("Phase 1: The central warehouse received information from Store%c \n",recv_buf1[0]);
	store_id=(int)recv_buf1[0]-49;


	//print the string u have entered on the output screen//
	 pch=strtok(recv_buf1,",");
		while(pch != NULL)
		 {
   		  store1_data[store_id][i]=atoi(pch);
		  pch=strtok(NULL,",");
   	 	  i++;
 		 }
       	 i=0;
       	 num_stores--;
       	 close(new_fd);  // parent doesn't need this

   }

		printf("Phase 1: End of Phase1 for the central warehouse\n");

//truck vector computation
		temp[0]=1;
		for (j=1;j<4;j++)
		{
  			temp[j]=store1_data[0][j]+store1_data[1][j]+store1_data[2][j]+store1_data[3][j];
			if(temp[j]<0)
			 temp[j]=-temp[j];
			else
			 temp[j]=0;
		}

		k=sprintf(truck_vector,"%d,%d,%d,%d",temp[0], temp[1], temp[2], temp[3]);
 
		
		udp_phase2(truck_vector);
return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UDP Connection 

int udp_phase2(char truck_vector[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    struct sockaddr_in server_addr;
    struct hostent *host;
    host = gethostbyname("localhost");
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	int rv1;
	
//sending to store1
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo("localhost", UDPSTORE1, &hints, &servinfo)) != 0) 
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}

     // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) 
	{	
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    	}

	sleep(2);

    if ((numbytes = sendto(sockfd, truck_vector, strlen(truck_vector), 0, p->ai_addr, p->ai_addrlen)) == -1)
	{
        perror("talker: sendto");
        exit(1);
    	}	

    freeaddrinfo(servinfo);

	if ((rv1 = getaddrinfo("localhost", UDPWAREHOUSE, &hints, &servinfo)) != 0) 
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
	
	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        } 

        break;
    }

	printf("Phase 2: The central warehouse has UDP port number %s and IP address %s\n", UDPWAREHOUSE, inet_ntoa(server_addr.sin_addr));

    printf("Phase 2: Sending the truck-vector to outlet store store_1. The truck vector value is <%s>\n",&truck_vector[2]);
    close(sockfd);

///////////////////////////////////////////////////////////////////
//receiving from store4

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("localhost", UDPWAREHOUSEEND, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    printf("Phase 2: The central warehouse has UDP port %s and IP address %s\n", UDPWAREHOUSEEND, inet_ntoa(server_addr.sin_addr));

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);


    memset(&buf, 0, sizeof buf);

    addr_len = sizeof their_addr;
    numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1 , 0,(struct sockaddr *)&their_addr, &addr_len); 

    buf[numbytes] = '\0';
    printf("Phase 2: The final truck-vector is received from the outlet store store 4, the truck vector value is <%s>  \n",&buf[2]);
    printf("Phase 2: End of Phase 2 for the central warehouse\n");
	close(sockfd);

   return 0;
}
