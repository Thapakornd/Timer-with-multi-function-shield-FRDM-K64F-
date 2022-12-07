/*
	PORT used to LED => PTD0, PTD2, PTD3, PTD1
	PORT used to Buzzer => PTA1 (ACTIVE Low)
	PORT used to 7-Segment Digit display
	PORT used to Switch => PTB3, PTB10, PTB11
	
	- Seven Segment Information - 
	LATCH_pin => 4 : PTB23
	CLK_pin => 7 : PTC3
	DATA_pin => 8 :  PTC12
	
	*** uint8_t => Byte Type ***
	Seg_1 = 0xF1;
	Seg_2 = 0xF2;
	Seg_3 = 0xF4;
	Seg_4 = 0xF8;
	
	- FTM timer module use to 1 ms -
	20.97 MHz / 128 / 164 = 1 kHz 
*/
#include "MK64F12.h"
void delayMs(int n);
void shift(uint8_t val);
void segment_send(uint8_t val, uint8_t seg);
const uint8_t seg_val[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
const uint8_t seg_pos[4] = {0xF1,0xF2,0xF4,0xF8};
const uint8_t LED[4] = {0x1,0x4,0x8,0x2};
int c_min = 0;
int c_sec = 0;
	
int main(void){
	SIM_SCGC5 |= 0x3E00;                       /* Set clock GPIO */
	SIM_SCGC6 |= 1 << SIM_SCGC6_FTM0_SHIFT;    /* Set clock FTM */
	
	/* Buzzer and Sw1 - Sw3 */
	PORTA->PCR[1] = 0x100;      /* Buzzer */
	PORTB->PCR[3] = 0x100;      /* Sw1 (From left) */
	PORTB->PCR[10] = 0x100;     /* Sw2 */
	PORTB->PCR[11] = 0x100;     /* Sw3 */
	PTB->PDDR &= ~((1UL << 3) | (1UL << 10) | (1UL << 11));
	
	/* 7-Segment */
	PORTB->PCR[23] = 0x100;     /* Latch pin */
	PORTC->PCR[3] = 0x100;      /* Clock pin */
	PORTC->PCR[12] = 0x100;     /* Data pin */
	PTB->PDDR |= (1UL << 23);
	PTC->PDDR |= ((1UL << 3) | (1UL << 12));
	
	/* LED */
	PORTD->PCR[1] = 0x100;      /* LED 1 From Top */
	PORTD->PCR[3] = 0x100;      /* LED 2 */
	PORTD->PCR[2] = 0x100;			/* LED 3 */
	PORTD->PCR[0] = 0x100;			/* LED 4 */
	PTD->PDDR |= ((1UL << 1) | (1UL << 3) | (1UL << 2) | (1UL << 0));
	PTD->PDOR |= ((1UL << 1) | (1UL << 3) | (1UL << 2) | (1UL << 0));
	
	/* FTM0 Timer */
	FTM0->SC = 0x0;					/* Disable timer to config */
	FTM0->CNTIN = 0x00;     /* value of FTM counter (start) */
	FTM0->MOD = 0xA4;       /* Max mod-ulo value to 1 ms */
	FTM0->SC |= 0x07;				/* Set prescal = 128 */
	FTM0->SC |= 0x08;        /* Enable timer  */
	
	/* Set Interupt */
	__disable_irq();
	PORTB->PCR[3] &= ~0xF0000;  /* Interupt Sw1 */
	PORTB->PCR[10] &= ~0xF0000; /* Sw 2 */
	PORTB->PCR[11] &= ~0xF0000; /* Sw 3 */
	
	PORTB->PCR[3] |= 0xA0000;   /* Falling Edge Sw1 */
	PORTB->PCR[10] |= 0xA0000;   /* Falling Edge Sw2 */
	PORTB->PCR[11] |= 0xA0000;   /* Falling Edge Sw3 */
	
	NVIC->ISER[1] |= ((1UL << 28));
	__enable_irq();
	
	while (1) {
		segment_send(seg_val[c_min / 10],seg_pos[0]);
		segment_send(seg_val[c_min % 10] & ~(1 << 7),seg_pos[1]);
		segment_send(seg_val[c_sec / 10],seg_pos[2]);
		segment_send(seg_val[c_sec % 10],seg_pos[3]);
	}
}

/* Segment Send to display */
void segment_send(uint8_t val, uint8_t seg){
	PTB->PDOR &= ~(1UL << 23);
	shift(val);
	shift(seg);
	PTB->PDOR |= (1UL << 23);
}

/* Interupt PORTB */
void PORTB_IRQHandler(void){
	int i,j;
	/* Sw 1 Increase Number */
	if((PTB->PDIR & (1UL << 3)) == 0){
		c_sec += 20;
		if (c_sec >= 60){
			c_sec = c_sec % 60;
			c_min += 1;
		}
	}
	
	/* Sw 2 Decrease Number */
	if ((PTB->PDIR & (1UL << 10))== 0){
		c_sec -= 10;
		if ((c_sec < 0) & (c_min >= 1)){
			c_min -= 1;
			c_sec = 60 + c_sec;
		}
	}
	
	/* Sw 3 Start Timer */
	if ((PTB->PDIR & (1UL << 11))==0){
		int i,j=0;
		/* Set LED to Hour glass */
		while((c_sec >= 0) | (c_min > 0)){
		   /* 1 sec display segment */
			SysTick->LOAD = 0x1F4000- 1;
			SysTick->CTRL = 5;
			for (i=0;i<10;i++){
				while((SysTick->CTRL & 0x10000) == 0){
					segment_send(seg_val[c_min / 10], seg_pos[0]);
					segment_send(seg_val[c_min % 10] & ~(1UL << 7), seg_pos[1]);
					segment_send(seg_val[c_sec / 10], seg_pos[2]);
					segment_send(seg_val[c_sec % 10], seg_pos[3]);
				}
			}
			c_sec--;
			PTD->PDOR = ~(LED[j % 4]);
			if ((c_sec < 0) & (c_min > 0)){
				c_sec = 60 + c_sec;			
				c_min--;
			}
			j++;
		}
		while(1){
			PTA->PDDR |= (1UL << 1);    /* Buzzer alram */
			PTD->PDOR = (0xF);
			delayMs(80);
			PTA->PDDR &= ~(1UL << 1);
			PTD->PDOR = (0x0);
			delayMs(50);
		}
	}
	delayMs(300);                   /* Delay to space not double interupt */
	PORTB->ISFR |= ((1UL << 3) | (1UL << 10) | (1UL << 11));
}

/* Shiftout Function */
void shift(uint8_t val){
	int i;
	for (i=0;i<8;i++){
		if ((val & 128) != 0){
			PTC->PDOR |= (1UL << 12);
		}else{
			PTC->PDOR &= ~(1UL << 12);
		}
		val <<= 1;
		PTC->PDOR |= (1UL << 3);
		PTC->PDOR &= ~(1UL << 3);
	}
}

/* Delay Ms */
void delayMs(int n){
	int i;
	for (i=0;i<n;i++){
		while((FTM0->SC & 0x80) == 0){
			segment_send(0xBF, seg_pos[0]);
			segment_send(0xBF, seg_pos[1]);
			segment_send(0xBF, seg_pos[2]);
			segment_send(0xBF, seg_pos[3]);
		}
		FTM0->SC &= ~(0x80);
	}
}
