/************************************************************************************
 *  File name: G8RTOS_scheduler.c
 *  Modified: 25 October 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for scheduling threads in the G8RTOS.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "G8RTOS_scheduler.h"
#include "G8RTOS_critical_section.h"
#include "G8RTOS_ipc.h" // used for FIFO_count
#include "msp.h"
#include "BSP.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////

#define THREADFREQ      1000        // 1000Hz -> 1ms
#define THUMBBIT        0x01000000         // used to set Thumb mode in PSR

// trigger PENDSV_handler to switch thread contexts
#define G8RTOS_CONTEXT_SWITCH()     SCB->ICSR |= 0x10000000;    // set PendSV bit

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////

extern void G8RTOS_START();     // exists in G8RTOS_scheduler_asm.s

// defined in G8RTOS_critical_section_asm.s
extern int32_t G8RTOS_START_CRITICAL_SECTION(void);
extern void G8RTOS_END_CRITICAL_SECION(int32_t primask);

extern tcb_t *running_thread_ptr;    // pointer to currently running thread TCB

////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

uint32_t system_time;       // holds the current time for the whole system
                            // this is initialized to zero upon calling G8RTOS_init

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////

// an array of TCBs to hold information for each thread
static tcb_t TCB[MAX_THREADS];

// an array of arrays that will hold each TCB's stack of STACKSIZE
static int32_t TCB_stacks[MAX_THREADS][STACKSIZE];

// counter to store current number of threads in scheduler
static uint32_t TCB_count;

// global thread ID counter as defined in Lab 4
static uint16_t tid_count;

// an array of PETs to hold information about each periodic event thread
static pet_t PET[MAX_PETS];

// counter to store current number of PETs in scheduler
static uint32_t PET_count;

// used to identify head of PET linked list
// PET list traversals use the head pointer
static pet_t *PET_head;

// used to identify tail of PET linked list
// G8RTOS_add_pet uses the tail pointer to add PETs
static pet_t *PET_tail;

// global PET ID counter
static uint16_t pid_count;


//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////

/************************************************************************************
 * Name: G8RTOS_systick_init()
 * Purpose: To initialize/enable the SysTick system in the MSP432. The SysTick
 *          interrupt is also enabled and set to the lowest interrupt priority (7)
 *          The programmer decides period of SysTick underflow, in terms of clock
 *          cycles. Note: Programmer must be aware of system_clock_frequency and is
 *          responsible for calculating the correct number of clock cycles.
 * Input(s): uint32_t clock_cycles
 * Output: systick_response_code_t response_code
 ************************************************************************************/
static int G8RTOS_systick_init(uint32_t clock_cycles)
{
    if (clock_cycles <= 0)
    {
        return -1;
    }
    else
    {
        SysTick->CTRL = 0;      // disable the SysTick system
        SysTick->VAL = 0;       // reset the SysTick counter (CVR)
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   // enable the TICK interrupt
        NVIC_SetPriority(SysTick_IRQn, OSINT_PRIORITY);   // set interrupt priority
        SysTick->LOAD = clock_cycles - 1;       // load the RVR register
        SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);   // system_clock, enable
    }

    return 0;
}

/************************************************************************************
 * Name: G8RTOS_scheduler
 * Purpose:  Choose the next thread to run. Current scheduling algorithm finds
 *           highest priority thread in thread list. No aging/starvation of threads
 *           is implemented in current scheduling algorithm. If the next thread is 
 *           blocked, the scheduler chooses the first non-blocked thread to run next.
 * Input(s): N/A
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_scheduler(void)
{
    uint32_t current_max_priority = 256;     // corresponds to lowest priority
    static tcb_t *temp;        // temp pointer to traverse current list of threads
    
    temp = running_thread_ptr;      // start traversal at currently running thread
    
    // search for highest thread that is not blocked nor sleeping
    for (int i = 0; i < TCB_count; i++)
    {
        // attempt to skip at least the currently running thread
        // if more than one thread, this allows round-robin for threads of equal priority
        temp = temp->next;
        
        // if next thread has higher priority and neither blocked nor asleep
        if ((temp->priority < current_max_priority) &&
            ((temp->blocked) == 0) &&
            ((temp->isAsleep == FALSE)))
        {
            // new lowest priority is current thread; update currently running thread
            running_thread_ptr = temp;

            // update new highest priority
            current_max_priority = temp->priority;
        }
    }
    
    return no_G8RTOS_err;       // return 0
                                // (in practice, should you error-check the scheduler?)
}

/********THIS FUNCTION WOULD BE USED TO ALLOW A THREAD A FULL 1MS TIME-SLOT**********
*************************************************************************************
 * Name: G8RTOS_thread_suspend
 * Purpose: To suspend a currently running thread, and allow the G8RTOS to run the
 *          next active thread. This function is called whenever a currently running
 *          thread's needed resource is blocked.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************
G8RTOS_response_code_t G8RTOS_thread_suspend(void)
{
    // reset SysTick counter as to allow the next thread a full 1ms time-slot
    SysTick->VAL = 0;
    
    // trigger the thread scheduler by setting the SysTick interrupt flag
    SCB->ICSR |= 0x04000000;        // what is the group configuration for this??
    
    return no_G8RTOS_err;
}
*/


//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
 * Name: G8RTOS_add_aperiodic_thread
 * Purpose: Function to potentially add an aperiodic thread to the scheduler.
 *          The aperiodic thread must configure a valid IRQ number (0 - 40)
 *          in the MSP432 processor.
 *          The aperiodic thread must have a valid user IRQ priority
 *          (0 <= priority < 7) (priority = 7 is reserved for the SysTick ISR)
 * Input(s): void (*aperiodic_thread_function)(void),
             uint8_t priority, IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_add_aperiodic_thread(void (*aperiodic_thread_function)(void),
                                                   IRQn_Type IRQn, uint8_t priority)
{
    // verify specified IRQ number is valid
    if ((IRQn < PSS_IRQn) || (IRQn > PORT6_IRQn))
    {
        return aperiodic_invalid_irq_number;        // return -9
    }
    // verify specified aperiodic priority is valid
    if (priority > MAX_APERIODIC_THREAD_PRIORITY)
    {
        return aperiodic_invalid_priority;      // return -10
    }

    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();    // save current status of PRIMASK
                                                                 // start atomic operations

    // configure specified IRQ to point to specified aperiodic thread function (ISR)
    __NVIC_SetVector(IRQn, (uint32_t)(aperiodic_thread_function));

    // set specified aperiodic thread priority
    NVIC_SetPriority(IRQn, priority);

    // enable specified aperiodic thread ISR
    NVIC_EnableIRQ(IRQn);

    G8RTOS_END_CRITICAL_SECTION(primask_status);   // restore previous status of PRIMASK
                                                   // end atomic operations

    return no_G8RTOS_err;       // return 0

}

/************************************************************************************
 * Name: G8RTOS_disable_aperiodic_thread
 * Purpose: Function to disable a specific aperiodic thread's IRQ
 * Input(s): IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_disable_aperiodic_thread(IRQn_Type IRQn)
{
    NVIC_DisableIRQ(IRQn);

    return no_G8RTOS_err;       // return 0
}

/************************************************************************************
 * Name: G8RTOS_disable_aperiodic_thread
 * Purpose: Function to enable a specific aperiodic thread's IRQ.
 *          Note: the aperiodic thread MUST have already been added by making a call
 *          to G8RTOS_add_aperiodic_event.
 * Input(s): IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_enable_aperiodic_thread(IRQn_Type IRQn)
{
    NVIC_EnableIRQ(IRQn);

    return no_G8RTOS_err;       // return 0
}

/************************************************************************************
 * Name: G8RTOS_add_pet
 * Purpose: Add a periodic event thread to the array of periodic event threads PET[].
 *          If multiple PETs share a common multiple of execution time,
 *          it is the responsibility of the programmer to determine an appropriate
 *          time offset for the PET, as to avoid sharing a common multiple of
 *          execution time.
 *          QUESTION: Should there be semaphores implemented for the head and tail
 *                    pointers???
 * Input(s): void (*handler)(void), uint32_t period, uint32_t time_offset
 * Output: G8RTOS_response_code_t response_code
 ***********************************************************************************/
G8RTOS_response_code_t G8RTOS_add_pet(void (*handler)(void), uint32_t period,
                                      uint32_t time_offset, char *pet_name)
{
    // check if there is any space to add a periodic event thread
    if (PET_count >= MAX_PETS)    // if no space
    {
        return max_pet_err;   // max_pet_err = -5
    }

    // should you error-check for the specified name being a valid length ??
    // The user MUST leave space for NULL character (MAX_NAME_LENGTH - 1)
    // NOTE: the null character is automatically passed in string literals

    // counter used to determine which PET is available in PET list (used for list traversal)
    static uint32_t temp_pet_counter;

    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();    // save current status of PRIMASK
                                                                 // start atomic operations

    // if no elements yet exist in PET list
    if (PET_count == 0)
    {
        PET_head = &PET[0];     // used to identify head in PET linked list
        PET_tail = &PET[0];     // used to identify tail in PET linked list
        temp_pet_counter = 0;
        PET[0].next = &PET[0];
        PET[0].prev = &PET[0];
    }
    else    // if other elements exist in PET list
    {
        // find first "dead" (available) PET to be instantiated
        // arbitrarily start list traversal from beginning of PET list
        // there is guaranteed to be a "dead" PET, since PET_count < MAX_PETS
        for (temp_pet_counter = 0; temp_pet_counter < MAX_PETS; temp_pet_counter++)
        {
            if (PET[temp_pet_counter].isAlive == FALSE)
            {
                break;      // first "dead" PET found, retain temp_thread_counter value
            }
        }

        // add the new PET at the end of the PET list (also before the head)
        // update necessary PET linked list pointers (UPDATE!!!)
        PET_head->prev->next = &PET[temp_pet_counter];
        PET[temp_pet_counter].prev = PET_head->prev;
        PET[temp_pet_counter].next = PET_head;
        PET_head->prev = &PET[temp_pet_counter];
        PET_tail = &PET[temp_pet_counter];      // update tail of PET list
    }

    // activate PET (make PET "alive")
    PET[temp_pet_counter].isAlive = TRUE;

    PET[temp_pet_counter].time_offset = time_offset;       // assign time-offset
    PET[temp_pet_counter].execution_time = system_time + period + time_offset;      // assign PET execution time
    PET[temp_pet_counter].period = period;     // assign passed-in period
    PET[temp_pet_counter].handler = handler;       // assign PET function

    // assign new PET's ID
    PET[temp_pet_counter].pet_id = (((pid_count++) << 16) | temp_pet_counter);

    // assign new PET's name (from passed-in char array)
    for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++)
    {
        PET[temp_pet_counter].pet_name[i] = pet_name[i];
    }

    PET_count = PET_count + 1;      // update current PET count

    G8RTOS_END_CRITICAL_SECTION(primask_status);   // restore previous status of PRIMASK
                                                   // end atomic operations

    return no_G8RTOS_err;           // return 0

}

/************************************************************************************
 * Name: G8RTOS_add_thread
 * Purpose: Function to potentially add a thread to the scheduler. If a TCB can be
 *          added, all of the TCB's data members are set up and initialized.
 *          A void pointer to a task (a function) is passed into G8RTOS_add_thread.
 *          This pointer will be assigned to the PC of the thread's TCB.
 * Input(s): void (*thread_function)(void),
             uint8_t priority, char *thread_name
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_add_thread(void (*thread_function)(void),
                                         uint8_t priority, char *thread_name)
{
    // check if there is any space to add a thread_function
    if (TCB_count >= MAX_THREADS)    // if no space
    {
        return max_thread_err;   // max_thread_err = -3
    }
    // check if attempted thread priority is valid
    if (priority > MAX_THREAD_PRIORITY)
    {
        return thread_invalid_priority;     // return -8
    }

    // should you error-check for the specified name being a valid length ??
    // The user MUST leave space for NULL character (MAX_NAME_LENGTH - 1)
    // NOTE: the null character is automatically passed in string literals


    // counter used to determine which TCB is available in TCB list
    static uint32_t temp_thread_counter;
    
    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();    // save current status of PRIMASK
                                                                // start atomic operations
    
    // check if there is no currently running thread exists
    if (running_thread_ptr == NULL)
    {
        // no TCBs exist
        // add first thread to head of TCB list
        TCB[0].next = &TCB[0];
        TCB[0].prev = &TCB[0];
        running_thread_ptr = &TCB[0];
        temp_thread_counter = 0;     // thread added to first element in TCB list
    }
    // otherwise, there exists at least one TCB in TCB list
    else
    {
       // find first "dead" (available) thread to be instantiated
       // arbitrarily start list traversal from head of TCB_list
       // there is guaranteed to be a "dead" thread, since TCB_count < MAX_THREADS
        for (temp_thread_counter = 0; temp_thread_counter < MAX_THREADS; temp_thread_counter++)
        {
            if (TCB[temp_thread_counter].isAlive == FALSE)
            {
                break;      // first "dead" thread found, retain temp_thread_counter value
            }
        }

        // add the new TCB before currently running thread
        // update necessary TCB linked list pointers


        running_thread_ptr->prev->next = &TCB[temp_thread_counter];
        TCB[temp_thread_counter].prev = running_thread_ptr->prev;
        TCB[temp_thread_counter].next = running_thread_ptr;
        running_thread_ptr->prev = &TCB[temp_thread_counter];


    }

    // activate thread (make thread "alive")
    TCB[temp_thread_counter].isAlive = TRUE;

    // initialize no blocking or sleeping conditions
    TCB[temp_thread_counter].blocked = 0;
    TCB[temp_thread_counter].isAsleep = FALSE;

    // adjust new thread's stack pointer
    TCB[temp_thread_counter].sp = &TCB_stacks[temp_thread_counter][STACKSIZE-16];

    // initialize the new TCB's stack
    // from first pushed to last pushed (to the BOTTOM of the stack):
    // PSR (include THUMBBIT), PC, LR, R12, R3-R0, R11-R4
    TCB_stacks[temp_thread_counter][STACKSIZE-1] = THUMBBIT;   // save THUMBBIT within PSR
    TCB_stacks[temp_thread_counter][STACKSIZE-2] = (int32_t)(thread_function);   // point PC to thread's function
    TCB_stacks[temp_thread_counter][STACKSIZE-3] = 0x14141414;     // default, not-used value for initial LR
    TCB_stacks[temp_thread_counter][STACKSIZE-4]  = 0x12121212;    // default, not-used value for R12
    TCB_stacks[temp_thread_counter][STACKSIZE-5]  = 0x03030303;    // ...
    TCB_stacks[temp_thread_counter][STACKSIZE-6]  = 0x02020202;
    TCB_stacks[temp_thread_counter][STACKSIZE-7]  = 0x01010101;
    TCB_stacks[temp_thread_counter][STACKSIZE-8]  = 0x00000000;
    TCB_stacks[temp_thread_counter][STACKSIZE-9]  = 0x11111111;
    TCB_stacks[temp_thread_counter][STACKSIZE-10] = 0x10101010;
    TCB_stacks[temp_thread_counter][STACKSIZE-11] = 0x09090909;
    TCB_stacks[temp_thread_counter][STACKSIZE-12] = 0x08080808;
    TCB_stacks[temp_thread_counter][STACKSIZE-13] = 0x07070707;
    TCB_stacks[temp_thread_counter][STACKSIZE-14] = 0x06060606;
    TCB_stacks[temp_thread_counter][STACKSIZE-15] = 0x05050505;
    TCB_stacks[temp_thread_counter][STACKSIZE-16] = 0x04040404;

    // stack pointer (sp) now points to correct location on pre-defined stack

    // assign new thread's priority
    TCB[temp_thread_counter].priority = priority;
    
    // assign new thread's ID (as defined in Lab 4)
    // why can't we just assign tid_count to thread_id?
    TCB[temp_thread_counter].thread_id = (((tid_count++) << 16) | temp_thread_counter);

    // assign new thread's name (from passed-in char array)
    for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++)
    {
        TCB[temp_thread_counter].thread_name[i] = thread_name[i];
    }

    // update current thread count
    TCB_count = TCB_count + 1;
    
    G8RTOS_END_CRITICAL_SECTION(primask_status);   // restore previous status of PRIMASK
                                                   // end atomic operations

    return no_G8RTOS_err;           // return 0
    
    }

/************************************************************************************
* Name: G8RTOS_get_tid
* Purpose: Get thread ID of currently running thread (running_thread_ptr)
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
tid_t G8RTOS_get_tid(void)
{
    return running_thread_ptr->thread_id;
}

/************************************************************************************
* Name: G8RTOS_init
* Purpose: Initializes necessary variables and hardware for G8RTOS.
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_init(void)
{
    // use pre-defined functions to initialize all necessary board hardware components
    BSP_InitBoard();
    
    // relocate the MSP432's interrupt vector table to 0x20000000
    // aperiodic threads can only be configured when vector table resides in SRAM!
    uint32_t newVTORTable = 0x20000000;
    memcpy((uint32_t *)newVTORTable, (uint32_t *)SCB->VTOR, 57*4);  // 57 interrupt vectors to copy
    SCB->VTOR = newVTORTable;       // re-point vector table

    // disable interrupts until OS is started (see G8RTOS_START)
    // no need to save previous PRIMASK
    G8RTOS_START_CRITICAL_SECTION();
    
    // set system clock to run at 48MHz
    ClockSys_SetMaxFreq();
    
    // initialize system time
    system_time = 0;
    
    // initialize number of threads
    TCB_count = 0;
    
    // initialize global thread ID counter (as defined in Lab 4)
    tid_count = 0;
    
    // initialize number of periodic event threads
    PET_count = 0;

    // initialize global PET ID counter
    pid_count = 0;

    // initialize number of FIFOs
    FIFO_count = 0;
    
    // initialize all threads in TCB[] to be "dead" (available)
    for (uint8_t i = 0; i < MAX_THREADS; i++)
    {
        TCB[i].isAlive = FALSE;     // thread is dead, baby - Bruce Willis, Pulp Fiction
    }

    // initialize all PETs in PET[] to be "dead" (available)
    for (uint8_t i = 0; i < MAX_PETS; i++)
    {
        PET[i].isAlive = FALSE;
    }

    // initialize running thread pointer (currently running thread) to NULL
    // NULL defined in G8RTOS_scheduler.h
    running_thread_ptr = NULL;

    // initialize PET head and tail pointers to NULL
    // used to dynamically add/remove PETs in PET list
    PET_head = NULL;
    PET_tail = NULL;

    return no_G8RTOS_err;       // return 0
    
}

/************************************************************************************
* Name: G8RTOS_kill_all_other_threads
* Purpose: Attempts to kill all other threads besides currently running thread.
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_all_other_threads()
{
    /* Critical Section */
    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();

    /* Check if this is last thread (we cannot have all threads killed in this scheduler */
    if (TCB_count == 1)
    {
    G8RTOS_END_CRITICAL_SECTION(primask_status);
        return cannot_kill_thread;
    }

    tcb_t *threadPtr = running_thread_ptr;
    for(int i = 1; i < TCB_count; i++)
    {
        /* Set alive bit of next TCB to false*/
        threadPtr->next->isAlive= FALSE;

        threadPtr = threadPtr->next;
    }

    /* Update Number of Threads */
    TCB_count = 1;

    /* Set currently running thread's next TCB to itself */
    running_thread_ptr->next = running_thread_ptr;

    /* Set currently running thread's previous TCB to itself */
    running_thread_ptr->prev = running_thread_ptr;

    G8RTOS_END_CRITICAL_SECTION(primask_status);

    return no_G8RTOS_err;
}

/************************************************************************************
* Name: G8RTOS_kill_pet
* Purpose: Attempts to kill specified PET
* Input(s): pid_t pet_id
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_pet(pid_t pet_id)
{
    // check if no PETs are running
    if (PET_count < 1)
    {
        return cannot_kill_pet;     // return -11
    }

    // enter critical section
    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();

    // create temporary PET to traverse active PETs in PET list
    pet_t *temp = PET_head;

    // attempt to find specified PET to kill
    for (uint8_t i = 0; i < PET_count; i++)
    {
        // if specified PET to kill is found
        if (temp->pet_id == pet_id)
        {
            // kill specified PET
            temp->isAlive = FALSE;

            // update number of PETs
            PET_count = PET_count - 1;

            // THIS DOESN'T ACCOUNT FOR ONLY SINGLE ELEMENT!!! FIX!!!

            // update necessary pointers in PET list
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            // end critical section (cannot return no_G8RTOS_err response code)
            G8RTOS_END_CRITICAL_SECTION(primask_status);

        }

        // otherwise, point to next active PET in PET list
        temp = temp->next;
    }

    // if list traversal completes, specified PET was not found
    return pet_nonexistent;      // return -12
}


/************************************************************************************
* Name: G8RTOS_kill_current_thread
* Purpose: Attempts to kill currently running thread
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_current_thread(void)
{

    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();


    // check if less than two threads are running
    if (TCB_count == 1)
    {

        G8RTOS_END_CRITICAL_SECTION(primask_status);

        return cannot_kill_thread;     // return -6
    }


    // kill currently running thread
    running_thread_ptr->isAlive = FALSE;



    // is this necessary???

//    if (running_thread_ptr->blocked != 0)
//    {
//        *(running_thread_ptr->blocked) += 1;
//    }


    // update necessary pointers in TCB list
    running_thread_ptr->prev->next = running_thread_ptr->next;
    running_thread_ptr->next->prev = running_thread_ptr->prev;

    // update number of threads
    TCB_count = TCB_count - 1;

    // set context switch (will trigger after end of critical section)
    G8RTOS_CONTEXT_SWITCH();

    // end critical section (cannot return no_G8RTOS_err response code)
    G8RTOS_END_CRITICAL_SECTION(primask_status);

    return no_G8RTOS_err;       // return 0 (never executed)
}

/************************************************************************************
* Name: G8RTOS_kill_thread
* Purpose: Attempts to kill specified thread
* Input(s): tid_t thread_id
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_thread(tid_t thread_id)
{
    // check if less than two threads are running
    if (TCB_count < 2)
    {
        return cannot_kill_thread;     // return -6
    }

    // enter critical section
    int32_t primask_status = G8RTOS_START_CRITICAL_SECTION();

    // create temporary TCB to traverse active threads in TCB list
    tcb_t *temp = running_thread_ptr;

    // attempt to find specified thread to kill
    for (uint8_t i = 0; i < TCB_count; i++)
    {
        // if specified thread to kill is found
        if (temp->thread_id == thread_id)
        {
            // kill specified thread
            temp->isAlive = FALSE;

            // update number of threads
            TCB_count = TCB_count - 1;

            // increment semaphore, if temp thread is blocked
            if (temp->blocked != 0)
            {
                *(temp->blocked) += 1;
            }

            // update necessary pointers in TCB list
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;



            // if killing the currently running thread,
            if (temp == running_thread_ptr)
            {
                // set context switch (will trigger after end of critical section)
                G8RTOS_CONTEXT_SWITCH();
            }

            // end critical section (cannot return no_G8RTOS_err response code)
            G8RTOS_END_CRITICAL_SECTION(primask_status);

        }

        // otherwise, point to next active thread in TCB list
        temp = temp->next;

    }

    // if list traversal completes, specified thread was not found
    return thread_nonexistent;      // return -7
}

/************************************************************************************
* Name: G8RTOS_launch
* Purpose: Launch the G8RTOS system. The SysTick system is configured, and the
*          context is set to the highest priority thread in the initial TCB list.
*          If there are no active threads, function returns missing_thread_err (-4).
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_launch(void)
{
    // should you error-check for G8RTOS having already been launched? (use a boolean?)

    // check if there are active threads
    if (TCB_count == 0)            // if no active threads
    {
       return missing_thread_err;      // return -4
    }
    
    // initialize SysTick system timer and its interrupt to the lowest priority (7)
    // the period of underflow is configured for 1 ms
    G8RTOS_systick_init((ClockSys_GetSysFreq() / THREADFREQ));
    
    // initialize PendSV interrupt to the lowest priority (7)
    NVIC_SetPriority(PendSV_IRQn, OSINT_PRIORITY);
    
    uint32_t current_max_priority = 256;     // corresponds to lowest priority
    tcb_t *temp;        // temp pointer to traverse current list of threads
    
    temp = &TCB[0];      // start traversal at beginning of initial TCB list
    
    // search for highest thread (no need to check for sleeping or blocking threads)
    for (uint8_t i = 0; i < TCB_count; i++)
    {
        // if current thread has higher priority than current max_priority
        if (temp->priority < current_max_priority)
        {
            // new lowest priority is current thread's priority
            current_max_priority = temp->priority;
                
            // new thread has higher priority, update currently running thread (not yet running)
            running_thread_ptr = temp;
        }
        
        // point to next thread in initial TCB list
        temp = temp->next;
    }
            
    
    // running_thread_ptr is set to thread with highest priority
    // G8RTOS_START references running_thread_ptr
    G8RTOS_START();     // set context to highest priority thread in TCB list
                        // interrupts are enabled after context is set
    
    return no_G8RTOS_err;       // return 0
}

/************************************************************************************
 * Name: G8RTOS_thread_sleep
 * Purpose: To sleep a currently running thread, and allow the G8RTOS to remain
 *          for a finite amount of time. A thread in the sleep state will NOT be run.
 *          The thread will become active again in the G8RTOS_scheduler function,
 *          after the system_time matches the sleep_finish_time, which is located
 *          in the specific TCB's structure.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_thread_sleep(uint32_t sleep_duration)
{
    // assign sleep finish time to currently running thread
    running_thread_ptr->sleep_finish_time = system_time + sleep_duration;

    // put thread asleep
    running_thread_ptr->isAsleep = TRUE;

    // suspend the running thread to allow for the necessary context switch
    // this method does not give the next thread a full 1ms time-slot
    // implementation of G8RTOS_thread_suspend would fix this issue
    G8RTOS_CONTEXT_SWITCH();

    return no_G8RTOS_err;
}


/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////


////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////

// every 1 ms
void SysTick_Handler()
{
    static pet_t *temp_pet;     // used to traverse active PET list

    system_time++;      // increment system time every 1ms
    
    if (PET_count > 0)      // verify that there are active periodic event threads
    {
        // call any necessary periodic event threads (starting from head of the PET list)
        temp_pet = PET_head;

        for (uint8_t i = 0; i < PET_count; i++)     // is this the best looping technique?
        {
            if (system_time == temp_pet->execution_time)    // check if PET should be run
            {
                // update PET execution time
                temp_pet->execution_time = system_time +
                                           temp_pet->period +
                                           temp_pet->time_offset;

                temp_pet->handler();        // run PET function
            }

            temp_pet = temp_pet->next;      // check next active PET
        }
    }

    // wake up any necessary threads (starting from 0th thread every time)
    // using the TCB array will not be functional if user is allowed to remove TCBs

   // tcb_t *temp_tcb = &TCB[0];

    /***** THIS NEEDS TO START AT THE CURRENTLY RUNNING THREAD OR YOUR SHIT WONT WORK, CHRIS *****/
    tcb_t *temp_tcb = running_thread_ptr;
    
    for (uint8_t i = 0; i < TCB_count; i++)
    {
        if ( (temp_tcb->isAsleep) && (system_time == temp_tcb->sleep_finish_time) )
        {
            temp_tcb->isAsleep = FALSE;      // wake up thread
        }

        temp_tcb = temp_tcb->next;      // check next TCB
    }
    
    // switch contexts to next thread
    G8RTOS_CONTEXT_SWITCH();
}

////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

/////////////////////////////END OF G8RTOS_scheduler.c///////////////////////////////
