//
//  Yashwanth Reddy Gurijala
//  COSC 3360 - Operating System Fundamentals
//  Due Date - 4/1/2020
//  
//  Description:
//  A client program that will connect with
//  server and send it requests for the weather report
//  for a given city.
//
//  Yashwanth_Gurijala_client.c
//
//  Created by Yashwanth Reddy Gurijala on 4/1/2020.
//  Socket Tutorial used for help:- https://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
//  Copyright © 2020 Yashwanth Reddy Gurijala. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int main()
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[1024], portno[1024], hostName[1024];

    printf("Enter the server host name: ");
    bzero(hostName, 1024);
    fgets(hostName, 1023, stdin);

    printf("Enter the server port number: ");
    bzero(portno, 1024);
    fgets(portno, 1023, stdin);
    printf("\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket");
        return -1;
    }

    server = gethostbyname("localhost");
    if (server == NULL) 
    {
        printf("ERROR, no such host\n");
        return - 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(portno));
   
    if (connect(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        return -1;
    }
    
    printf("Enter a city name: ");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        printf("ERROR writing to socket");
        return -1;
    }
    
    bzero(buffer, 1024);

    n = read(sockfd, buffer, 1023);
    if (n < 0)
    {
        printf("ERROR reading from socket");
        return -1;
    }
    
    printf("%s\n", buffer);
    
    return 0;
}