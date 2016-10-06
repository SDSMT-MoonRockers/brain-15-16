#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <core.h>
#include <unistd.h>

/*Global Variables*/
int LeftFor = 8;
int LeftRev = 7;
int RightFor = 13;
int RightRev = 12;
int ConvFor = 2;
int ConvRev = 3;
int ConvIn = 4;
int ConvOut = 5;
int DumpIn = 6;
int DumpOut = 9;
int MineMode = 10;

//play with the values of these functions. Conveyortime should be close.
static int conveyortime = 5000;//milliseconds for conveyor to get into position
int conpos = 0; //integer storing position of conveyor
static int minetime = 5000;//milliseconds to mine
static int dumptime = 5000;//miliseconds to dump

//added functions for autonomous mining and dumping

/*Connection Globals*/
int listenfd = 0, connfd = 0;
int n;
struct sockaddr_in serv_addr;
char sendBuff[1025];
time_t ticks;

void mineRoutine();
void dumpRoutine()

void setup()
{
pinMode(LeftFor, OUTPUT);
pinMode(LeftRev, OUTPUT);
pinMode(RightFor, OUTPUT);
pinMode(RightRev, OUTPUT);
pinMode(ConvFor, OUTPUT);
pinMode(ConvRev, OUTPUT);
pinMode(ConvIn, OUTPUT);
pinMode(ConvOut, OUTPUT);
pinMode(DumpIn, OUTPUT);
pinMode(DumpOut, OUTPUT);
pinMode(MineMode, OUTPUT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

}

void loop()
{
     n = read(connfd, sendBuff, strlen(sendBuff) );

     if(n>0)
    {
      if(sendBuff[6]=='1')           digitalWrite(LeftFor, HIGH);
      else                             digitalWrite(LeftFor, LOW);
      if(sendBuff[7]=='1')           digitalWrite(LeftRev, HIGH);
      else                             digitalWrite(LeftRev, LOW);
      if(sendBuff[8]=='1')           digitalWrite(RightFor, HIGH);
      else                             digitalWrite(RightFor, LOW);
      if(sendBuff[9]=='1')           digitalWrite(RightRev, HIGH);
      else                             digitalWrite(RightRev, LOW);
      if(sendBuff[5]=='1')           digitalWrite(ConvFor, HIGH);
      else                             digitalWrite(ConvFor, LOW);
      if(sendBuff[1]=='1')           digitalWrite(ConvRev, HIGH);
      else                             digitalWrite(ConvRev, LOW);
      if(sendBuff[4]=='1')           digitalWrite(ConvIn, HIGH);
      else                             digitalWrite(ConvIn, LOW);
      if(sendBuff[2]=='1')           digitalWrite(ConvOut, HIGH);
      else                             digitalWrite(ConvOut, LOW);

      //we might not need this line unless we want to be able to run the
      //dump conveyor in reverse.
      if (sendBuff[3] == '1')         digitalWrite(DumpIn, HIGH);
      else                             digitalWrite(DumpIn, LOW);
      
      if (sendBuff[0] == '1')
      {
          digitalWrite(DumpOut, HIGH);

          while (sendBuff[0] == '1')
          {
              delay(100);
          }
          digitalWrite(DumpOut, LOW);
          
      }
      else                             digitalWrite(DumpOut, LOW);


      if (sendBuff[10] == '1')
      {
        digitalWrite(MineMode, HIGH);//switch into Mine Mode
        digitalWrite(ConvOut, HIGH); //begin putting out conveyor

        while(sendBuff[10] == '1' && conpos != conveyortime/100)
        {
            conpos++;
            delay(100);
        }

        digitalWrite(ConvOut, LOW); //stop the conveyor
        digitalWrite(LeftFor, HIGH);//begin moving forward
        digitalWrite(RightFor, HIGH);
        digitalWrite(ConvFor, HIGH);

        while(sendBuff[10] == '1')
        {
            delay(10);
        }

        digitalWrite(LeftFor, LOW);
        digitalWrite(RightFor, LOW);
        digitalWrite(ConvFor, LOW);
        digitalWrite(ConvIn, HIGH);

        while(conpos !=0)
        {
            conpos--;
            delay(100);
        }

        digitalWrite(ConvIn, LOW);
        digitalWrite(MineMode, LOW);
      }
      
    
    
    int  i = 0;
    while(i < 9)
    {
         sendBuff[i] = '0';
         i++;
    }
     
    usleep(50000);
}
  //    close(connfd);
  //   sleep(1);
}


//this function will mine at the spot for a time determined in the global variables
//you could implememnt the timer library to make it so you can pilot the thing while its mining

void mineRoutine()
{
    digitalWrite(MineMode, HIGH);//switch into Mine Mode

    digitalWrite(ConvOut, HIGH);//put the conveyor out
    delay(conveyortime);//wait until conveyor is out
    digitalWrite(ConvOut, LOW);//stop the actuators

    digitalWrite(LeftFor, HIGH);
    digitalWrite(RightFor, HIGH);
    digitalWrite(ConvFor, HIGH);//turn the conveyor to mine
    //here would be the place to implement a weight sensor.
    //you could use an analog read and compare with max weight
    delay(minetime);//wait until done mining
    digitalWrite(LeftFor, LOW);
    digitalWrite(RightFor, LOW);

    digitalWrite(ConvFor, LOW); //stop the conveyor
    digitalWrite(ConvIn, HIGH); //pull the conveyor back in
    delay(conveyortime); //wait until it's in
    digitalWrite(ConvIn, LOW);
    digitalWrite(MineMode, LOW); //exit mining mode

}

void dumpRoutine()
{
    //this code whould work the same with actuators
    //as it will with a conveyor.
    digitalWrite(DumpOut, HIGH);
    delay(dumptime);
    digitalWrite(DumpOut, LOW);
}
