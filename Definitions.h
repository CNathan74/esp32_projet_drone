#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

typedef union{
  unsigned int ui_StateBtnLaby     :4;
  boolean b_StateBtnLaby[4];
}StateBtnLaby;

#define LBR_BTN_GAUCHE            18
#define LBR_BTN_DROITE            5
#define LBR_BTN_AV                21
#define LBR_BTN_AR                19
#define LBR_BTN_1                 4
#define LBR_BTN_2                 2
#define LBR_BTN_3                 15
#define LBR_BTN_1_LED             13
#define LBR_BTN_2_LED             12
#define LBR_BTN_3_LED             14
#define PMP_ANEMO                 32

#define SLD_RETRO_1               33
#define SLD_RETRO_2               25
#define SLD_RETRO_3               27


#define FREQUENCE_TIMER_PRINCIPAL 10
#define NB_PERIODE_SEQUENCEUR     10 

#endif
