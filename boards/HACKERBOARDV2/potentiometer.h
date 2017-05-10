#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

void initADC ( void );
uint16_t sync_read_potentiometer ( void );
void async_read_potentiometer_on(void);
void async_read_potentiometer_off(void);

#endif
