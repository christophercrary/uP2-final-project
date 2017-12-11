//this file will hold all application related implementation that is shared amongst all (or some) of the applications on the phone
// most notably, this is where all wifi communication receive threads will be.


#include "Applications.h"
#include "G8RTOS.h"

semaphore_t semaphore_CC3100;          // used to access CC3100 WiFi chip
uint16_t count1 = 0;
uint16_t count2 = 0;
int16_t send_again = -5;

/*
 * NOTE:    The board with Chris' data MUST BE connected to the host first. Then Wes. This ensures that the contact data is valid, I.E. Wes wont be chris
 *          and chris wont be wes (not a fatal error but for sending messages to the correct contact will be important)
 */
void thread_init_host_wifi()
{
    initCC3100(Host); //initialize CC3100 as the host

    //establish connection with client1
     // establish connection with client1,


    //CLIENT1 NEEDS TO BE CHRIS
     while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0);

     client1.hasAcknowledged = true;
     SendData((uint8_t*)&client1.hasAcknowledged, client1.IP_address, sizeof(client1.hasAcknowledged));


     //establish connection with client2

     //CLIENT2 NEEDS TO BE WES
     //SendData((uint8_t*)&client1.hasAcknowledged, client2.IP_address, sizeof(client1.hasAcknowledged));

     while(ReceiveData((uint8_t*)&client2, sizeof(client2)) < 0);

     client2.hasAcknowledged = true;
   //  SendData((uint8_t*)&client2.hasAcknowledged, client2.IP_address, sizeof(client2.hasAcknowledged));
     SendData((uint8_t*)&client2.hasAcknowledged, client2.IP_address, sizeof(client2.hasAcknowledged)); //send client1 info to client2


     //both clients have communicated with the host.

     SendData((uint8_t*)&client2, client1.IP_address, sizeof(client2)); //send client2 to client1

      count1 = 0;

     client1.hasAcknowledged = false;
     while(!client1.hasAcknowledged)
     {
         while(ReceiveData((uint8_t*)&client1.hasAcknowledged, sizeof(client1.hasAcknowledged)) < 0);
         count1++;


     }


     SendData((uint8_t*)&client1, client2.IP_address, sizeof(client1)); //send client2 to client1
     client2.hasAcknowledged = false;
     while(!client2.hasAcknowledged)
     {
         while(ReceiveData((uint8_t*)&client2.hasAcknowledged, sizeof(client2.hasAcknowledged)) < 0);
         count2++;

     }


     // light up led to show WiFi connection
     P2->DIR |= (BIT0); //make p2.0 an output
     P2->OUT |= (BIT0);

     //G8RTOS_add_thread(thread_receive_data, 50, "Receive data");
     phone.IP_address = HOST_IP_ADDR;

     G8RTOS_kill_current_thread();

}


void thread_init_client_wifi()
{

    initCC3100(Client); //initialize CC3100 as the client

  /*
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

    client1.hasAcknowledged = false;
  //  G8RTOS_add_thread(thread_receive_data, 50, "Receive data");
*/



//if chris' board, need to wait for acknowlege from Host that all boards are ready (since chris' board is the second board to be connected)
   if(phone.self_contact == CHRIS)
   {
       client1.IP_address = getLocalIP();
       SendData((uint8_t *)&client1, HOST_IP_ADDR, sizeof(client1));


       while(ReceiveData((uint8_t*)&client1.hasAcknowledged, sizeof(client1.hasAcknowledged)) < 0);
       if(client1.hasAcknowledged)
       {
           //do something to show connection
           while(ReceiveData((uint8_t*)&client2, sizeof(client2)) < 0); //get client2 info

           client1.hasAcknowledged = true;
           SendData((uint8_t *)&client1.hasAcknowledged, HOST_IP_ADDR, sizeof(client1.hasAcknowledged));


       }
       else
       {
           while(ReceiveData((uint8_t*)&client1.hasAcknowledged, sizeof(client1.hasAcknowledged)) < 0);
       }

   }

   else if(phone.self_contact == WES)
   {
       client2.IP_address = getLocalIP();

     //  while(ReceiveData((uint8_t*)&client2.hasAcknowledged, sizeof(client2.hasAcknowledged)) < 0);


       SendData((uint8_t *)&client2, HOST_IP_ADDR, sizeof(client2));

       while(ReceiveData((uint8_t*)&client2.hasAcknowledged, sizeof(client2.hasAcknowledged)) < 0);
       if(client2.hasAcknowledged)
       {
           //do something to show connection
         //  while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0); //get client1 info
          // client2.hasAcknowledged = true;
           //SendData((uint8_t *)&client2.hasAcknowledged, HOST_IP_ADDR, sizeof(client2.hasAcknowledged));

           while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0); //get client2 info

           client2.hasAcknowledged = true;
           SendData((uint8_t *)&client2.hasAcknowledged, HOST_IP_ADDR, sizeof(client2.hasAcknowledged));

       }
       else
       {
           while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0);
       }

   }


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
            sleep_time = 2000; //500 ms sleep time
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
