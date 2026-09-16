#include <pic.h>

#define _XTAL_FREQ 4000000

/* SWITCHES */
#define Start   RD0
#define Stop    RD1
#define Pick    RD2
#define Set     RD3

/* RELAYS */
#define Relay1  RD4
#define Relay2  RD5
#define Relay3  RD6

/* SENSORS */
#define Warp    RA0
#define Weft    RA1
#define Prox1   RA2
#define Prox2   RA3

/* LCD */
#define ldata PORTB
#define RS RC0
#define RW RC1
#define EN RC2

/* LEDS */
#define IRLED RC3
#define LedR  RC5
#define LedY  RC6
#define LedG  RC7

__CONFIG(WDTDIS & LVPDIS & HS & PWRTEN & BOREN);

/* LCD MESSAGES */

unsigned char MAINMSG[] = "NK POWER LOOM";
unsigned char WARPMSG[] = "WARP ERROR";
unsigned char WEFTMSG[] = "WEFT ERROR";
unsigned char STARTMSG[] = "MOTOR RUNNING";
unsigned char STOPMSG[] = "MOTOR STOP";
unsigned char PICKMSG[] = "SINGLE PICK";
unsigned char DONE[] = "PICK COMPLETED";

/* FUNCTIONS */

void Initial(void);
void lcdcmd(unsigned char);
void lcd_data(unsigned char);
void lcddata(unsigned char *);

void MotorStop(void);
void MotorForward(void);
void MotorReverse(void);

void Warp_Error(void);
void Weft_Error(void);

void main(void)
{
    ADCON1 = 0x06;

    /* PORT CONFIGURATION */

    TRISA = 0xFF;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x0F;

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    /* RELAYS OFF */

    Relay1 = 1;
    Relay2 = 1;
    Relay3 = 1;

    /* LED OFF */

    LedR = 0;
    LedY = 0;
    LedG = 0;

    IRLED = 1;

    /* LCD */

    Initial();

    lcdcmd(0x80);
    lcddata(MAINMSG);

    __delay_ms(1000);

    while(1)
    {
        /* ========================= */
        /* WARP ERROR */
        /* ========================= */

        if(Warp == 1)
        {
            Warp_Error();

            while(Warp == 1);

            __delay_ms(100);

            Initial();
            lcdcmd(0x80);
            lcddata(MAINMSG);
        }

        /* ========================= */
        /* WEFT ERROR */
        /* ========================= */

        if(Weft == 1)
        {
            Weft_Error();

            while(Weft == 1);

            __delay_ms(100);

            Initial();
            lcdcmd(0x80);
            lcddata(MAINMSG);
        }

        /* ========================= */
        /* START BUTTON */
        /* ========================= */

        if(Start == 1)
        {
            __delay_ms(50);

            if(Start == 1)
            {
                if(Warp == 1)
                {
                    Warp_Error();
                }
                else if(Weft == 1)
                {
                    Weft_Error();
                }
                else
                {
                    MotorForward();

                    Initial();
                    lcdcmd(0x80);
                    lcddata(STARTMSG);
                }

                while(Start == 1);

                __delay_ms(50);
            }
        }

        /* ========================= */
        /* SINGLE PICK */
        /* ========================= */

        if(Pick == 1)
        {
            __delay_ms(50);

            if(Pick == 1)
            {
                if(Warp == 1)
                {
                    Warp_Error();
                }
                else if(Weft == 1)
                {
                    Weft_Error();
                }
                else
                {
                    /* FORWARD */

                    MotorForward();

                    Initial();
                    lcdcmd(0x80);
                    lcddata(PICKMSG);

                    LedY = 1;
                    LedG = 0;
                    LedR = 0;

                    while(Prox1 == 0)
                    {
                        if(Warp == 1)
                        {
                            Warp_Error();
                            return;
                        }

                        if(Weft == 1)
                        {
                            Weft_Error();
                            return;
                        }

                        if(Stop == 1)
                        {
                            MotorStop();
                            break;
                        }
                    }

                    /* STOP */

                    MotorStop();

                    __delay_ms(500);

                    /* REVERSE */

                    MotorReverse();

                    LedY = 1;
                    LedG = 0;
                    LedR = 0;

                    while(Prox2 == 0)
                    {
                        if(Warp == 1)
                        {
                            Warp_Error();
                            return;
                        }

                        if(Weft == 1)
                        {
                            Weft_Error();
                            return;
                        }

                        if(Stop == 1)
                        {
                            MotorStop();
                            break;
                        }
                    }

                    /* COMPLETED */

                    MotorStop();

                    LedG = 1;
                    LedY = 0;
                    LedR = 0;

                    Initial();
                    lcdcmd(0x80);
                    lcddata(DONE);

                    __delay_ms(1000);

                    Initial();
                    lcdcmd(0x80);
                    lcddata(MAINMSG);
                }

                while(Pick == 1);

                __delay_ms(50);
            }
        }

        /* ========================= */
        /* STOP BUTTON */
        /* ========================= */

        if(Stop == 1)
        {
            __delay_ms(50);

            if(Stop == 1)
            {
                MotorStop();

                while(Stop == 1);

                __delay_ms(50);
            }
        }
    }
}


/* ================================= */
/* MOTOR FORWARD */
/* ================================= */

void MotorForward(void)
{
    /* NEVER switch both relays ON */

    Relay2 = 1;
    __delay_ms(200);

    Relay1 = 0;

    LedG = 1;
    LedY = 0;
    LedR = 0;
}


/* ================================= */
/* MOTOR REVERSE */
/* ================================= */

void MotorReverse(void)
{
    Relay1 = 1;
    __delay_ms(200);

    Relay2 = 0;

    LedG = 0;
    LedY = 1;
    LedR = 0;
}


/* ================================= */
/* MOTOR STOP */
/* ================================= */

void MotorStop(void)
{
    Relay1 = 1;
    Relay2 = 1;

    LedG = 0;
    LedY = 0;
    LedR = 1;
}


/* ================================= */
/* WARP ERROR */
/* ================================= */

void Warp_Error(void)
{
    /* MOTOR MUST STOP */

    Relay1 = 1;
    Relay2 = 1;

    LedG = 0;
    LedY = 0;
    LedR = 1;

    Initial();

    lcdcmd(0x80);
    lcddata(WARPMSG);

    /* Remain here while Warp button is pressed */

    while(Warp == 1)
    {
        Relay1 = 1;
        Relay2 = 1;

        LedR = 1;
        LedG = 0;
        LedY = 0;
    }

    LedR = 0;
}


/* ================================= */
/* WEFT ERROR */
/* ================================= */

void Weft_Error(void)
{
    /* MOTOR MUST STOP */

    Relay1 = 1;
    Relay2 = 1;

    LedG = 0;
    LedY = 0;
    LedR = 1;

    Initial();

    lcdcmd(0x80);
    lcddata(WEFTMSG);

    /* Remain here while Weft button is pressed */

    while(Weft == 1)
    {
        Relay1 = 1;
        Relay2 = 1;

        LedR = 1;
        LedG = 0;
        LedY = 0;
    }

    LedR = 0;
}


/* ================================= */
/* LCD INITIALIZATION */
/* ================================= */

void Initial(void)
{
    lcdcmd(0x38);
    lcdcmd(0x0C);
    lcdcmd(0x06);
    lcdcmd(0x01);

    __delay_ms(5);
}


/* ================================= */
/* LCD COMMAND */
/* ================================= */

void lcdcmd(unsigned char cmd)
{
    ldata = cmd;

    RS = 0;
    RW = 0;

    EN = 1;
    __delay_ms(2);
    EN = 0;
    __delay_ms(2);
}


/* ================================= */
/* LCD DATA */
/* ================================= */

void lcd_data(unsigned char data)
{
    ldata = data;

    RS = 1;
    RW = 0;

    EN = 1;
    __delay_ms(2);
    EN = 0;
    __delay_ms(2);
}


/* ================================= */
/* LCD STRING */
/* ================================= */

void lcddata(unsigned char *str)
{
    unsigned char i = 0;

    while(str[i] != '\0')
    {
        lcd_data(str[i]);
        i++;
    }
}