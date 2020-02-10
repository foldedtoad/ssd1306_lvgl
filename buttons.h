/*
 *   buttons.h
 */
#ifndef __BUTTONS_H
#define __BUTTONS_H


enum {
    SW1_ID  = 0,
    SW2_ID  = 1,
    SW3_ID  = 2,
    SW4_ID  = 3,
};

#define NO_PRESS   (-1)

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void buttons_init(void);
int  buttons_get_state(void);

#endif  /* __BUTTONS_H */