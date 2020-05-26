//
//  Yashwanth Reddy Gurijala
//  COSC 3360 - Operating System Fundamentals
//  Due Date - 4/1/2020
//  
//  Description:
//  A server program that will wait for connection
//  requests from client and send weather data of cities 
//  which client requested
//
//  Yashwanth_Gurijala_server.c
//
//  Created by Yashwanth Reddy Gurijala on 4/1/2020.
//  Socket Tutorial used for help:- https://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
//  Copyright © 2020 Yashwanth Reddy Gurijala. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

//struct to hold weather data
struct Node
{
    char cityName[1024], maxTemp[1024], skyConditions[1024];
    struct Node* next;
};

//insertion into my linked list
void insert(struct Node** n, char* a, char* b, char* c)
{
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));

    strcpy(newNode->cityName, a);
    strcpy(newNode->maxTemp, b);
    strcpy(newNode->skyConditions, c);
    newNode->next = (*n);

    (*n) = newNode;
}

//displays all contents in my linked list
void display(struct Node* n)
{
    while (n != NULL)
    {
        printf("%s,", n->cityName);
        printf("%s,", n->maxTemp);
        printf("%s", n->skyConditions);
        n = n->next;
    }
}

// receives city name from client, then searches my list
// Once it finds a city, it returns weather information of that city to client
void dostuff(int sock, struct Node* a)
{
    int n;
    char buffer[1024];

    n = read(sock, buffer, 1024);
    strtok(buffer, "\n");
    printf("Weather report for %s\n", buffer);

    while (a != NULL)
    {
        if ((strcmp(a->cityName, buffer)) == 0)
        {
            bzero(buffer, 1024);

            strcat(buffer, "Tommorow's maximum temperature is ");
            strcat(buffer, a->maxTemp);
            strcat(buffer, "F\nTommorow's sky condition is ");
            strcat(buffer, a->skyConditions);

            printf("%s\n", buffer);

            n = write(sock, buffer, strlen(buffer));
            if (n < 0) 
                error("ERROR writing to socket");

            return;
        }

        a = a->next;
    }

    bzero(buffer, 1024);
    strcat(buffer, "No Data\n");
    printf("%s\n", buffer);

    n = write(sock, buffer, strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
}

int main()
{
    struct Node* head = NULL;
    FILE* fptr = fopen("weather20.txt", "r");
    char line[1024];
    char* cityName = NULL, * maxTemp = NULL, * cloudConditions = NULL;

    while (fgets(line, sizeof(line), fptr))
    {
        char* token = strtok(line, ",");

        while (token != NULL)
        {
            if (cityName == NULL)
                cityName = token;
            else if (maxTemp == NULL)
                maxTemp = token;
            else if (cloudConditions == NULL)
                cloudConditions = token;

            token = strtok(NULL, ",");
        }

        insert(&head, cityName, maxTemp, cloudConditions);

        cityName = NULL;
        maxTemp = NULL;
        cloudConditions = NULL;
        token = NULL;
    }

    fclose(fptr);

    int sockfd, newsockfd, clilen, pid;
    struct sockaddr_in servAddr, cliAddr;
    char portNum[1024];

    printf("Enter server port number: ");
    bzero(portNum, 1024);
    fgets(portNum, 1023, stdin);
    printf("\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket \n");
        return -1;
    }
    
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(atoi(portNum));
    
    if (bind(sockfd, (struct sockaddr*) & servAddr, sizeof(servAddr)) < 0)
    {
        printf("ERROR on binding");
        return -1;
    }
    
    listen(sockfd, 5);
    clilen = sizeof(cliAddr);
    
    while (1) 
    {
        newsockfd = accept(sockfd, (struct sockaddr*) &cliAddr, &clilen);
        if (newsockfd < 0)
        {
            printf("ERROR on accept");
            return -1;
        }
        pid = fork();
        if (pid < 0)
        {
            printf("ERROR on fork");
            return -1;
        }
        if (pid == 0) 
        {
            close(sockfd);
            dostuff(newsockfd, head);
            exit(0);
        }
        else 
            close(newsockfd);
    } 

    return 0; 
}
