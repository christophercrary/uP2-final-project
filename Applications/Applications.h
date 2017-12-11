// This header file will define certain data structures used specifically for wifi communication across multiple applications


#include "cc3100_usage.h"
#include <stdbool.h>
#include "G8RTOS.h"
#include "mumessage.h"
#include "pong.h"
#include "BSP.h"
#ifndef APPLICATIONS_H
#define APPLICATIONS_H
//enum used to determine the different types of data that can be sent/received
typedef enum{

    HOME_SCREEN = 0,
    MUMESSAGE = 1,
    PONG = 2

} Intended_Data_t;

typedef enum{

    SENT = 0,
    RECEIVED = 1,
    DELIVERED = 2
}Message_Status_t;


typedef struct{

    Intended_Data_t intended_app;
    uint16_t size_of_data;
} Header_Data_t;


typedef Intended_Data_t Current_App_t; //make another data type determining which app is currently opened


//used to determine if a board is a client or a host
typedef playerType Board_Type_t;

//struct for client information
typedef struct
{
    uint32_t IP_address;    // client's dynamically chosen IP address
    bool isReady;
    bool hasJoined;
    bool hasAcknowledged;
}ClientInfo_t;

//host IP address is a define


typedef struct
{

    Current_App_t current_app; //which app is currently open
    Board_Type_t board_type; //host or client

    //messaging app related  data
    //Message_Data_t message_data;
    //Old_Messages_t old_messages;


}Phone_t;

//global variables
Phone_t phone; //everything will probably be ran through this data structure
ClientInfo_t client1;
ClientInfo_t client2;

/*************** Thread declarations *****************/

//this thread will receive all data from cc3100
void thread_receive_data();

//initialize board as host
void thread_init_host_wifi();

//initialize board as client
void thread_init_client_wifi();

#endif
