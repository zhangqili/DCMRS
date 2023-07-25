//#define IN_1_A GPIO_PIN_0
//#define IN_1_B GPIO_PIN_1
//#define IN_2_A GPIO_PIN_2
//#define IN_2_B GPIO_PIN_3
#define IN_1 GPIO_PIN_0
#define IN_2 GPIO_PIN_1
#define IN_3 GPIO_PIN_2
#define IN_4 GPIO_PIN_3
#define IN_5 GPIO_PIN_19
#define PWM_1 GPIO_PIN_17
#define PWM_2 GPIO_PIN_18

#define ABS(a) (a>0?a:(-a))
void car_gpio_init();
int limit(int a);
void Give_Motor_PWM(int MotorL_PWM,int MotorR_PWM);
void Turn_Right_more(void);
void Turn_Left_more(void);
void Turn_Right_little(void);
void Turn_Left_little(void);
void run_straight(void);