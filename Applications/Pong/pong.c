/************************************************************************************
 *  File name: pong.c
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary & Brit Chesley
 *  Purpose: Source file for Pong application, created by Brit Chesley and
 *           Chris Crary.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

// IMPLEMENT: CLEAN UP THESE DEPENDENCIES
#include "pong.h"
#include <stdlib.h>
#include "time.h"
#include "BSP.h"
#include "cc3100_usage.h"

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
extern semaphore_t semaphore_CC3100;
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

/* global variables */
static GameState_t gameState;         // global gamestate used by host and client separately

static uint8_t client_overall_scores[2];       // holds scores of each player

// static boolean to tell if the host has pressed button 1
static bool button_pressed = false;

static Header_Data_t header_data;

/* semaphores */
semaphore_t semaphore_gameState;       // used to access overall packet of gamestate information


/************************** Static Host IP Address *****************************/
//192.168.1.2


//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/*
 * Updates player's paddle based on current and new center
 */
void UpdatePlayerOnScreen(PrevPlayer_t *prevPlayer, Player_t *currentPlayer)
{

    // displacement between current and previous player centers
    int16_t displacement = currentPlayer->currentCenterX - prevPlayer->centerX;

    // variable to store coordinate of leftmost point of paddle
    int16_t start_of_paddle = prevPlayer->centerX - PADDLE_LEN_D2;

    // variable to store coordinate of rightmost point of paddle
    int16_t end_of_paddle = prevPlayer->centerX + PADDLE_LEN_D2;

    if(displacement > 0)        // move paddle to the right
    {

        if(currentPlayer->position == BOTTOM)
        {


        // add to right-side of pre-existing paddle
        LCD_DrawRectangle(end_of_paddle - 4*WIGGLE_ROOM,
                          end_of_paddle + displacement,
                          BOTTOM_PADDLE_Y_MIN,
                          BOTTOM_PADDLE_Y_MAX,
                          currentPlayer->color);

        // erase from left-side of pre-existing paddle
        LCD_DrawRectangle(start_of_paddle ,
                          start_of_paddle + displacement,
                          BOTTOM_PADDLE_Y_MIN,
                          BOTTOM_PADDLE_Y_MAX,
                          ARENA_BACK_COLOR);
        }
        else
        {

            // add to right-side of pre-existing paddle
            LCD_DrawRectangle(end_of_paddle - 4*WIGGLE_ROOM,
                              end_of_paddle + displacement,
                              TOP_PADDLE_Y_MIN,
                              TOP_PADDLE_Y_MAX,
                              currentPlayer->color);

            // erase from left-side of pre-existing paddle
            LCD_DrawRectangle(start_of_paddle,
                              start_of_paddle + displacement,
                              TOP_PADDLE_Y_MIN,
                              TOP_PADDLE_Y_MAX,
                              ARENA_BACK_COLOR);


        }
    }
    else if(displacement < 0)       // move paddle to the left
    {

        displacement *= -1;       // used just for easier readability below

        if(currentPlayer->position == BOTTOM)
        {

            // erase from the right-side of pre-existing paddle
            LCD_DrawRectangle(end_of_paddle - displacement,
                              end_of_paddle,
                              BOTTOM_PADDLE_Y_MIN,
                              BOTTOM_PADDLE_Y_MAX,
                              ARENA_BACK_COLOR);

            // add to left-side of pre-existing paddle
            LCD_DrawRectangle(start_of_paddle - displacement,
                              start_of_paddle + 4*WIGGLE_ROOM,
                              BOTTOM_PADDLE_Y_MIN,
                              BOTTOM_PADDLE_Y_MAX,
                              currentPlayer->color);
        }
        else
        {

            // erase from the right-side of pre-existing paddle
              LCD_DrawRectangle(end_of_paddle - displacement,
                                end_of_paddle,
                                TOP_PADDLE_Y_MIN,
                                TOP_PADDLE_Y_MAX,
                                ARENA_BACK_COLOR);

              // add to left-side of pre-existing paddle
              LCD_DrawRectangle(start_of_paddle - displacement,
                                start_of_paddle + 4*WIGGLE_ROOM,
                                TOP_PADDLE_Y_MIN,
                                TOP_PADDLE_Y_MAX,
                                currentPlayer->color);
        }

    }

    // update previous player position
    prevPlayer->centerX = currentPlayer->currentCenterX;

    return;
}

/*
 * Initializes and prints initial game state
 */
void InitBoardState()
{

    phone.current_app = PONG;

    // initialize necessary game semaphores
    G8RTOS_semaphore_init(&semaphore_gameState, 1);
   // G8RTOS_semaphore_init(&semaphore_CC3100,1);

    //initialize header data
    header_data.intended_app = PONG;
    if(phone.board_type == Host)
    {
        header_data.size_of_data = sizeof(ClientInfo_t);
    }
    else
    {
        //client
        header_data.size_of_data = sizeof(GameState_t);

    }

    // initialize which position the players are in in the gameState
    gameState.players[BOTTOM].position = BOTTOM;
    gameState.players[TOP].position = TOP;

    // initialize starting position of player paddles
    gameState.players[TOP].currentCenterX = PADDLE_X_CENTER;
    gameState.players[BOTTOM].currentCenterX = PADDLE_X_CENTER;

    // initialize player paddle colors
    gameState.players[TOP].color = PLAYER_RED;
    gameState.players[BOTTOM].color = PLAYER_BLUE;

    // initialize LED scores
    gameState.LEDScores[BOTTOM] = 0;
    gameState.LEDScores[TOP] = 0;

    // initialize amount of balls on screen
    gameState.ballCount = 0;

    // initialize game over flag
    gameState.gameDone = FALSE;

    // initialize game winner to NULL (also equal to HOST)
    gameState.winner = NULL;

    // initialize all static balls to "dead" (available)
    for (uint8_t i = 0; i < MAX_NUM_OF_BALLS; i++)
    {
        gameState.balls[i].isAlive = false;
    }

    // establish Internet connection with client

    // make screen all black
    LCD_Clear(LCD_BLACK);

    // draw edges of game arena
    LCD_DrawRectangle(ARENA_MIN_X-1, ARENA_MIN_X, ARENA_MIN_Y, ARENA_MAX_Y, ARENA_BOUNDARY_COLOR);
    LCD_DrawRectangle(ARENA_MAX_X, ARENA_MAX_X+1, ARENA_MIN_Y, ARENA_MAX_Y, ARENA_BOUNDARY_COLOR);

    // draw player paddles in initial center position
    LCD_DrawRectangle(PADDLE_INITIAL_X_MIN, PADDLE_INITIAL_X_MAX,
                      TOP_PADDLE_Y_MIN, TOP_PADDLE_Y_MAX,
                      PLAYER_RED);
    LCD_DrawRectangle(PADDLE_INITIAL_X_MIN, PADDLE_INITIAL_X_MAX,
                      BOTTOM_PADDLE_Y_MIN, BOTTOM_PADDLE_Y_MAX,
                      PLAYER_BLUE);

    return;
}

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////


/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////

/*
 * helper function to draw a ball in its current location
 */
static inline void draw_ball(Ball_t *ball)
{
    // draw ball
    LCD_DrawRectangle((ball->center.x)-BALL_SIZE_D2, (ball->center.x)+BALL_SIZE_D2,
                      (ball->center.y)-BALL_SIZE_D2, (ball->center.y)+BALL_SIZE_D2,
                      (ball->color));

    return;
}

/*
 * helper function to erase a ball from its current location
 */
static inline void erase_ball(PrevBall_t *prev_ball)
{
    // erase ball
    LCD_DrawRectangle((prev_ball->center.x)-BALL_SIZE_D2,
                      (prev_ball->center.x)+BALL_SIZE_D2,
                      (prev_ball->center.y)-BALL_SIZE_D2,
                      (prev_ball->center.y)+BALL_SIZE_D2,
                      ARENA_BACK_COLOR);

    return;
}

//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////////////THREADS//////////////////////////////////////////


/******************************* Aperiodic Threads *********************************/

void Host_button_press(){

    NVIC_DisableIRQ(PORT4_IRQn); //disable interrupts

    __delay_cycles(48000*2); //for button debouncing

     if((P4->IFG & BIT4))
     {
         button_pressed = TRUE;
     }
     else
     {
         NVIC_EnableIRQ(PORT4_IRQn);
     }

     P4->IFG &= ~BIT4; //clear flag

}

/************************************************************************************
 * Name: aperiodic_select_player
 * Purpose: Aperiodic event created at the initial start of the Pong game. This
 *          thread relies on the user to one of the options presented on the LCD
 *          screen.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_select_player(void)
{
    uint8_t status;     // used to identify which P4 pin triggered the ISR (pins 4 or 5)

    status = P4IV;      // P4IV = (2*(n+1)) where n is the pin number of the
                        // lowest bit with a pending interrupt.
                        // this access will only clear flag n.

    // if triggering switch is high after debounce time
    if ( ( P4->IN & ( 1 << (status/2) - 1 ) ) == ( 1 << (status/2) - 1 ) )
    {
        return;     // ISR improperly called
    }

    // otherwise, interrupt was properly called

    // if LCD TP triggered interrupt (port 4, pin 0)
    if (status == 2)        //  2 = 2*(0+2)
    {
        // disable pin 0 interrupt (will be rearmed if invalid LCD screen press was made)
        GPIO_disableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

        // create thread to check LCD screen press
        // potentially select a game player role within created thread
        G8RTOS_add_thread(thread_select_player, 40, "select player");
    }
}


/******************************* Aperiodic Threads *********************************/


/******************************** Common Threads ***********************************/

/************************************************************************************
 * Name: thread_start_game
 * Purpose: High priority thread to display initial game screen, and enable the
 *          aperiodic_select_player aperiodic event thread, allowing the user to
 *          select a player role by pressing the LCD screen.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_start_game(void)
{
    // add necessary threads (QUESTION: where should this actually go??)
  //  G8RTOS_add_thread(Idle, 255, "Idle");

    // display game start screen visuals

    LCD_Text(0, 0,      "  _____     _____    _     _    _____~n"
                        " (_____)   (_____)  (_)   (_)  (_____)~n"
                        " (_)__(_) (_)   (_) (__)_ (_) (_)  ___~n"
                        " (_____)  (_)   (_) (_)(_)(_) (_) (___)~n"
                        " (_)      (_)___(_) (_)  (__) (_)___(_)~n"
                        " (_)       (_____)  (_)   (_)  (_____)~n~n", LCD_BRIGHT_GREEN);
    LCD_Text(0, 100,    "               Created by:~n"
                        "      Brit Chesley and Chris Crary~n~n", LCD_LIGHT_GREEN);

    // draw separation line
    LCD_DrawRectangle(MIN_SCREEN_X, MAX_SCREEN_X, 135, 137, LCD_WHITE);

    LCD_Text(0, 145,    "        Choose your player type:~n~n~n", LCD_LIGHT_GREEN);

    // draw host and client buttons
    LCD_DrawRectangle(18, 152, 168, 222, LCD_WHITE);
    LCD_DrawRectangle(168, 302, 168, 222, LCD_WHITE);
    LCD_DrawRectangle(20, 150, 170, 220, LCD_BLUE);
    LCD_DrawRectangle(170, 300, 170, 220, LCD_RED);

    LCD_Text(0, 188, "         HOST             CLIENT", LCD_WHITE);

    // add aperiodic event to select player type
    G8RTOS_add_aperiodic_thread(aperiodic_select_player, PORT4_IRQn, 6);

    // kill current thread (game visuals will remain)
    G8RTOS_kill_current_thread();
}

/************************************************************************************
 * Name: thread_select_player
 * Purpose: High priority thread created from touch being made to LCD Touch Panel,
 *          at the initial loading of the game.
 *          Before entering this thread, the LCD's Touch Panel interrupt on
 *          port 4, pin 0 was disabled in the aperiodic thread aperiodic_select_player.
 *          This thread will choose a player type, based on the location of the
 *          user's press. If a valid selection was made,
 *          an additional thread either for the host or client will be made.
 *          If no valid choice was made (LCD press location was not valid),
 *          no additional thread is created, and this thread will sleep for 500ms, as to
 *          debounce the LCD Touch Panel. After the 500ms slumber, the Touch Panel
 *          interrupt on port 4, pin 0 will be rearmed, and the proper interrupt flag
 *          will be cleared.
 *          In either case, this thread will ultimately
 *          kill itself, as to allow the LCD touch panel screen to be pressed again,
 *          checking again if a player role was chosen.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_select_player(void)
{
    // static Point variable used to read LCD Touch Panel
    static Point temp_coordinates;

    // read x and y coordinates from LCD Touch Panel
    temp_coordinates = TP_ReadXY();

    // IMPLEMENT SEPARATE CREATEGAME THREADS FOR HOST/CLIENT

    // check if host role was chosen
    if ( (temp_coordinates.x >= SELECT_HOST_BUTTON_X_MIN) &&
         (temp_coordinates.x <= SELECT_HOST_BUTTON_X_MAX) &&
         (temp_coordinates.y >= SELECT_HOST_BUTTON_Y_MIN) &&
         (temp_coordinates.y <= SELECT_HOST_BUTTON_Y_MAX) )
    {
        // disable the select_player aperiodic thread
        G8RTOS_disable_aperiodic_thread(PORT4_IRQn);

        // create game as host
        G8RTOS_add_thread(CreateGame, 30, "Create Game");
    }
    // check if client role was chosen
    else if ( (temp_coordinates.x >= SELECT_CLIENT_BUTTON_X_MIN) &&
              (temp_coordinates.x <= SELECT_CLIENT_BUTTON_X_MAX) &&
              (temp_coordinates.y >= SELECT_CLIENT_BUTTON_Y_MIN) &&
              (temp_coordinates.y <= SELECT_CLIENT_BUTTON_Y_MAX) )
    {
        // disable the select_player aperiodic thread
        G8RTOS_disable_aperiodic_thread(PORT4_IRQn);

        LCD_Clear(LCD_BLACK);

        // join game as client
        G8RTOS_add_thread(JoinGame, 30, "Join Game");
    }
    else
    {
       // no valid LCD screen press was made
       // allow user to press LCD screen again

       // debounce LCD screen for 500ms
       G8RTOS_thread_sleep(500);

       // re-enable LCD Touch Panel interrupt (clear interrupt flag)
       GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);
       GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN0);
    }

    // kill thread
    G8RTOS_kill_current_thread();
}


/*
 * Idle thread
 */
void Idle()
{
    while(1);       // permanently active
}

/*
 * Thread to draw all the objects in the game
 */
void DrawObjects()
{
    // static local variables used to erase objects
    static PrevBall_t previous_balls[MAX_NUM_OF_BALLS];
    static PrevPlayer_t previous_players[MAX_NUM_OF_PLAYERS];
    static bool ball_erased[MAX_NUM_OF_BALLS];
    static bool first_time_drawing[MAX_NUM_OF_BALLS];

    // initialize starting position of prevPlayers
    previous_players[TOP].centerX = PADDLE_X_CENTER;
    previous_players[BOTTOM].centerX = PADDLE_X_CENTER;

    for (int i = 0; i< MAX_NUM_OF_BALLS; i++)
    {
        first_time_drawing[i] = TRUE;
        ball_erased[i] = FALSE;
    }

    while (1)
    {

        // wait for the global gamestate to be available
        G8RTOS_semaphore_wait(&semaphore_gameState);

        // draw currently alive balls
        for (uint8_t i = 0; i < MAX_NUM_OF_BALLS; i++)
        {
            // if specific ball is alive
            if (gameState.balls[i].isAlive)
            {
                // draw first instance of specific ball only, don't erase
                if(first_time_drawing[i])
                {
                    draw_ball(&gameState.balls[i]);
                    first_time_drawing[i] = FALSE;
                }
                else        // update previously alive ball on screen
                {
                    erase_ball(&previous_balls[i]);
                    draw_ball(&gameState.balls[i]);
                }

                // update previous center
                previous_balls[i].center.x = gameState.balls[i].center.x;
                previous_balls[i].center.y = gameState.balls[i].center.y;
            }
            else        // if specific ball is dead
            {
                // check if an dead ball needs to be erased
                if (ball_erased[i])
                {
                    erase_ball(&previous_balls[i]);
                    ball_erased[i] = FALSE;

                    gameState.balls[i].center.x = -1;
                    gameState.balls[i].center.y = -1;

                    previous_balls[i].center.x = gameState.balls[i].center.x;
                    previous_balls[i].center.y = gameState.balls[i].center.y;
                }
                else
                {
                    // allow ball to be erased in the future
                    ball_erased[i] = TRUE;
                }
            }
        }

        // draw players' joysticks
        for (uint8_t i = 0; i < MAX_NUM_OF_PLAYERS; i++)
        {
            UpdatePlayerOnScreen(&previous_players[i], &gameState.players[i]);
        }

        G8RTOS_semaphore_signal(&semaphore_gameState);

        // sleep that bih
        G8RTOS_thread_sleep(20);
    }
}

/*
 * Thread to update LEDs based on score
 */
void MoveLEDs()
{
    while(1)
    {
        G8RTOS_semaphore_wait(&semaphore_gameState);

        leds_update(RED, gameState.LEDScores[TOP]);
        leds_update(BLUE, gameState.LEDScores[BOTTOM]);

        G8RTOS_semaphore_signal(&semaphore_gameState);

        G8RTOS_thread_sleep(HALF_SECOND_MS);
    }

}

/******************************** Common Threads ***********************************/


/******************************** Client Threads ***********************************/

/*
 * Thread for client to join game
 */
void JoinGame()
{
    LCD_Clear(LCD_BLACK);

    LCD_Text(0, 0,      "  _____     _____    _     _    _____~n"
                        " (_____)   (_____)  (_)   (_)  (_____)~n"
                        " (_)__(_) (_)   (_) (__)_ (_) (_)  ___~n"
                        " (_____)  (_)   (_) (_)(_)(_) (_) (___)~n"
                        " (_)      (_)___(_) (_)  (__) (_)___(_)~n"
                        " (_)       (_____)  (_)   (_)  (_____)~n~n", LCD_BRIGHT_GREEN);
    LCD_Text(0, 110,    "       Waiting for game to start...", LCD_LIGHT_GREEN);

    //only thread to run after launching OS

    //set initial Specific player Info
    //initCC3100(Client); //initialize wifi as client

    gameState.client.IP_address = client1.IP_address;
    gameState.client.displacement = 0;
    gameState.client.hasAcknowledged = false; //initially set to false
    gameState.client.hasJoined = false;
    gameState.client.isReady = true;
    gameState.client.player = PLAYER2;

    G8RTOS_semaphore_wait(&semaphore_CC3100);

    //send player info to host
    SendData((uint8_t *)&header_data, HOST_IP_ADDR, sizeof(header_data));
    SendData((uint8_t *)&gameState.client, HOST_IP_ADDR, sizeof(gameState.client));

    Header_Data_t temp_data;

    // wait for server response
    while((ReceiveData((uint8_t*)&temp_data, sizeof(temp_data))) < 0);
    while(ReceiveData((uint8_t*)&gameState.client.hasAcknowledged, sizeof(gameState.client.hasAcknowledged)) < 0);

    G8RTOS_semaphore_signal(&semaphore_CC3100);

    // if joined game, light LED to show connection
    gameState.client.hasJoined = true;

    // light up led to show WiFi connection
    P2->DIR |= (BIT0); //make p2.0 an output
    P2->OUT |= (BIT0);

    InitBoardState();

    // display initial player scores
    LCD_Text(PLAYER_RED_SCORE_TEXT_MIN_X, PLAYER_RED_SCORE_TEXT_MIN_Y, "Red:", PLAYER_RED);
    LCD_Text(PLAYER_RED_SCORE_NUM_MIN_X, PLAYER_RED_SCORE_NUM_MIN_Y, "00", PLAYER_RED);
    LCD_Text(PLAYER_BLUE_SCORE_TEXT_MIN_X, PLAYER_BLUE_SCORE_TEXT_MIN_Y, "Blue:", PLAYER_BLUE);
    LCD_Text(PLAYER_BLUE_SCORE_NUM_MIN_X, PLAYER_BLUE_SCORE_NUM_MIN_Y, "00", PLAYER_BLUE);

    // initialize player scores to zero (ASCII)
    gameState.overallScores[0] = 0x30;
    gameState.overallScores[1] = 0x30;

    client_overall_scores[0] = 0x30;
    client_overall_scores[1] = 0x30;

   //add threads

    G8RTOS_add_thread(ReadJoystickClient, 45, "JoystickClient");
    G8RTOS_add_thread(SendDataToHost, 45, "dataToHost");
   // G8RTOS_add_thread(ReceiveDataFromHost, 40, "receiveData");
    G8RTOS_add_thread(DrawObjects, 30, "DrawObjects");
    G8RTOS_add_thread(MoveLEDs, 50, "MoveLEDs");
    //dont need to add idle thread, already added at menu screen

    G8RTOS_kill_current_thread(); // kill self
}

/*
 * Thread that receives game state packets from host
 */
void ReceiveDataFromHost()
{
    // temporary gamestate to avoid holding global gamestate for too long
    GameState_t tempGameState;

        /* continually receive data until a value greater than 0 is returned */

        // wait for the CC3100 to be available
        G8RTOS_semaphore_wait(&semaphore_CC3100);

        // fill local gamestate with host's gamestate
        while((ReceiveData((uint8_t*)&tempGameState, sizeof(GameState_t))) < 0)
        {
            //for reading data till a nonzero value is returned
            G8RTOS_semaphore_signal(&semaphore_CC3100);
            G8RTOS_thread_sleep(1); //sleep for 1ms to avoid deadlock
            G8RTOS_semaphore_wait(&semaphore_CC3100);
         }

        G8RTOS_semaphore_signal(&semaphore_CC3100);

        /* empty received packet */

        G8RTOS_semaphore_wait(&semaphore_gameState);
        gameState = tempGameState; //empty packet to from host to client
        G8RTOS_semaphore_signal(&semaphore_gameState);

        /* if game is done, add EndOfGameClient thread with highest priority */
        if(tempGameState.gameDone)
        {
            G8RTOS_add_thread(EndOfGameClient, 1, "EndOfGameClient");
        }

      //  G8RTOS_thread_sleep(5);
      //  G8RTOS_kill_current_thread();


}

/*
 * Thread that sends UDP packets to host
 */
void SendDataToHost()
{
    // temporary client info used to avoid holding global gamestate semaphore
    ClientInfo_t tempClientInfo;

    while (1)
    {
        G8RTOS_semaphore_wait(&semaphore_gameState);

        // initialize the local info
        tempClientInfo = gameState.client;

        G8RTOS_semaphore_signal(&semaphore_gameState);

        //send player info to host
        G8RTOS_semaphore_wait(&semaphore_CC3100);

        SendData((uint8_t*)&header_data, HOST_IP_ADDR, sizeof(header_data));
        SendData((uint8_t*)&tempClientInfo, HOST_IP_ADDR, sizeof(tempClientInfo)); //HOST_IP_ADDR is defined in CC3100 usage

        G8RTOS_semaphore_signal(&semaphore_CC3100);

        //sleep for 2ms
        G8RTOS_thread_sleep(2);
    }

}

/*
 * Thread to read client's joystick
 */
void ReadJoystickClient()
{

    // local variables to store joystick data and client's displacement
    int16_t xPos = 0;
    int16_t yPos = 0;
    int16_t displacement;

    while(1)
    {
        // update local client variables
        displacement = 0;
        GetJoystickCoordinates(&xPos, &yPos);

        /*** Man my joystick is super fucked ***/

        if (xPos > 6700)
        {
            displacement = -1;       // move left 1 unit
        }
        else if (xPos > 6660)
        {
            displacement = 0;        // do not move
        }
        else
        {
            displacement = 1;        // move right 1 unit
        }

        // update the client's gamestate
        G8RTOS_semaphore_wait(&semaphore_gameState);

        gameState.client.displacement = displacement;

        G8RTOS_semaphore_signal(&semaphore_gameState);

        // sleep to allow other wifi threads to run
        G8RTOS_thread_sleep(2);
    }

}

/*
 * End of game for the client
 */
void EndOfGameClient()
{

    //wait for all semaphores to be released
    G8RTOS_semaphore_wait(&semaphore_gameState);
    G8RTOS_semaphore_wait(&semaphore_CC3100);

    //kill all other threads
    G8RTOS_kill_all_other_threads();

    //reinitialize semaphores

    G8RTOS_semaphore_init(&semaphore_gameState, 1);
    G8RTOS_semaphore_init(&semaphore_CC3100,1);

    // clear screen with winner's colors
    if(gameState.winner == (BOTTOM))
    {
        LCD_Clear(LCD_BLUE);

        gameState.overallScores[0]++;
        leds_update(BLUE, 0xffff);

        if(gameState.overallScores[0] > 0x39)
        {
            gameState.overallScores[0] = 0x30;
            gameState.overallScores[1]++;
        }

    }

    else if(gameState.winner == (TOP))
    {
        LCD_Clear(LCD_RED);
        leds_update(RED, 0xffff);

        client_overall_scores[0]++;

        if(client_overall_scores[0] > 0x39)
        {
            client_overall_scores[0] = 0x30;
            client_overall_scores[1]++;
        }

    }

    LCD_Clear(LCD_BLACK);

    LCD_Text(0, 0,      "  _____     _____    _     _    _____~n"
                        " (_____)   (_____)  (_)   (_)  (_____)~n"
                        " (_)__(_) (_)   (_) (__)_ (_) (_)  ___~n"
                        " (_____)  (_)   (_) (_)(_)(_) (_) (___)~n"
                        " (_)      (_)___(_) (_)  (__) (_)___(_)~n"
                        " (_)       (_____)  (_)   (_)  (_____)~n~n", LCD_BRIGHT_GREEN);
    LCD_Text(0, 110,    "  Waiting for host to start new game...", LCD_LIGHT_GREEN);

    G8RTOS_semaphore_wait(&semaphore_CC3100);

    //wait for host to restart game
    while((ReceiveData((uint8_t*)&phone.header_data, sizeof(phone.header_data))) < 0);
    while(ReceiveData((uint8_t*)&gameState.client.isReady, sizeof(gameState.client.isReady)) < 0);
    G8RTOS_semaphore_signal(&semaphore_CC3100);

    InitBoardState();       // initialize board again

    //erase top player score
    LCD_DrawRectangle(PLAYER_RED_SCORE_NUM_MIN_X,
                      ARENA_LEFT_BOUNDARY_CENTER_X - 2,
                      PLAYER_RED_SCORE_NUM_MIN_Y,
                      ARENA_LEFT_BOUNDARY_CENTER_Y + 20,
                      ARENA_BACK_COLOR);


    //erase bottom player score
    LCD_DrawRectangle(PLAYER_BLUE_SCORE_NUM_MIN_X ,
                      ARENA_LEFT_BOUNDARY_CENTER_X - 2,
                      PLAYER_BLUE_SCORE_NUM_MIN_Y,
                      ARENA_LEFT_BOUNDARY_CENTER_Y + 20,
                      ARENA_BACK_COLOR);


    LCD_Text(PLAYER_RED_SCORE_TEXT_MIN_X, PLAYER_RED_SCORE_TEXT_MIN_Y, "Red:", PLAYER_RED);
    LCD_PutChar(PLAYER_RED_SCORE_NUM_MIN_X, PLAYER_RED_SCORE_NUM_MIN_Y,  client_overall_scores[1], PLAYER_RED);
    LCD_PutChar(PLAYER_RED_SCORE_NUM_MIN_X+8, PLAYER_RED_SCORE_NUM_MIN_Y, client_overall_scores[0], PLAYER_RED);

    LCD_Text(PLAYER_BLUE_SCORE_TEXT_MIN_X, PLAYER_BLUE_SCORE_TEXT_MIN_Y, "Blue:", PLAYER_BLUE);
    LCD_PutChar(PLAYER_BLUE_SCORE_NUM_MIN_X, PLAYER_BLUE_SCORE_NUM_MIN_Y, gameState.overallScores[1], PLAYER_BLUE);
    LCD_PutChar(PLAYER_BLUE_SCORE_NUM_MIN_X+8, PLAYER_BLUE_SCORE_NUM_MIN_Y, gameState.overallScores[0], PLAYER_BLUE);

    //add all threads back and restart game variables

    G8RTOS_add_thread(ReadJoystickClient, 50, "JoystickClient");
    G8RTOS_add_thread(SendDataToHost, 50, "dataToHost");
  //  G8RTOS_add_thread(ReceiveDataFromHost, 50, "receiveData");
    G8RTOS_add_thread(DrawObjects, 50, "DrawObjects");
    G8RTOS_add_thread(MoveLEDs, 50, "MoveLEDs");
    G8RTOS_add_thread(Idle, 100, "Idle");

    G8RTOS_kill_current_thread();
}

/******************************** Client Threads ***********************************/


/********************************* Host Threads ************************************/

/*
 * Thread for the host to create a game
 */
void CreateGame()
{

    LCD_Clear(LCD_BLACK);

    LCD_Text(0, 0,      "  _____     _____    _     _    _____~n"
                        " (_____)   (_____)  (_)   (_)  (_____)~n"
                        " (_)__(_) (_)   (_) (__)_ (_) (_)  ___~n"
                        " (_____)  (_)   (_) (_)(_)(_) (_) (___)~n"
                        " (_)      (_)___(_) (_)  (__) (_)___(_)~n"
                        " (_)       (_____)  (_)   (_)  (_____)~n~n", LCD_BRIGHT_GREEN);
    LCD_Text(0, 110,    "       Waiting for client to join...", LCD_LIGHT_GREEN);

    //initCC3100(Host); //initialize CC3100 as the host
    G8RTOS_semaphore_wait(&semaphore_CC3100);

    // establish connection with client
    while(ReceiveData((uint8_t*)&header_data, sizeof(header_data)) < 0);
    while(ReceiveData((uint8_t*)&gameState.client, sizeof(gameState.client)) < 0);

    G8RTOS_semaphore_signal(&semaphore_CC3100);
    // initialize the board visuals
    InitBoardState();
    // display initial player scores
    LCD_Text(PLAYER_RED_SCORE_TEXT_MIN_X, PLAYER_RED_SCORE_TEXT_MIN_Y, "Red:", PLAYER_RED);
    LCD_Text(PLAYER_RED_SCORE_NUM_MIN_X, PLAYER_RED_SCORE_NUM_MIN_Y, "00", PLAYER_RED);
    LCD_Text(PLAYER_BLUE_SCORE_TEXT_MIN_X, PLAYER_BLUE_SCORE_TEXT_MIN_Y, "Blue:", PLAYER_BLUE);
    LCD_Text(PLAYER_BLUE_SCORE_NUM_MIN_X, PLAYER_BLUE_SCORE_NUM_MIN_Y, "00", PLAYER_BLUE);

    // initialize player scores to zero
    gameState.overallScores[0] = 0x30;
    gameState.overallScores[1] = 0x30;

    client_overall_scores[0] = 0x30;
    client_overall_scores[1] = 0x30;

    G8RTOS_semaphore_wait(&semaphore_CC3100);
    // acknowledge client
    gameState.client.hasAcknowledged = true;
    SendData((uint8_t*)&header_data, gameState.client.IP_address, sizeof(header_data));
    SendData((uint8_t*)&gameState.client.hasAcknowledged, gameState.client.IP_address, sizeof(gameState.client.hasAcknowledged));
    G8RTOS_semaphore_signal(&semaphore_CC3100);

    //light up led to show connection
    P2->DIR |= (BIT0); //make p2.0 an output
    P2->OUT |= (BIT0);

    G8RTOS_add_thread(GenerateBall, 50, "Generate Ball");
    G8RTOS_add_thread(ReadJoystickHost, 50, "Joystick Host");
    G8RTOS_add_thread(DrawObjects, 30, "Draw Objects");
    G8RTOS_add_thread(MoveLEDs, 50, "MoveLEDs");
    G8RTOS_add_thread(SendDataToClient, 40, "DataToClient");
  //  G8RTOS_add_thread(ReceiveDataFromClient, 45, "receiveFromClient");

    G8RTOS_kill_current_thread();
}

/*
 * Thread that sends game state to client
 */
void SendDataToClient()
{
    GameState_t tempState;

    while (1)
    {
        //fill packet for client
        G8RTOS_semaphore_wait(&semaphore_gameState);
        tempState = gameState;
        G8RTOS_semaphore_signal(&semaphore_gameState);

        //send packet
        G8RTOS_semaphore_wait(&semaphore_CC3100);
        SendData((uint8_t*)&header_data, tempState.client.IP_address, sizeof(header_data));

        SendData( (uint8_t*)&tempState, tempState.client.IP_address, sizeof(tempState));
        G8RTOS_semaphore_signal(&semaphore_CC3100);

        //check if game is done
        if(tempState.gameDone){
            G8RTOS_add_thread(EndOfGameHost, 1, "EndOfGameHost");
        }

        //sleep for 7 ms (the GOLDEN number)
        G8RTOS_thread_sleep(7);
    }
}

/*
 * Thread that receives UDP packets from client
 */
void ReceiveDataFromClient()
{
    ClientInfo_t playerInfo; //temporary variable

   // while(1)
   // {

        //continually receive data until return value greater than zero is returned
        G8RTOS_semaphore_wait(&semaphore_CC3100);

        while(ReceiveData((uint8_t*)&playerInfo, sizeof(playerInfo)) < 0)
        {
            G8RTOS_semaphore_signal(&semaphore_CC3100);
            G8RTOS_thread_sleep(1); // sleep for 1ms to avoid a deadlock
            G8RTOS_semaphore_wait(&semaphore_CC3100);
        }

        G8RTOS_semaphore_signal(&semaphore_CC3100);

        G8RTOS_semaphore_wait(&semaphore_gameState);

        // move the client based on its received displacement
        // no need to update host's global client info
        gameState.players[TOP].currentCenterX += playerInfo.displacement;

        // check if player has moved past the arena boundaries
        if(gameState.players[TOP].currentCenterX > HORIZ_CENTER_MAX_PL)
        {
            gameState.players[TOP].currentCenterX = HORIZ_CENTER_MAX_PL;
        }
        else if(gameState.players[TOP].currentCenterX < HORIZ_CENTER_MIN_PL)
        {
            gameState.players[TOP].currentCenterX = HORIZ_CENTER_MIN_PL;
        }

        G8RTOS_semaphore_signal(&semaphore_gameState);

        //sleep for 2ms
        //G8RTOS_thread_sleep(2);
     //   G8RTOS_kill_current_thread();

   // }
}

/*
 * Generate a ball on the screen, if there is less than MAX_NUM_OF_BALLS
 * already on the screen. The rate of ball generation is proportional to
 * the amount of balls on the screen (every ball creates a 0.1 second extension
 * to the delay between ball generation).
 */
void GenerateBall()
{
    static uint8_t tempBallCount;       // used to sleep thread for appropriate amount of time

    while(1)
    {

        // wait for the global ball counter to be available
        // G8RTOS_semaphore_wait(&semaphore_ballCount);

        G8RTOS_semaphore_wait(&semaphore_gameState);

        // check if there is available memory for another ball creation
        if (gameState.ballCount < MAX_NUM_OF_BALLS)
        {
            G8RTOS_add_thread(Ball, 50, "Ball");
            gameState.ballCount++;       // update the amount of balls
        }

        tempBallCount = gameState.ballCount;      // update

        G8RTOS_semaphore_signal(&semaphore_gameState);

        // sleep a proportional amount to the current ballCount
        G8RTOS_thread_sleep(100 * tempBallCount);
    }
}

/*
 * Thread to read host's joystick
 */
void ReadJoystickHost()
{

    /* variables to hold position of joystick, only care about the x-coordinate */
    int16_t xPos;
    int16_t yPos;

    /* displacement variable to "move" the hosts joystick */
    int16_t host_displacement;


    while(1)
    {

        GetJoystickCoordinates(&xPos, &yPos);

        /******* For brits joystick, offset was about 5400, max was 8000, min was -8000 ******/
        /*** Man my joystick is super fucked ***/

        host_displacement = 0;

        G8RTOS_semaphore_wait(&semaphore_gameState);

        if (xPos > 6000)
        {
            host_displacement = -2;      // move player to right
        }
        else if (xPos > -200)
        {
            host_displacement = 0;      // don't move player at all
        }
        else
        {
            host_displacement = 2;     // move player to left
        }

        G8RTOS_semaphore_signal(&semaphore_gameState);

        G8RTOS_thread_sleep(10); // used to avoid host advantage

        G8RTOS_semaphore_wait(&semaphore_gameState);

        gameState.players[BOTTOM].currentCenterX += host_displacement; //update player position

        if(gameState.players[BOTTOM].currentCenterX > HORIZ_CENTER_MAX_PL)
        {
            gameState.players[BOTTOM].currentCenterX = HORIZ_CENTER_MAX_PL;
        }
        else if(gameState.players[BOTTOM].currentCenterX < HORIZ_CENTER_MIN_PL)
        {
            gameState.players[BOTTOM].currentCenterX = HORIZ_CENTER_MIN_PL;
        }

        G8RTOS_semaphore_signal(&semaphore_gameState);

    }


    //end of thread
}

/*
 * Thread to create and move a single ball
 */
void Ball()
{
    // static counter used to determine where to add new ball to static balls list
    static uint8_t temp_ball_counter;

    // create a new unique ball
    Ball_t *ball;

    // create variables to store ball velocity (initialized to 0)
    int16_t velocityX;
    int16_t velocityY;

    // set random velocity to ball
    if (rand() % 2 == 0)
    {
        velocityX = -3;
    }
    else
    {
        velocityX = 3;
    }
    if (rand() % 2 == 0)
    {
        velocityY = -3;
    }
    else
    {
        velocityY = 3;
    }

    G8RTOS_semaphore_wait(&semaphore_gameState);

    // determine where to place the new ball in the global static balls list
    // find the first "dead" (available) ball
    for (temp_ball_counter = 0; temp_ball_counter < MAX_NUM_OF_BALLS; temp_ball_counter++)
    {
        if (gameState.balls[temp_ball_counter].isAlive == FALSE)
        {
            ball = &gameState.balls[temp_ball_counter];       // set address of thread's unique ball
            break;      // do not continue to traverse list
        }
    }

    // activate ball
    ball->isAlive = TRUE;

    // default initial location of ball to the middle of the screen
    // IMPLEMENT RANDOM SPAWNING LOCATION
    ball->center.x = (rand() % (HORIZ_CENTER_MAX_BALL-HORIZ_CENTER_MIN_BALL-5)) + HORIZ_CENTER_MIN_BALL + BALL_SIZE+5;
    ball->center.y = (rand() % (VERT_CENTER_MAX_BALL-20)) + 20;

    // initialize ball color to white
    ball->color = INITIAL_BALL_COLOR;

    G8RTOS_semaphore_signal(&semaphore_gameState);

    // update the ball's position, until it is killed or until the game ends
    while(1)
    {
        G8RTOS_semaphore_wait(&semaphore_gameState);

        // update ball's center coordinates based on velocity
        // currently, the velocity is directly proportional to the ball's
        // x and y coordinate displacement
        ball->center.x += velocityX;
        ball->center.y += velocityY;

        // check if ball collided with top paddle
        if (   ((ball->center.y - BALL_SIZE_D2) <= TOP_PADDLE_Y_MAX) &&
            (   ( ((ball->center.x + BALL_SIZE_D2) > (gameState.players[TOP].currentCenterX - PADDLE_LEN_D2))   &&
                 ((ball->center.x - BALL_SIZE_D2) < (gameState.players[TOP].currentCenterX + PADDLE_LEN_D2)) ) ||
             ( ((ball->center.x + BALL_SIZE_D2) > (gameState.players[TOP].currentCenterX - PADDLE_LEN_D2))   &&
              ((ball->center.x - BALL_SIZE_D2) < (gameState.players[TOP].currentCenterX + PADDLE_LEN_D2)) )    ) )
        {

            //increment Y velocity
            if(abs(velocityY) < MAX_BALL_SPEED)
            {
                velocityY--;
            }

            velocityY *= -1; // reflect ball's y-velocity

            /* find where the ball hit the paddle */
            if (ball->center.x - velocityX > (gameState.players[TOP].currentCenterX + _1_3_PADDLE))
            {
                //ball hit the rightmost third
                 velocityX = abs(velocityY); //to keep things at 45 degree angle
            }
            else if (ball->center.x - velocityX < (gameState.players[TOP].currentCenterX - _1_3_PADDLE))
            {
                velocityX = (-1)*velocityY; //to keep things at 45 degree angle
            }
            else
            {
                //ball hit the middle of the paddle, dont change
                velocityX = 0; //move straight up
            }

            // increase velocity and deflect ball (reflect ball's y-velocity)

            // calculate ball's center displacement for y-coordinate past the vertical boundary
            // if ball has x-velocity other than 0, the displacement
            // for the x-coordinate will be equal to the displacement for the y-coordinate
            // add BALL_SIZE_D2 to ball's center to accurately calculate displacement!
            int16_t displacement = TOP_PADDLE_Y_MAX - (ball->center.y-BALL_SIZE_D2);

            // adjust ball's y coordinate
            ball->center.y = ball->center.y + displacement;

            //update balls color
            ball->color =gameState.players[TOP].color;

            // check if ball's x coordinate needs to change also
            if (velocityX < 0)
            {
                // ball moving at 135 degree angle
                ball->center.x = ball->center.x + displacement;
            }
            else if (velocityX > 0)
            {
                // ball moving at 45 degree angle
                ball->center.x = ball->center.x - displacement;
            }

        }

        // check if ball collided with bottom paddle
        else if (   ((ball->center.y + BALL_SIZE_D2) >= BOTTOM_PADDLE_Y_MIN) &&
                 (   ( ((ball->center.x + BALL_SIZE_D2) > (gameState.players[BOTTOM].currentCenterX - PADDLE_LEN_D2))   &&
                      ((ball->center.x - BALL_SIZE_D2) < (gameState.players[BOTTOM].currentCenterX + PADDLE_LEN_D2)) ) ||
                  ( ((ball->center.x + BALL_SIZE_D2) > (gameState.players[BOTTOM].currentCenterX - PADDLE_LEN_D2))   &&
                   ((ball->center.x - BALL_SIZE_D2) < (gameState.players[BOTTOM].currentCenterX + PADDLE_LEN_D2)) )    ) )
        {

            if(abs(velocityY) < MAX_BALL_SPEED)
            {
                velocityY++;
            }

            velocityY *= -1;

            /* find where the ball hit the paddle */
            if (ball->center.x - velocityX> (gameState.players[BOTTOM].currentCenterX + _1_3_PADDLE))
            {
                velocityX = (-1)*velocityY; //keep ball moving at 45 degree angle
            }
            else if (ball->center.x - velocityX < (gameState.players[BOTTOM].currentCenterX  - _1_3_PADDLE))
            {
                velocityX = velocityY;
            }
            else
            {
                //ball hit the middle of the paddle, don't change x velocity
                velocityX = 0; //move straight up
            }

            // calculate ball's center displacement for y-coordinate past the vertical boundary
            // if ball has x-velocity other than 0, the displacement
            // for the x-coordinate will be equal to the displacement for the y-coordinate
            // add BALL_SIZE_D2 to ball's center to accurately calculate displacement!
            int16_t displacement = (ball->center.y + BALL_SIZE_D2) - BOTTOM_PADDLE_Y_MIN;

            // adjust ball's y coordinate
            ball->center.y = ball->center.y - displacement;


            //update color of ball
            ball->color = gameState.players[BOTTOM].color;



            // check if ball's x coordinate needs to change also
            if (velocityX < 0)
            {
                // ball moving at -135 degree angle
                ball->center.x = ball->center.x + displacement;
            }
            else if (velocityX > 0)
            {
                // ball moving at -45 degree angle

                ball->center.x = ball->center.x - displacement;
            }

        }

        // check if ball collided with left arena boundary
        else if (ball->center.x <= HORIZ_CENTER_MIN_BALL)
        {
            // deflect ball (reflect ball's x-velocity)
            velocityX *= -1;

            // calculate ball's center displacement for x-coordinate past the horizontal boundary
            // if ball has y-velocity other than 0, the displacement
            // for the y-coordinate will be equal to the displacement for the x-coordinate
            int16_t displacement = HORIZ_CENTER_MIN_BALL - ball->center.x;

            // adjust ball's x coordinate
            ball->center.x = ball->center.x + displacement;

            // check if ball's y coordinate needs to change also
            if (velocityY < 0)
            {
                // ball moving at 135 degree angle
                ball->center.y = ball->center.y + displacement;
            }
            else if (velocityY > 0)
            {
                // ball moving at -135 degree angle
                ball->center.y = ball->center.y - displacement;
            }
        }

        // check if ball collided with right arena boundary
        else if (ball->center.x >= HORIZ_CENTER_MAX_BALL)
        {
            // deflect ball (reflect ball's x-velocity)
            velocityX *= -1;

            // calculate ball's center displacement for x-coordinate past the horizontal boundary
            // if ball has y-velocity other than 0, the displacement
            // for the y-coordinate will be equal to the displacement for the x-coordinate
            int16_t displacement = ball->center.x - HORIZ_CENTER_MAX_BALL;

            // adjust ball's x coordinate
            ball->center.x = ball->center.x - displacement;

            // check if ball's y coordinate needs to change also
            if (velocityY < 0)
            {
                // ball moving at 45 degree angle
                ball->center.y = ball->center.y + displacement;
            }
            else if (velocityY > 0)
            {
                // ball moving at -45 degree angle
                ball->center.y = ball->center.y - displacement;
            }
        }

        // check if ball completely passed a player's paddle
        if ( ((ball->center.y + BALL_SIZE_D2) < ARENA_MIN_Y)  ||
                 ((ball->center.y - BALL_SIZE_D2) > ARENA_MAX_Y) )
        {
            // do not re-draw ball, kill ball
            ball->isAlive = FALSE;

            gameState.ballCount--;

            if((ball->center.y + BALL_SIZE_D2) < ARENA_MIN_Y && (ball->color == PLAYER_BLUE ))
            {
                //give point to the bottom player
                if (gameState.LEDScores[BOTTOM] == 0)
                {
                    gameState.LEDScores[BOTTOM] = 0x8000;
                }
                else
                {
                    gameState.LEDScores[BOTTOM] |= (gameState.LEDScores[BOTTOM] >> 1);
                }

                //account for game ending
                if(gameState.LEDScores[BOTTOM] >= 0xffff)
                {
                    //end game, bottom player has won
                    gameState.winner = BOTTOM;
                    gameState.gameDone = true;

                }

            }
            else if((ball->center.y - BALL_SIZE_D2) > ARENA_MAX_Y && ball->color == PLAYER_RED)
            {

                //give point to the top player
                if (gameState.LEDScores[TOP] == 0)
                {
                    gameState.LEDScores[TOP] = 0x8000;
                }
                else
                {
                    gameState.LEDScores[TOP] |= (gameState.LEDScores[TOP] >> 1);
                }
                //account for game ending
                if (gameState.LEDScores[TOP] >= 0xffff)
                {

                    gameState.winner = TOP;
                    gameState.gameDone = true;
                }
            }

            G8RTOS_semaphore_signal(&semaphore_gameState);

            // kill ball thread
            G8RTOS_kill_current_thread();
        }

        G8RTOS_semaphore_signal(&semaphore_gameState);

        // sleep ball movement for 35 milliseconds
        G8RTOS_thread_sleep(35);
    }
}

/*
 * End of game for the host
 */
void EndOfGameHost()
{

    /* wait for all semaphores to be released */
    G8RTOS_semaphore_wait(&semaphore_gameState);
    G8RTOS_semaphore_wait(&semaphore_CC3100);

    /* kill all other threads */
    G8RTOS_kill_all_other_threads();


    // reinitialize CC3100 semaphores
    G8RTOS_semaphore_init(&semaphore_gameState, 1);
    G8RTOS_semaphore_init(&semaphore_CC3100,1);

    // clear screen with winner's colors
    if(gameState.winner == (BOTTOM))
    {
        LCD_Clear(LCD_BLUE);
        leds_update(BLUE, 0xffff);
        gameState.overallScores[0]++;

        if(gameState.overallScores[0] > 0x39)
        {
            gameState.overallScores[0] = 0x30;
            gameState.overallScores[1]++;
        }

    }
    else if(gameState.winner == (TOP))
    {
        LCD_Clear(LCD_RED);
        leds_update(RED, 0xffff);
        client_overall_scores[0]++;

        if(client_overall_scores[0] > 0x39)
        {
            client_overall_scores[0] = 0x30;
            client_overall_scores[1]++;
        }
    }

    LCD_Clear(LCD_BLACK);
    LCD_Text(0, 0,      "  _____     _____    _     _    _____~n"
                        " (_____)   (_____)  (_)   (_)  (_____)~n"
                        " (_)__(_) (_)   (_) (__)_ (_) (_)  ___~n"
                        " (_____)  (_)   (_) (_)(_)(_) (_) (___)~n"
                        " (_)      (_)___(_) (_)  (__) (_)___(_)~n"
                        " (_)       (_____)  (_)   (_)  (_____)~n~n", LCD_BRIGHT_GREEN);
    LCD_Text(0, 110,    "  Waiting for host to start new game...", LCD_LIGHT_GREEN);

    /* add aperiodic thread that waits for host to restart game */

    /* aperiodic event will be on button B1 (P4.4) */
    P4->IFG &= ~BIT4; //cleaer flag at first
    G8RTOS_add_aperiodic_thread(Host_button_press, PORT4_IRQn, 1);

    while(!button_pressed); //wait for button to be pressed

    button_pressed = false; //reset boolean flag

    gameState.client.isReady = true;
    SendData((uint8_t*) &gameState.client.isReady, gameState.client.IP_address, sizeof(gameState.client.isReady));

    // re-initialize board state
    InitBoardState();

    // erase previous scores

    //erase top player score
    LCD_DrawRectangle(PLAYER_RED_SCORE_NUM_MIN_X,
                      ARENA_LEFT_BOUNDARY_CENTER_X - 2,
                      PLAYER_RED_SCORE_NUM_MIN_Y,
                      ARENA_LEFT_BOUNDARY_CENTER_Y + 20,
                      ARENA_BACK_COLOR);


    //erase bottom player score
    LCD_DrawRectangle(PLAYER_BLUE_SCORE_NUM_MIN_X ,
                      ARENA_LEFT_BOUNDARY_CENTER_X - 2,
                      PLAYER_BLUE_SCORE_NUM_MIN_Y,
                      ARENA_LEFT_BOUNDARY_CENTER_Y + 20,
                      ARENA_BACK_COLOR);


    // re-write player scores
    LCD_Text(PLAYER_RED_SCORE_TEXT_MIN_X, PLAYER_RED_SCORE_TEXT_MIN_Y, "Red:", PLAYER_RED);
    LCD_PutChar(PLAYER_RED_SCORE_NUM_MIN_X, PLAYER_RED_SCORE_NUM_MIN_Y,  client_overall_scores[1], PLAYER_RED);
    LCD_PutChar(PLAYER_RED_SCORE_NUM_MIN_X+8, PLAYER_RED_SCORE_NUM_MIN_Y, client_overall_scores[0], PLAYER_RED);

    LCD_Text(PLAYER_BLUE_SCORE_TEXT_MIN_X, PLAYER_BLUE_SCORE_TEXT_MIN_Y, "Blue:", PLAYER_BLUE);
    LCD_PutChar(PLAYER_BLUE_SCORE_NUM_MIN_X, PLAYER_BLUE_SCORE_NUM_MIN_Y, gameState.overallScores[1], PLAYER_BLUE);
    LCD_PutChar(PLAYER_BLUE_SCORE_NUM_MIN_X+8, PLAYER_BLUE_SCORE_NUM_MIN_Y, gameState.overallScores[0], PLAYER_BLUE);


    // add back all necessary threads
    G8RTOS_add_thread(Idle, 255, "Idle");
    G8RTOS_add_thread(GenerateBall, 50, "Generate Ball");
    G8RTOS_add_thread(ReadJoystickHost, 50, "Joystick Host");
    G8RTOS_add_thread(DrawObjects, 50, "Draw Objects");
    G8RTOS_add_thread(MoveLEDs, 50, "MoveLEDs");
    G8RTOS_add_thread(SendDataToClient, 50, "DataToClient");
   // G8RTOS_add_thread(ReceiveDataFromClient, 50, "receiveFromClient");

    //kill self
    G8RTOS_kill_current_thread(); //RIP
}

/********************************* Host Threads ************************************/


/////////////////////////////////END OF THREADS//////////////////////////////////////

////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

////////////////////////////////END OF pong.c////////////////////////////////////
