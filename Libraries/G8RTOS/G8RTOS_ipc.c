/************************************************************************************
 *  File name: G8RTOS_ipc.h
 *  Modified: 25 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for inter-process communication
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "G8RTOS_ipc.h"
#include "G8RTOS_critical_section.h"    // used for critical section functions

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////

#define MAX_FIFOS   4       // max number of FIFOs allowed in G8RTOS

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////

// defined in G8RTOS_critical_section_asm.s
extern int32_t G8RTOS_START_CRITICAL_SECTION(void);
extern void G8RTOS_END_CRITICAL_SECION(int32_t primask);

////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////

// an array of FIFOs
static fifo_t FIFO[MAX_FIFOS];

//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////


////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
 * Name: G8RTOS_add_fifo
 * Purpose: Add a FIFO to the FIFO array FIFO[], using a passed-in index.
 * Input(s): uint32_t index
 * Output: FIFO_response_code_t response_code
 ***********************************************************************************/
FIFO_response_code_t G8RTOS_add_fifo(uint32_t index)
{
    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();    // save current status of PRIMASK
                                                                 // start atomic operations
    // check if there is any space to add a FIFO
    if (FIFO_count >= MAX_FIFOS)
    {
        return fifo_max_fifo_err;          // return -1
    }

    // new FIFO will be added to FIFO[index]

    // point head and tail pointers to beginning of FIFO[index] buffer
    FIFO[index].head = &(FIFO[index].buffer[0]);
    FIFO[index].tail = &(FIFO[index].buffer[0]);

    // initialize size of FIFO[index] (recall that current_size is a semaphore)
    G8RTOS_semaphore_init(&(FIFO[index].current_size), 0);

    // initialize mutex of FIFO[index] to be available
    G8RTOS_semaphore_init(&(FIFO[index].mutex), 1);

    // initialize count of lost data to 0
    FIFO[index].lost_data_count = 0;

    // increase FIFO count
    FIFO_count = FIFO_count + 1;

    G8RTOS_END_CRITICAL_SECTION(primask_status);   // restore previous status of PRIMASK
                                                   // end atomic operations

    return no_fifo_err;       // return 0

}

/************************************************************************************
 * Name: G8RTOS_fifo_read
 * Purpose: Read from a given FIFO in FIFO[].
 * Input(s): uint32_t index
 * Output: int32_t data
 ***********************************************************************************/
int32_t G8RTOS_fifo_read(uint32_t index)
{
    int32_t data;       // variable to store data to be read from FIFO[index]

    G8RTOS_semaphore_wait(&(FIFO[index].current_size));    // block if FIFO is empty
    G8RTOS_semaphore_wait(&(FIFO[index].mutex));    // block if FIFO is already being used

    data = *(FIFO[index].head++);     // read oldest data from FIFO (post-increment)

    // check if updated head needs to wrap around to front of buffer
    if (FIFO[index].head == &(FIFO[index].buffer[FIFOSIZE]))
    {
        FIFO[index].head = &(FIFO[index].buffer[0]);        // wrap to front of FIFO
    }

    G8RTOS_semaphore_signal(&(FIFO[index].mutex));   // signal that FIFO is available

    return data;       // return read data from FIFO[index]
}

/************************************************************************************
 * Name: G8RTOS_fifo_write
 * Purpose: Write data to a given FIFO in FIFO[]. If FIFO is full, data will be lost
 *          and the lost_data_count in the specific FIFO  will be incremented.
 *          Note that in the current implementation, there is only one producer
 *          for each FIFO, thus it is unnecessary to use a mutex for writing to
 *          a FIFO.
 * Input(s): uint32_t index, int32_t data
 * Output: FIFO_response_code_t response_code
 ***********************************************************************************/
FIFO_response_code_t G8RTOS_fifo_write(uint32_t index, int32_t data)
{
    // check if there is no available space in FIFO for writing new data
    if (FIFO[index].current_size == FIFOSIZE - 1)       // still not convinced about the -1
    {
        FIFO[index].lost_data_count++;      // if no space, new data will be lost
        return fifo_lost_data_err;          // return -7
    }

    // otherwise, there is available space in FIFO for writing new data
    
    // wait for FIFO to be available for use
    //G8RTOS_semaphore_wait(&(FIFO[index].mutex));

    *((FIFO[index].tail)++) = data;  // write newest data using tail (post increment)

    // check if updated tail needs to wrap around to front of buffer
    if (FIFO[index].tail == &(FIFO[index].buffer[FIFOSIZE]))
    {
        FIFO[index].tail = &(FIFO[index].buffer[0]);        // wrap to front of FIFO
    }

    // signal that FIFO is available for use
    //G8RTOS_semaphore_signal(&(FIFO[index].mutex));
    
    // increase current_size of FIFO[index] (increment blocking semaphore)
    G8RTOS_semaphore_signal(&(FIFO[index].current_size));

    return no_fifo_err;       // return 0
}

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////


/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////


//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

////////////////////////////////END OF G8RTOS_ipc.c////////////////////////////////////
