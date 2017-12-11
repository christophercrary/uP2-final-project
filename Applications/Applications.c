//this file will hold all application related implementation that is shared amongst all (or some) of the applications on the phone
// most notably, this is where all wifi communication receive threads will be.


#include "Applications.h"
#include "G8RTOS.h"

semaphore_t semaphore_CC3100;          // used to access CC3100 WiFi chip

//this thread is used to setup certain shared aspects across applications (currently just wifi)
void thread_init_host_wifi()
{
    initCC3100(Host); //initialize CC3100 as the host

     // establish connection with client
     while(ReceiveData((uint8_t*)&client1, sizeof(client1)) < 0);

     // light up led to show WiFi connection
     P2->DIR |= (BIT0); //make p2.0 an output
     P2->OUT |= (BIT0);

     client1.hasAcknowledged = true;
     SendData((uint8_t*)&client1.hasAcknowledged, client1.IP_address, sizeof(client1.hasAcknowledged));

     //G8RTOS_add_thread(thread_receive_data, 50, "Receive data");


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

  //  G8RTOS_add_thread(thread_receive_data, 50, "Receive data");
    G8RTOS_kill_current_thread();

}





//this thread is responsible for receiving data
void thread_receive_data()
{

    Header_Data_t header_data; //variable that determines who the data is for
    while(1)
    {
        G8RTOS_semaphore_wait(&semaphore_CC3100);

        //read in data, the first byte that needs to be sent will determine which application the data is intended for
        while((ReceiveData((uint8_t*)&header_data, sizeof(header_data))) < 0)
         {
             //for reading data till a nonzero value is returned
             G8RTOS_semaphore_signal(&semaphore_CC3100);
             G8RTOS_thread_sleep(1); //sleep for 1ms to avoid deadlock
             G8RTOS_semaphore_wait(&semaphore_CC3100);
          }
        G8RTOS_semaphore_signal(&semaphore_CC3100);

        if(header_data.intended_app == MUMESSAGE)
        {
            G8RTOS_semaphore_wait(&semaphore_CC3100);

            ReceiveData((uint8_t*)&message_data.message[0], header_data.size_of_data);

            G8RTOS_semaphore_signal(&semaphore_CC3100);            //do something

            // message has now been sent, need to update the global message log
            for(int i = 0; i < message.data.header_info.size_of_data; i++)
            {
                old_messages.message_history[current_number_of_messages].old_message[i] = message_data.message[i];
            }

            // mark message as sent
            message_log[0].message_history[current_number_of_messages].message_status[current_number_of_messages++] = RECEIVED;

        }
        else if(header_data.intended_app == PONG)
        {
           // G8RTOS_add_thread(thread_receive_pong_data, 10, "pong_data");
            //do something

            G8RTOS_semaphore_wait(&semaphore_CC3100);

           // ReceiveData((uint8_t*)&rand, sizeof(rand));
            G8RTOS_semaphore_signal(&semaphore_CC3100);

        }

        G8RTOS_thread_sleep(100); //sleep 5 ms
    }


}
