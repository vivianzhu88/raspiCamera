
#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <sys/time.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"


#define PORT 8888
#define MAX_BUFF 100000   


using namespace cv;
using namespace std;


int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    const char *hello = "Hello from client"; 
    char buffer[MAX_BUFF] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    printf("Connected to server!"); 

    string descriptor, text;
    const int dataLen = 640*480*3;
    Mat buffer1(480,640,CV_8UC3);
    Mat buffer2(480,640,CV_8UC3);
    int buff1pos = 0;
    int buff2pos = 0;
    int frameCount =0;
    struct timeval currFrameTime,lastFrameTime;
    while(waitKey(1) != 'q')
    {
        
        while(buff1pos < dataLen)
        {
            valread = read( sock , buffer1.data + buff1pos, dataLen - buff1pos);
            buff1pos += valread;
        }
        imshow("Sample", buffer1);

        gettimeofday(&currFrameTime, NULL);
        long int ms =(currFrameTime.tv_sec * 1000 + currFrameTime.tv_usec / 1000) - (lastFrameTime.tv_sec * 1000 + lastFrameTime.tv_usec / 1000) ;
        lastFrameTime = currFrameTime;   
        text = to_string((float)(1000.0/ ms))+  " time stamp: " + to_string(currFrameTime.tv_usec/ 1000.0);
        cout << text << " current frame: " << frameCount++ << endl;

        buff1pos =0;
    }
     
    return 0; 
} 