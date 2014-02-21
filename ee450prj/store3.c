/*
** store3.c 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "21319" // the port client will be connecting to 
#define UDPSTORE3 "13319" //UDP Port for Phase2 Part1

#define UDPSTORE3SEND1 "14319" //Port of store3 for sending to store4 during 1st round
#define UDPSTORE4 "17319" //Port of store4 binding for 1st round send

#define UDPSTORE3ROUND2 "15319" //UDP Port for listening from store2

#define UDPSTORE3SEND4 "16319" //UDP Port for sending to store4 round2
#define UDPSTORE4ROUND2 "19319" //UDP Port of store4 binding for 2nd round send

#define MAXDATASIZE 100 // max number of bytes we can get at once 

int udp_phase2(char outlet_vector[]);
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
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET_ADDRSTRLEN];
	int sa_len,sd,z;
	
	struct sockaddr_storage their_addr;
    socklen_t addr_len;
    struct sockaddr_in client_addr,sa;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
	
    struct hostent *host;
    host = gethostbyname("localhost");
    client_addr.sin_addr = *((struct in_addr *)host->h_addr);
    
/*----Sending the outlet vector to the warehouse---*/	
	
    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

	sa_len=sizeof(sa);
	z=getsockname(sockfd, (struct sockaddr *)&sa, &sa_len);

	printf("Phase 1: store_3 has TCP port number %d and IP address %s\n", (int)ntohs(sa.sin_port), inet_ntoa(client_addr.sin_addr));

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);


    freeaddrinfo(servinfo); // all done with this structure


	FILE *pFile;
	char buffer[40]="";
	char items[100]="";
	char outlet_vector[100]="";
	char out_send[100]="";

	char *num_items;
	char *line;

	pFile=fopen("Store-3.txt","r");
	if(pFile==NULL)
	{
		perror("Error");
	}
	strcpy(outlet_vector,"3");

	while(fgets(buffer,100,pFile)!=NULL)
	{
		strcat(items,buffer);
		strtok_r(items," ",&num_items);
		strcat(outlet_vector,",");
		strcat(outlet_vector, num_items);
		strtok_r(outlet_vector,"\n",&line);
	}
	fclose(pFile);




	if (send(sockfd,outlet_vector,strlen(outlet_vector),0)==-1)
	{
	perror("send");
	}
	
	printf("Phase 1: The outlet vector <%s> for store_3 has been sent to the central warehouse\n",&outlet_vector[2]);

    close(sockfd);

	printf("End of Phase1 for store3\n");
	udp_phase2(outlet_vector);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//UDP Connection

int udp_phase2(char outlet_vector[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXDATASIZE];
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    char *pch;
    int truck_data[10]={0};
    int outlet_data[10]={0};
    char truck_vector[100]="";
    int temp=0;
    int i=0;
    int j=0;
    int k=0;
	int rv1=0;
	
    struct sockaddr_in server_addr;
    struct hostent *host;
    host = gethostbyname("localhost");
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);

//receiving from store2
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("localhost", UDPSTORE3, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }


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
	printf("Phase 2: Store_3 has UDP port %s and IP address %s\n", UDPSTORE3, inet_ntoa(server_addr.sin_addr));
    printf("Phase 2: Store_3 received the truck-vector<%s> from store 2 \n",&buf[2]);
    close(sockfd);

    pch=strtok(buf,",");
		while(pch != NULL)
		 {
   		  truck_data[i]=atoi(pch);
		  pch=strtok(NULL,",");
   	 	  i++;
 		 }
     i=0;

     pch=strtok(outlet_vector,",");
		while(pch != NULL)
		 {
   		  outlet_data[i]=atoi(pch);
		  pch=strtok(NULL,",");
   	 	  i++;
 		 }
     i=0;

	for(i=1;i<4;i++)
	{
	  temp=truck_data[i]+outlet_data[i];
		if(temp<0)
		  {outlet_data[i]=temp;
	           truck_data[i]=0;}
		else
		  {outlet_data[i]=0;
		   truck_data[i]=temp;}
	}


	k=sprintf(truck_vector,"%d,%d,%d,%d",truck_data[0], truck_data[1], truck_data[2], truck_data[3]);
	
	j=sprintf(outlet_vector,"%d,%d,%d,%d",outlet_data[0], outlet_data[1], outlet_data[2], outlet_data[3]);


	freeaddrinfo(servinfo);
	close(sockfd);

//////////////////////////////////////////////////////////////////
//Sending to store4

	memset(&hints, 0, sizeof hints);
    	hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    	hints.ai_socktype = SOCK_DGRAM;
    	hints.ai_flags = AI_PASSIVE; // use my IP
 
	if ((rv = getaddrinfo("localhost", UDPSTORE4, &hints, &servinfo)) != 0) 
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
	{
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

	if ((rv1 = getaddrinfo("localhost", UDPSTORE3SEND1, &hints, &servinfo)) != 0) 
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
	
	printf("Phase 2: The Store3 has UDP port number %s and IP address %s\n", UDPSTORE3SEND1, inet_ntoa(server_addr.sin_addr));

    printf("Phase 2: The updated truck-vector<%s> has been sent to store_4 \n",&truck_vector[2]);
    printf("Phase 2: Store_3 updated outlet-vector is <%s>\n",&outlet_vector[2]);

    close(sockfd);

///////////////////////////////////////////////////////////////////////
//receiving from store2 - round2

memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("localhost", UDPSTORE3ROUND2, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    printf("Phase 2: Store_3 has UDP port %s and IP address %s\n", UDPSTORE3ROUND2, inet_ntoa(server_addr.sin_addr));

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
    printf("Phase 2: truck-vector <%s> has been received from Store_2 \n",&buf[2]);
    close(sockfd);
	
	i=0;
    pch=strtok(buf,",");
		while(pch != NULL)
		 {
   		  truck_data[i]=atoi(pch);
		  pch=strtok(NULL,",");
   	 	  i++;
 		 }
     i=0;

     pch=strtok(outlet_vector,",");
		while(pch != NULL)
		 {
   		  outlet_data[i]=atoi(pch);
		  pch=strtok(NULL,",");
   	 	  i++;
 		 }
     i=0;

	for(i=1;i<4;i++)
	{
	  temp=truck_data[i]+outlet_data[i];
		if(temp<0)
		  {outlet_data[i]=temp;
	           truck_data[i]=0;}
		else
		  {outlet_data[i]=0;
		   truck_data[i]=temp;}
	}


	k=sprintf(truck_vector,"%d,%d,%d,%d",truck_data[0], truck_data[1], truck_data[2], truck_data[3]);
	
	j=sprintf(outlet_vector,"%d,%d,%d,%d",outlet_data[0], outlet_data[1], outlet_data[2], outlet_data[3]);
	
	freeaddrinfo(servinfo);
	close(sockfd);

////////////////////////////////////////////////////////////////
//sending to store4 round2

memset(&hints, 0, sizeof hints);
    	hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    	hints.ai_socktype = SOCK_DGRAM;
    	hints.ai_flags = AI_PASSIVE; // use my IP
 
	if ((rv = getaddrinfo("localhost", UDPSTORE4ROUND2, &hints, &servinfo)) != 0) 
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
	{
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

	if ((rv1 = getaddrinfo("localhost", UDPSTORE3SEND4, &hints, &servinfo)) != 0) 
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
	printf("Phase 2: The Store3 has UDP port number %s and IP address %s\n", UDPSTORE3SEND4, inet_ntoa(server_addr.sin_addr));

	
    printf("Phase 2: The updated truck-vector<%s> has been sent to store_4 \n",&truck_vector[2]);
    printf("Phase 2: Store_3 updated outlet-vector is <%s>\n",&outlet_vector[2]);

    close(sockfd);

    printf("End of Phase 2 for store 3\n");

return 0;
}
