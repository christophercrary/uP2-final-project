//this file will hold all application related implementation that is shared amongst all (or some) of the applications on the phone
// most notably, this is where all wifi communication receive threads will be.


#include "Applications.h"
#include "G8RTOS.h"

semaphore_t semaphore_CC3100;          // used to access CC3100 WiFi chip


/*
 *
 *
 *
 * NOTE:    The board with Chris' data MUST BE connected to the host first. Then Wes. This ensures that the contact data is valid, I.E. Wes wont be chris
 *          and chris wont be wes (not a fatal error but for sending messages to the correct contact will be important)
 */
void thread_init_host_wifi()
{
    initCC3100(Host); //initialize CC3100 as the host

    //CLIENT1 NEEDS TO BE CHRIS
     while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0);

     client1.hasAcknowledged = true;
     SendData((uint8_t*)&client1.hasAcknowledged, client1.IP_address, sizeof(client1.hasAcknowledged));

     // light up led to show WiFi connection
     P2->DIR |= (BIT0); //make p2.0 an output
     P2->OUT |= (BIT0);

     //G8RTOS_add_thread(thread_receive_data, 50, "Receive data");
     phone.IP_address = HOST_IP_ADDR;

     G8RTOS_add_thread(thread_receive_data, 40, "receiveData"); //add background receive wifi thread

     G8RTOS_kill_current_thread();

}


void thread_init_client_wifi()
{

    initCC3100(Client); //initialize CC3100 as the client

    client1.IP_address = getLocalIP(); //give client an IP


    //send player info to host
    SendData((uint8_t *)&client1, HOST_IP_ADDR, sizeof(client1));

    // wait for server response
    while(ReceiveData((uint8_t*)&client1.hasAcknowledged, sizeof(client1.hasAcknowledged)) < 0);

    if(client1.hasAcknowledged)
    {
        // light up led to show WiFi connection
        P2->DIR |= (BIT0); //make p2.0 an output
        P2->OUT |= (BIT0);
    }
    phone.IP_address = client1.IP_address;

  //  G8RTOS_add_thread(thread_receive_data, 50, "Receive data");
     G8RTOS_add_thread(thread_receive_data, 40, "receiveData"); //add background receive wifi thread

    G8RTOS_kill_current_thread();

}





//this thread is responsible for receiving data
void thread_receive_data()
{

    uint32_t sleep_time = 100;

    while(1)
    {
        if(phone.current_app == HOME_SCREEN)
        {
            sleep_time = 50; //500 ms sleep time
        }
        else if(phone.current_app == MUMESSAGE)
        {
            sleep_time = 50;
        }
        else if(phone.current_app == PONG)
        {
            sleep_time = 3; //5 ms
        }


        G8RTOS_semaphore_wait(&semaphore_CC3100);

        //read in data, the first byte that needs to be sent will determine which application the data is intended for
        while((ReceiveData((uint8_t*)&phone.header_data, sizeof(phone.header_data))) < 0)
         {
             //for reading data till a nonzero value is returned
             G8RTOS_semaphore_signal(&semaphore_CC3100);
             G8RTOS_thread_sleep(sleep_time); //sleep for 1ms to avoid deadlock
             G8RTOS_semaphore_wait(&semaphore_CC3100);
          }
        G8RTOS_semaphore_signal(&semaphore_CC3100);

        if(phone.header_data.intended_app == MUMESSAGE)
        {
            G8RTOS_add_thread( thread_receive_message_data, 20, "Receive Message");
        }
        else if(phone.header_data.intended_app == PONG)
        {
           if(phone.board_type == Host)
           {
               //receive thread for host
              // G8RTOS_add_thread(ReceiveDataFromClient, 20 ,"ReceiveFromClient");
               ReceiveDataFromClient();
           }
           else
           {
               //receive thread for client
               //G8RTOS_add_thread(ReceiveDataFromHost, 20 ,"ReceiveFromHost");
               ReceiveDataFromHost();
           }

        }

        G8RTOS_thread_sleep(sleep_time); //sleep 5 ms
    }


}
