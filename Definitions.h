#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

typedef union{
  unsigned int ui_StateBtnLaby     :4;
  boolean b_StateBtnLaby[4];
}StateBtnLaby;

#define LBR_BTN_GAUCHE            13
#define LBR_BTN_DROITE            14
#define LBR_BTN_AV                26
#define LBR_BTN_AR                25
#define LBR_BTN_1                 36
#define LBR_BTN_2                 35
#define LBR_BTN_3                 34
//#define LBR_BTN_1_LED             18
//#define LBR_BTN_2_LED             19
//#define LBR_BTN_3_LED             21
#define PMP_ANEMO                 0


#define FREQUENCE_TIMER_PRINCIPAL 10
#define NB_PERIODE_SEQUENCEUR     10 

#endif
