/************************************************************************************
 *  File name: G8RTOS_ipc.h
 *  Modified: 25 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful functions/structures for inter-process communication
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "msp.h"    // used for int32_t
#include "G8RTOS_semaphores.h"      // used for semaphore_t

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////

//header guard
#ifndef G8RTOS_IPC_H_
#define G8RTOS_IPC_H_

#define FIFOSIZE    16      // size of each FIFO buffer

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

// FIFO error response codes
typedef enum {
    fifo_lost_data_err = -2,     // FIFO was full, data was lost on write
    fifo_max_fifo_err = -1,      // max amount of FIFOs in FIFO[]
    no_fifo_err = 0
}FIFO_response_code_t;

// FIFO structure
typedef struct fifo {
    int32_t buffer[FIFOSIZE];       // buffer to store fifo's data
    int32_t volatile *head;     // head of buffer (points to oldest data)
    int32_t volatile *tail;     // tail of buffer (points to newest data)
    semaphore_t current_size;       // store number of data currently in buffer (semaphore)
    semaphore_t mutex;          // mutex used to prevent two consumers from reading same FIFO data
    uint32_t lost_data_count;     // store number of lost data
}fifo_t;


// counter to store current number of FIFOs in scheduler
// IS THERE A WAY TO MAKE THIS STATIC/AVAILABLE IN SCHEDULER EASIER??
uint32_t FIFO_count;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////

/************************************************************************************
 * Name: G8RTOS_add_fifo
 * Purpose: Add a FIFO to the FIFO array FIFO[], using a passed-in index.
 * Input(s): uint32_t index
 * Output: FIFO_response_code_t response_code
 ***********************************************************************************/
FIFO_response_code_t G8RTOS_add_fifo(uint32_t index);

/************************************************************************************
 * Name: G8RTOS_fifo_read
 * Purpose: Read from a given FIFO in FIFO[].
 * Input(s): uint32_t index
 * Output: int32_t data
 ***********************************************************************************/
int32_t G8RTOS_fifo_read(uint32_t index);

/************************************************************************************
 * Name: G8RTOS_fifo_write
 * Purpose: Write data to a given FIFO in FIFO[]. If FIFO is full, data will be lost
 *          and the lost_data_count in the specific FIFO  will be incremented.
 * Input(s): uint32_t index, int32_t data
 * Output: FIFO_response_code_t response_code
 ***********************************************************************************/
FIFO_response_code_t G8RTOS_fifo_write(uint32_t index, int32_t data);

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

#endif		// end of header guard

////////////////////////////////END OF G8RTOS_ipc.h////////////////////////////////////
