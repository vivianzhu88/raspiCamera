#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 

#define PORT 8888 
#define BCAST_PORT 9999
#define BUFFER_SIZE 3*800*600
using namespace cv;
using namespace std;

void bCastThread(void)
{
      int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    char *sendString;                 /* String to broadcast */
    int broadcastPermission = 1;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */

    cout << "bcast thread start" << endl;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
    { 
        perror("UDP socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0) 
    { 
        perror("Could not set UDP permissions"); 
        exit(EXIT_FAILURE); 
    } 
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &broadcastPermission, sizeof(broadcastPermission)) < 0) 
    { 
        perror("Could not reuse UDP port"); 
        exit(EXIT_FAILURE); 
    } 
    

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;/* Broadcast IP address */
    broadcastAddr.sin_port = htons(BCAST_PORT);         /* Broadcast port */
    cout << "Starting beacon" << endl;
    while(1)
    {
        sendto(sock, "Host", 4, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
        
        sleep(2);
        
    }
}

int main()
{
    //tcp settings
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[BUFFER_SIZE] = {0}; 
    const char *hello = "Hello from server";     

    //text settings
    string text = "Test String";
    
    int tBoxPosX = 20,
        tBoxPosY = 20,
        tBoxBorderThickness = 1,
        tBoxFont = FONT_HERSHEY_COMPLEX_SMALL,
        tBoxBaseline = 0;

    double tBoxFontScale =1;

    Scalar textColor(255,50,55);

    //calc dependant varsmak
    Size tBoxBorderSize;

    //opencv data vars
    VideoCapture capture; //camera feed
    Mat currImg,          //output Image    
        textForground,    //text color layer
        textAlpha,        //text draw layer
        image_roi;        //roi of output image

    //bcast thread 
    thread threadHolder (bCastThread);


     // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Forcefully attaching socket to the port 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 

    bind(server_fd, (struct sockaddr *)&address,sizeof(address)); 
 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
  

    //timing variables to check performace
    clock_t startTime=clock(), currTime=clock();
    
    struct timeval currFrameTime,lastFrameTime;
    gettimeofday(&currFrameTime, NULL);

        
    capture.open(0);
    if(capture.isOpened())
    {
        cout << "Capture is opened" << endl;
        while(waitKey(10) != 'q')
        {
            gettimeofday(&currFrameTime, NULL);
            //calculate timing data
            long int ms =(currFrameTime.tv_sec * 1000 + currFrameTime.tv_usec / 1000) - (lastFrameTime.tv_sec * 1000 + lastFrameTime.tv_usec / 1000) ;
            text =   "Proc time: "+ to_string((float)(currTime - startTime) /CLOCKS_PER_SEC) + " FPS: " + to_string((float)(1000.0/ ms));
            lastFrameTime = currFrameTime;


            
            //grab image from camera
            capture >> currImg;
            if(currImg.empty())
                break;

            //log the cpu clock after alloc and draw
            

            //send images 
            Size imageSize = currImg.size();
            string output;
            static int frameNumber =0;
            output =  to_string(imageSize.width) + to_string(imageSize.height) + ":" +
                    to_string(imageSize.width * imageSize.height * 3) + "?";
            cout << "Frame Number: " << frameNumber++ << " " << text <<endl;
            int datalen = imageSize.width * imageSize.height * 3, currPos = 0;
            int packetSize = imageSize.width;
            int currPacket; 
            startTime = clock();
            while(currPos < datalen)
            {
                if(currPos + packetSize > datalen)
                    currPacket = datalen - currPos;
                else
                    currPacket = packetSize;
            
                send(new_socket, currImg.data + currPos, currPacket, 0);
                currPos += currPacket;
            }
            currTime = clock();

        }
    }

    return 0;
}


