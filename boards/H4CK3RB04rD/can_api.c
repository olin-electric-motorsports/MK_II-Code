#include "can_api.h"

/* ************** *
 * Functions      *
 * ************** */
uint8_t CAN_init( uint8_t interrupt_depth, uint8_t listen ){
    // Global interrupts
    sei();

    // Software reset; necessary for all CAN
    // stuff.
    CANGCON = _BV(SWRES);

    // CAN prescaler timing prescaler set to 0
    CANTCON = 0x00; 

    // Set BAUD rate
    /*
    CANBT1 = 0x01; // Set BAUD rate to 500kb CANBT2 = 0x04; // Re-synch handling
    CANBT3 = 0x13; // Phase edge error handling
    */
    // Old CAN settings
    /*
        CANBT1 = 0x06;
        CANBT2 = 0x0C;
        CANBT3 = 0x37;
    */

    // New CAN settings
    CANBT1 = 0x06;
    //CANBT2 = 0x0E;
    //CANBT3 = 0x7E;
    CANBT2 = 0x06;
    CANBT3 = 0xBE;

    //0x06    0x06    0xbe

    // Set up interrupts based on how much the user wants
    switch( interrupt_depth ){
        // Allow for fall-through with switch
        case 3:
            // Enable Bus off interrupt & buffer frame interrupt
            CANGIE |= _BV(ENBOFF) | _BV(ENBX);
        case 2:
            // Enable general CAN interrupts & MOb interrupts
            CANGIE |= _BV(ENERG) | _BV(ENERR);
        case 1:
            // Enable transmission interrupts
            CANGIE |= _BV(ENTX);
        case 0:
            // Allow all interrupts & receive interrupts
            CANGIE |= _BV(ENIT) | _BV(ENRX);
        default:
            break;
            // No interrupts ?
    }

    // compatibility with future chips
    CANIE1 = 0;

    // enable interrupts on all MObs
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1) |
            _BV(IEMOB2) | _BV(IEMOB3) | 
            _BV(IEMOB4) | _BV(IEMOB5) );


    // All MObs come arbitrarily set up at first,
    // must reset all in order to make them useable
    int8_t mob;
    for( mob=0; mob<6; mob++ ){
        // Selects Message Object 0-5
        // This changes the MOb that is selected
        CANPAGE = ( mob << 4 );

        // Disable mob
        CANCDMOB = 0x00;
        // Clear mob status register;
        CANSTMOB = 0x00;
    }

    // Set up as Enabled mode
    //  instead of standby
    //  Necessary in order to get CAN
    //  communication

    if( listen > 0 ){
        CANGCON |= _BV(LISTEN) | _BV( ENASTB );
    } else {
        CANGCON |= _BV( ENASTB );
    }

    return(0);
}

/* ************** *
 * MOb Type Setup *
 * ************** */
uint8_t CAN_Tx  ( uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t msg[]){
    // Check that the MOb is free
    if( bit_is_set(CANEN2, mob) ){
        return 1;
    }

    // Select CAN mob based on input MOb
    CANPAGE = (mob << MOBNB0);

    // Reset CANPAGE.INDXn
    CANPAGE &= ~(_BV(INDX0) | _BV(INDX1) | _BV(INDX2));

    // Clean CAN status for this MOb
    CANSTMOB = 0x0;

    // Set MOb ID
    //CANIDT1 = ((nodeID & 0x1F) << 3); // node ID
    CANIDT1 = ident; // node ID
    CANIDT2 = 0x00;
    CANIDT3 = 0x00;
    CANIDT4 = 0x00; // Data frame


    // Set mask to 0x00
    // Not used by Tx but good practice
    CANIDM1 = 0x00; 
    CANIDM2 = 0x00;
    CANIDM3 = 0x00;
    CANIDM4 = 0x00;

    // Set the message
    uint8_t i;
    for(i=0; i < msg_length; i++){
        CANMSG = msg[i];
    }
    
    // Send the message
    //CANCDMOB = _BV(CONMOB0) | (msg_length << DLC0);
    CANCDMOB = 0x00;
    CANCDMOB = (0x01 << CONMOB0) | (msg_length << DLC0);

    // Check for errors 
    // TODO: Set up interrupts for this shit
    //while( (CANSTMOB & _BV(TXOK)) == 0){
        //PORTB |= _BV(PB2);
        //if( CANSTMOB & _BV(BERR) != 0){
        //if( CANSTMOB & _BV(SERR) != 0){
        //if( CANSTMOB & _BV(CERR) != 0){
        //if( CANSTMOB & _BV(FERR) != 0){
        //if( CANSTMOB & _BV(AERR) != 0){
        //if( CANGSTA & _BV(TXBSY) != 0){
            //PORTC |= _BV(PC4);
    
        //}else{
            //PORTC &= ~_BV(PC4);
        //}
    //}

    // Should clear CANSTMOB once
    // Tx job is done. --This is done
    // on startup routine might not be needed
    //CANSTMOB=0x00;

    return 0;
}


uint8_t CAN_Rx(uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t mask){

    // Check that the MOb is free
    if( bit_is_set(CANEN2, mob) ){
        return 1;
    }

    // Select CAN mob based on input MOb
    CANPAGE = (mob << MOBNB0);

    // Clean CAN status for this MOb
    CANSTMOB = 0x0;

    // Set MOb ID
    //CANIDT1 = ((nodeID & 0x1F) << 3); // node ID
    CANIDT1 = ident;
    CANIDT2 = 0x00;
    CANIDT3 = 0x00;
    CANIDT4 = 0x00; // Data frame

    // Set up MASK
    CANIDM1 = mask;  // CANIDM1 & 2 are the only ones that matter
    CANIDM2 = 0x00;  // in 11 bit mode.
    CANIDM3 = 0x00;
    //CANIDM4 = 0x00; // Ignore what is set above
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // Use what is set above

    // Begin waiting for Rx
    //CANCDMOB = _BV(CONMOB1) | (msg_length << DLC0);
    CANCDMOB = 0x00;
    CANCDMOB = (0x02 << CONMOB0) | (msg_length << DLC0);

    return 0;
}
