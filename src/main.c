#include "stm8s.h"
#include "stm8s_gpio.h"
#include "milis.h"
#include "stm8s103_lcd_16x2.h"
#include "stm8s_tim2.h"
//piny pro enkodér a tlačítka
#define CLK_PIN GPIOB, GPIO_PIN_0
#define DT_PIN GPIOB, GPIO_PIN_1
#define SW_PIN GPIOB, GPIO_PIN_2

#define BTN1_PIN GPIOB, GPIO_PIN_3
#define BTN2_PIN GPIOB, GPIO_PIN_4
#define BTN3_PIN GPIOB, GPIO_PIN_5
#define BTN4_PIN GPIOC, GPIO_PIN_5
#define BTN5_PIN GPIOC, GPIO_PIN_6
#define BTN6_PIN GPIOC, GPIO_PIN_4

#define BTNMEM_PIN GPIOC, GPIO_PIN_7
//
int EncoderValue1=50;
int EncoderValue2=50;
int EncoderValue3=50;
int StoredValue1 = 0;
int StoredValue2 = 0;
int StoredValue3 = 0;
int isStored = 0;

uint8_t colors[][3] = {
        {100, 0, 0},    
        {0, 100, 0},    
        {0, 0, 100},    
        {100, 100, 0},  
        {0, 100, 100}, 
        {100, 0, 100},  
    };

static uint8_t colorIndex = 0;

uint8_t RGB_Mode = 0; 
uint8_t Strobe_Mode = 0; 
uint8_t Christmas_Mode = 0;

uint8_t lastCLKState1;
uint8_t lastCLKState2;
uint8_t lastCLKState3;

int cnt;
int currentLED = 1;
int encoder_done = 0;
uint16_t c;
uint16_t d;
void GPIO_Init_Encoder(void);
void GPIO_DeInit_Encoder(void); 
void PWM_Init(void);
void PWM_DeInit(void);
void Update_PWM(void);
void Display_Status(void);
void Encoder_Read1(void);
void Encoder_Read2(void);
void Encoder_Read3(void);
void All_Bright(void);
void All_Dark(void);
void RGB_Animation(void);
void Strobe_Animation(void);
void Christmas_Colors(void);
void encoder_mode(void);
void Display_Status_Begin(void);
void Stored_In_Memory(void);

void encoder_mode(void) {
    Lcd_Clear();
		Lcd_Set_Cursor(1, 1);
		Lcd_Print_String("REZIM SVITIVOSTI");
		Lcd_Set_Cursor(2, 1);
		Lcd_Print_String("RGB LED DIODY");
		delay_ms(1500);
		Lcd_Clear();
		Lcd_Set_Cursor(1, 2);
    Lcd_Print_String("LEDR");
    Lcd_Set_Cursor(1, 7);
    Lcd_Print_String("LEDG");
    Lcd_Set_Cursor(1, 12);
    Lcd_Print_String("LEDB");
		Lcd_Set_Cursor(2, 5);
		Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 10);
		Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 15);
    Lcd_Print_String("%");
		EncoderValue1=50;
		EncoderValue2=50;
		EncoderValue3=50;
		Display_Status_Begin();
		PWM_Init();
		GPIO_Init_Encoder();
		GPIO_Init(BTN6_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTN5_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTN4_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTN3_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTN2_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTN1_PIN, GPIO_MODE_IN_PU_NO_IT);
		GPIO_Init(BTNMEM_PIN, GPIO_MODE_IN_PU_NO_IT);
    while (1) {
				if (GPIO_ReadInputPin(BTN3_PIN) == RESET) {  
						delay_ms(200);
					if (currentLED == 1) currentLED = 2;
					else if (currentLED == 2) currentLED = 3;
					else currentLED = 1;
				}
				if (currentLED == 1) {
            Encoder_Read1();
        } else if (currentLED == 2) {
            Encoder_Read2();
        } else if (currentLED == 3) {
            Encoder_Read3();
        }
        Display_Status();
        Update_PWM();
				if (GPIO_ReadInputPin(BTN2_PIN) == RESET) {
						delay_ms(200);
						All_Bright();
				}
				if (GPIO_ReadInputPin(BTN4_PIN) == RESET) {  
						delay_ms(200);
						All_Dark();
				}
				if (GPIO_ReadInputPin(BTNMEM_PIN) == RESET) {
						delay_ms(200);
						Stored_In_Memory();
				}
				if (GPIO_ReadInputPin(BTN1_PIN) == RESET) {  
						delay_ms(200); 
						RGB_Mode=1;
						if (RGB_Mode == 1) {
								RGB_Animation();
						}
						else {
            RGB_Mode = 0;
						}
				}
				if (GPIO_ReadInputPin(BTN5_PIN) == RESET) {  
						delay_ms(200); 
						Strobe_Mode=1;
						if (Strobe_Mode == 1) {
								Strobe_Animation();
						}
						else {
            Strobe_Mode = 0;
						}
				}
				if (GPIO_ReadInputPin(BTN6_PIN) == RESET) {  
						delay_ms(200); 
						Christmas_Mode=1;
						if (Christmas_Mode == 1) {
								Christmas_Colors();
						}
						else {
            Christmas_Mode = 0;
						}
				}
        if (GPIO_ReadInputPin(SW_PIN) == RESET) {
            encoder_done = 1;
            delay_ms(200);
            break;
        }
    }
}
void Display_Status_Begin(void) {
    Lcd_Set_Cursor(2, 3);
		Lcd_Print_Char(0b00110000);
    Lcd_Set_Cursor(2, 2);
		Lcd_Print_Char(0b00110101);
    Lcd_Set_Cursor(2, 8);
		Lcd_Print_Char(0b00110000);
		Lcd_Set_Cursor(2, 7);
		Lcd_Print_Char(0b00110101);
    Lcd_Set_Cursor(2, 13);
		Lcd_Print_Char(0b00110000);
		Lcd_Set_Cursor(2, 12);
		Lcd_Print_Char(0b00110101);
}
void Load_Animation(void) {
    for (cnt = 0; cnt < 17; cnt++) {
        Lcd_Set_Cursor(2, cnt);
        Lcd_Print_Char(0xFF);
        delay_ms(100);
    }
    delay_ms(100);
}
void Stored_In_Memory() {
    if (!isStored) {
        StoredValue1 = EncoderValue1;
        StoredValue2 = EncoderValue2;
        StoredValue3 = EncoderValue3;
        isStored = 1;
				Lcd_Set_Cursor(1, 1);
				Lcd_Print_String(" ");
				Lcd_Set_Cursor(1, 15);
				Lcd_Print_String(" ");
				Lcd_Set_Cursor(1, 2);
				Lcd_Print_String("UKLADAM BARVY");
				delay_ms(1500);
				Lcd_Set_Cursor(1, 2);
				Lcd_Print_String("LEDR");
				Lcd_Set_Cursor(1, 7);
				Lcd_Print_String("LEDG");
				Lcd_Set_Cursor(1, 12);
				Lcd_Print_String("LEDB");
				Lcd_Set_Cursor(2, 5);
				Lcd_Print_String("%");
				Lcd_Set_Cursor(2, 10);
				Lcd_Print_String("%");
				Lcd_Set_Cursor(2, 15);
				Lcd_Print_String("%");
    } else {
        EncoderValue1 = StoredValue1;
        EncoderValue2 = StoredValue2;
        EncoderValue3 = StoredValue3;
        isStored = 0; 
				Lcd_Set_Cursor(1, 1);
				Lcd_Print_String("AKTUALIZUJI BARVY");
				delay_ms(1500);
				Display_Status();
				Update_PWM();
				Lcd_Set_Cursor(1, 2);
				Lcd_Print_String("LEDR");
				Lcd_Set_Cursor(1, 7);
				Lcd_Print_String("LEDG");
				Lcd_Set_Cursor(1, 12);
				Lcd_Print_String("LEDB");
				Lcd_Set_Cursor(2, 5);
				Lcd_Print_String("%");
				Lcd_Set_Cursor(2, 10);
				Lcd_Print_String("%");
				Lcd_Set_Cursor(2, 15);
				Lcd_Print_String("%");
    }
}
void RGB_Animation(void){
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("MENICI RGB REZIM");
    delay_ms(1500);
    Lcd_Set_Cursor(1, 2);
    Lcd_Print_String("LEDR");
    Lcd_Set_Cursor(1, 7);
    Lcd_Print_String("LEDG");
    Lcd_Set_Cursor(1, 12);
    Lcd_Print_String("LEDB");
    Lcd_Set_Cursor(2, 5);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 10);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 15);
    Lcd_Print_String("%");
    EncoderValue1 = 0;
    EncoderValue2 = 0;
    EncoderValue3 = 0;
    Display_Status();
    Update_PWM();
		while (RGB_Mode) {
        for (c = 0; c <= 100 && RGB_Mode; c++) {
            EncoderValue1 = 100 - c;
            EncoderValue2 = c;
            EncoderValue3 = 0;
            Update_PWM();
            Display_Status();
            delay_ms(2);
            if (GPIO_ReadInputPin(BTN1_PIN) == RESET) { 
                delay_ms(200);
                RGB_Mode = 0;
                break;
            }
        }
        for (c = 0; c <= 100 && RGB_Mode; c++) { 
            EncoderValue1 = 0;
            EncoderValue2 = 100 - c;
            EncoderValue3 = c;
            Update_PWM();
            Display_Status();
            delay_ms(2);
            if (GPIO_ReadInputPin(BTN1_PIN) == RESET) { 
                delay_ms(200);
                RGB_Mode = 0;
                break;
            }
        }
        for (c = 0; c <= 100 && RGB_Mode; c++) { 
            EncoderValue1 = c;
            EncoderValue2 = 0;
            EncoderValue3 = 100 - c;
            Update_PWM();
            Display_Status();
            delay_ms(2);
            if (GPIO_ReadInputPin(BTN1_PIN) == RESET) { 
                delay_ms(200);
                RGB_Mode = 0;
                break;
            }
        }
    }
}

void Strobe_Animation(void) {
	Lcd_Set_Cursor(1, 1);
	Lcd_Print_String("BLIKAJICI REZIM");
	delay_ms(1500);
	Lcd_Set_Cursor(1, 2);
	Lcd_Print_String("LEDR");
	Lcd_Set_Cursor(1, 7);
	Lcd_Print_String("LEDG");
	Lcd_Set_Cursor(1, 12);
	Lcd_Print_String("LEDB");
	Lcd_Set_Cursor(2, 5);
	Lcd_Print_String("%");
	Lcd_Set_Cursor(2, 10);
	Lcd_Print_String("%");
	Lcd_Set_Cursor(2, 15);
	Lcd_Print_String("%");
	EncoderValue1 = 0;
	EncoderValue2 = 0;
	EncoderValue3 = 0;
	Display_Status();
	Update_PWM();
	while (Strobe_Mode) {
        EncoderValue1 = 100;
        EncoderValue2 = 100;
        EncoderValue3 = 100;
				Display_Status();
        Update_PWM();
				if (GPIO_ReadInputPin(BTN5_PIN) == RESET) {  
            delay_ms(200);  
            Strobe_Mode = 0;
            break;
				}
        delay_ms(100);
        EncoderValue1 = 0;
        EncoderValue2 = 0;
        EncoderValue3 = 0;
				Display_Status();
        Update_PWM();
        delay_ms(100);
				if (GPIO_ReadInputPin(BTN5_PIN) == RESET) {  
            delay_ms(200); 
            Strobe_Mode = 0;
            break;
				}
    }
}

void Christmas_Colors(void){
	Lcd_Set_Cursor(1, 2);
	Lcd_Print_String("VANOCNI REZIM");
	delay_ms(1500);
	Lcd_Set_Cursor(1, 2);
	Lcd_Print_String("LEDR");
	Lcd_Set_Cursor(1, 7);
	Lcd_Print_String("LEDG");
	Lcd_Set_Cursor(1, 12);
	Lcd_Print_String("LEDB");
	Lcd_Set_Cursor(2, 5);
	Lcd_Print_String("%");
	Lcd_Set_Cursor(2, 10);
	Lcd_Print_String("%");
	Lcd_Set_Cursor(2, 15);
	Lcd_Print_String("%");
	EncoderValue1 = 0;
	EncoderValue2 = 0;
	EncoderValue3 = 0;
	Display_Status();
	Update_PWM();
	while (Christmas_Mode) {
    EncoderValue1 = colors[colorIndex][0];
    EncoderValue2 = colors[colorIndex][1];
    EncoderValue3 = colors[colorIndex][2];
		colorIndex = (colorIndex + 1) % 8;
    Update_PWM();
    Display_Status();
		if (GPIO_ReadInputPin(BTN6_PIN) == RESET) {  
            delay_ms(200); 
            Christmas_Mode = 0;
            break;
				}
		}
}
void All_Bright(void){
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("VSECHNY NA 100%");
    delay_ms(1500);
    Lcd_Set_Cursor(1, 2);
    Lcd_Print_String("LEDR");
    Lcd_Set_Cursor(1, 7);
    Lcd_Print_String("LEDG");
    Lcd_Set_Cursor(1, 12);
    Lcd_Print_String("LEDB");
    Lcd_Set_Cursor(2, 5);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 10);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 15);
    Lcd_Print_String("%");
		EncoderValue1 = 100;
    EncoderValue2 = 100;
    EncoderValue3 = 100;
    Update_PWM();
    Display_Status();
}

void All_Dark(void){
    Lcd_Set_Cursor(1, 1);
		Lcd_Print_String(" ");
		Lcd_Set_Cursor(1, 15);
		Lcd_Print_String(" ");
		Lcd_Set_Cursor(1, 2);
		Lcd_Print_String("VSECHNY NA 0%");
    delay_ms(1500);
    Lcd_Set_Cursor(1, 2);
    Lcd_Print_String("LEDR");
    Lcd_Set_Cursor(1, 7);
    Lcd_Print_String("LEDG");
    Lcd_Set_Cursor(1, 12);
    Lcd_Print_String("LEDB");
    Lcd_Set_Cursor(2, 5);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 10);
    Lcd_Print_String("%");
    Lcd_Set_Cursor(2, 15);
    Lcd_Print_String("%");
		EncoderValue1 = 0;
    EncoderValue2 = 0;
    EncoderValue3 = 0;
    Update_PWM();
    Display_Status();
}
void main(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
		GPIO_Init(SW_PIN, GPIO_MODE_IN_PU_NO_IT);
    init_milis();
    Lcd_Begin();
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("NACITAM PROGRAM");
    Load_Animation();
    while (1) {
				PWM_DeInit();
        Lcd_Clear(); 
        Lcd_Set_Cursor(1, 3);
        Lcd_Print_String("RGB LAMPICKA");
        Lcd_Set_Cursor(2, 4);
        Lcd_Print_String("POMOCI PWM");
        delay_ms(1500); 
        Lcd_Clear(); 
        Lcd_Set_Cursor(1, 1); 
        Lcd_Print_String("ZMACKNI ENCODER");
        Lcd_Set_Cursor(2, 1);
        Lcd_Print_String("CEKAM NA STISK:");
        while (1) {
            if (GPIO_ReadInputPin(SW_PIN) == RESET) { 
                encoder_mode();
                Lcd_Clear();
                if (encoder_done) {
                    encoder_done = 0;
                    break;  
                }
            }
        }
    }
}

void GPIO_Init_Encoder(void) {
    GPIO_Init(CLK_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(DT_PIN, GPIO_MODE_IN_PU_NO_IT);
}
void GPIO_DeInit_Encoder(void) {
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
}

void Encoder_Read1(void) {
    static uint8_t lastState1 = 0;
    uint8_t currentState1 = 0;
    

    if (GPIO_ReadInputPin(CLK_PIN)) currentState1 |= 0x01;
    if (GPIO_ReadInputPin(DT_PIN)) currentState1 |= 0x02;

    if (lastState1 == 0x00 && currentState1 == 0x01) (EncoderValue1)++;
    else if (lastState1 == 0x01 && currentState1 == 0x03) (EncoderValue1)++;
    else if (lastState1 == 0x03 && currentState1 == 0x02) (EncoderValue1)++;
    else if (lastState1 == 0x02 && currentState1 == 0x00) (EncoderValue1)++;
    else if (lastState1 == 0x00 && currentState1 == 0x02) (EncoderValue1)--;
    else if (lastState1 == 0x02 && currentState1 == 0x03) (EncoderValue1)--;
    else if (lastState1 == 0x03 && currentState1 == 0x01) (EncoderValue1)--;
    else if (lastState1 == 0x01 && currentState1 == 0x00) (EncoderValue1)--;
    lastState1 = currentState1;
    if (EncoderValue1 > 100) EncoderValue1 = 100;
		if (EncoderValue1 < 0) EncoderValue1 = 0;
}
void Encoder_Read2(void) {
    static uint8_t lastState2 = 0;
    uint8_t currentState2 = 0;
    

    if (GPIO_ReadInputPin(CLK_PIN)) currentState2 |= 0x01;
    if (GPIO_ReadInputPin(DT_PIN)) currentState2 |= 0x02;

    if (lastState2 == 0x00 && currentState2 == 0x01) (EncoderValue2)++;
    else if (lastState2 == 0x01 && currentState2 == 0x03) (EncoderValue2)++;
    else if (lastState2 == 0x03 && currentState2 == 0x02) (EncoderValue2)++;
    else if (lastState2 == 0x02 && currentState2 == 0x00) (EncoderValue2)++;
    else if (lastState2 == 0x00 && currentState2 == 0x02) (EncoderValue2)--;
    else if (lastState2 == 0x02 && currentState2 == 0x03) (EncoderValue2)--;
    else if (lastState2 == 0x03 && currentState2 == 0x01) (EncoderValue2)--;
    else if (lastState2 == 0x01 && currentState2 == 0x00) (EncoderValue2)--;
    lastState2 = currentState2;
    if (EncoderValue2 > 100) EncoderValue2 = 100;
		if (EncoderValue2 < 0) EncoderValue2 = 0; 
}
void Encoder_Read3(void) {
    static uint8_t lastState3 = 0;
    uint8_t currentState3 = 0;
    

    if (GPIO_ReadInputPin(CLK_PIN)) currentState3 |= 0x01;
    if (GPIO_ReadInputPin(DT_PIN)) currentState3 |= 0x02;

    if (lastState3 == 0x00 && currentState3 == 0x01) (EncoderValue3)++;
    else if (lastState3 == 0x01 && currentState3 == 0x03) (EncoderValue3)++;
    else if (lastState3 == 0x03 && currentState3 == 0x02) (EncoderValue3)++;
    else if (lastState3 == 0x02 && currentState3 == 0x00) (EncoderValue3)++;
    else if (lastState3 == 0x00 && currentState3 == 0x02) (EncoderValue3)--;
    else if (lastState3 == 0x02 && currentState3 == 0x03) (EncoderValue3)--;
    else if (lastState3 == 0x03 && currentState3 == 0x01) (EncoderValue3)--;
    else if (lastState3 == 0x01 && currentState3 == 0x00) (EncoderValue3)--;
    lastState3 = currentState3;
    if (EncoderValue3 > 100) EncoderValue3 = 100;
    if (EncoderValue3 < 0) EncoderValue3 = 0;	
}
//PWM-všechny kanály
void PWM_Init(void) {
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW); 
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW); 
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 999);
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_LOW);
    TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_LOW);
    TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_LOW);
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);
    TIM2_OC3PreloadConfig(ENABLE);
    TIM2_Cmd(ENABLE);
}

void PWM_DeInit(void) {
		TIM2_DeInit();
		GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); 
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT); 
    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
}

void Update_PWM(void) {
    TIM2_SetCompare1(EncoderValue1 * 10); 
    TIM2_SetCompare2(EncoderValue2 * 10);
    TIM2_SetCompare3(EncoderValue3 * 10);  
}
//změna LEDky na displeji
void Display_Status(void) {
		Lcd_Print_Int_At1(2, 2, EncoderValue1);  
		Lcd_Print_Int_At2(2, 7, EncoderValue2); 
		Lcd_Print_Int_At3(2, 12, EncoderValue3);  
    if (currentLED == 1){
			Lcd_Set_Cursor(1, 1);
			Lcd_Print_String(">");
			Lcd_Set_Cursor(1, 6);
			Lcd_Print_String(" ");
			Lcd_Set_Cursor(1, 11);
			Lcd_Print_String(" ");
		}
    else if (currentLED == 2){
			Lcd_Set_Cursor(1, 6);
			Lcd_Print_String(">");
			Lcd_Set_Cursor(1, 1);
			Lcd_Print_String(" ");
			Lcd_Set_Cursor(1, 11);
			Lcd_Print_String(" ");
		}
    else if (currentLED == 3){
			Lcd_Set_Cursor(1, 11);
			Lcd_Print_String(">");
			Lcd_Set_Cursor(1, 6);
			Lcd_Print_String(" ");
			Lcd_Set_Cursor(1, 1);
			Lcd_Print_String(" ");
		}
		Lcd_Set_Cursor(1, 16);
		Lcd_Print_String(" ");
}
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

