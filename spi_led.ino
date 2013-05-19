#include <msp430g2452.h>

void spi_init() {

    USICTL0  |= USISWRST;                   // put USI in reset mode, source USI clock from SMCLK
    USICTL0  |= USIPE5 | USIPE6 | USIPE7 | USIMST | USIOE;//активен вывод тактов, выход SPI, вход SPI, режим ведущего, вывод разрешен
    USICKCTL |= USIDIV_2 | USISSEL_2;// | USICKPL;       // 16MHz/4, SMCLK
    USICTL1   = USICKPH;                    // SPI_MODE_0

    P1OUT |= BIT5 | BIT6;                   // SPI OUTPUT PINS LOW
    P1DIR = (P1DIR & ~BIT7) | BIT5 | BIT6;  // configure P1.5, P1.6, P1.7 for USI
    P2DIR |= BIT0; //вывод ~CS
    P2OUT &= ~BIT0;

    USICTL0 &= ~USISWRST;    // release USI for operation

}

int main() {

    spi_init();
    USICTL0 &= ~USISWRST;    // release USI for operation
    int a = 15;
    
     USISRH = a;
    USISRL = 1; 
    P2OUT |= BIT0;

    USICNT |= USICNT4 | USI16B;
    while(!(USIIFG));
    P2OUT &= ~BIT0;

    
}

