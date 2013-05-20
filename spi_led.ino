#include <msp430g2452.h>
#define DECODE_MODE 9
#define INTENSITY   10
#define SCAN_LIMIT  11
#define SHUTDOWN    12
#define DIST_TEST   15
#define DIGIT_0     1
#define DIGIT_1     2
#define DIGIT_2     3
#define DIGIT_3     4
#define DIGIT_4     5
#define DIGIT_5     6
#define DIGIT_6     7
#define DIGIT_7     8

void spi_init() {

    USICTL0  |= USISWRST;                   // put USI in reset mode, source USI clock from SMCLK
    USICTL0  |= USIPE5 | USIPE6 | USIPE7 | USIMST | USIOE;//активен вывод тактов, выход SPI, вход SPI, режим ведущего, вывод разрешен
    USICKCTL |= USIDIV_2 | USISSEL_2;// | USICKPL;       // 16MHz/4, SMCLK
    USICTL1   = USICKPH;                    // SPI_MODE_0

    P1OUT |= BIT5 | BIT6;                   // SPI OUTPUT PINS LOW
    P1DIR = (P1DIR & ~BIT7) | BIT5 | BIT6;  // configure P1.5, P1.6, P1.7 for USI
    P2DIR |= BIT0; //вывод ~CS


    USICTL0 &= ~USISWRST;    // release USI for operation

}

void spi_senddata(int adr, int data) {
    P2OUT |= BIT0;
    USISRH = adr;
    if(adr == DIGIT_1) {
        USISRL = data | 128;
    } else {
        USISRL = data;
    }
    USICNT |= USICNT4 | USI16B;
     while (!(USICTL1 & USIIFG)) {
        ; // wait for an USICNT to decrement to 0
    }
    P2OUT &= ~BIT0;
}
void tempInit()
{
    ADC10CTL0=SREF_1 + REFON + ADC10ON + ADC10SHT_3 ; //1.5V ref,Ref on,64 clocks for sample
    ADC10CTL1=INCH_10+ ADC10DIV_3; //temp sensor is at 10 and clock/4
}
int tempOut()
{
    int t=0;
    __delay_cycles(1000);              //wait 4 ref to settle
    ADC10CTL0 |= ENC + ADC10SC;      //enable conversion and start conversion
    while(ADC10CTL1 & BUSY);         //wait..i am converting..pum..pum..
    t=ADC10MEM;                       //store val in t
    ADC10CTL0&=~ENC;                     //disable adc conv
    return(int) ((t * 27069L - 18169625L) >> 16); //convert and pass
}

int main() {

    volatile int temp;    //initialise
    int parsed = 0;
    WDTCTL = WDTPW + WDTHOLD; //stop..bow..boww
    temp=0;
    tempInit();//initialise adc
    spi_init();

    spi_senddata(DIST_TEST,0);
    spi_senddata(SHUTDOWN,1);
    spi_senddata(DECODE_MODE,0xFF);
    spi_senddata(INTENSITY,15);
    spi_senddata(SCAN_LIMIT,7);
    spi_senddata(DIGIT_0,15);
    spi_senddata(DIGIT_1,15);
    spi_senddata(DIGIT_2,15);
    spi_senddata(DIGIT_3,15);
    spi_senddata(DIGIT_4,15);
    spi_senddata(DIGIT_5,15);
    spi_senddata(DIGIT_6,15);
    spi_senddata(DIGIT_7,15);

    while(1) {
        __delay_cycles(50000);
 
        temp=tempOut();
        __delay_cycles(50000);
        __delay_cycles(50000);
        //temp = (temp*0.41-277.7)*100;

        //parsed = (int) temp;
        //parsed %= 10;
        temp = temp % 10;
        spi_senddata(DIGIT_0, temp);

    }    
}
