#pragma once

#define WAVEFORM_DFT     0

#define SYMBOL_NUM           14
#define SUB_CARRIER_NUM      12
#define RE_NUM_PER_SC        12
#define SLOT_NUM_IN_1S       2000
#define CODE_RADIO_CTRL_THR  95
#define SIB_RB_START         28
#define MAX_RB_NUM           272
#define SSBLOCK_SYM_NUM      4
#define SSBLOCK_RB_NUM       20
#define SSBLOCK_NUM          1

//rb分配方式为从高频向低频
typedef struct CfgInfo
{
//basic
    int rbNum;
    int mcs;
    int ueMaxLayer;
    int totalLayer;
    int is256Qam;
    int waveform;
//d
    int csiPeriod; //10//只出现在d slot
    int csiSym; //1
    int sibPeriod; //80//只出现在8号slot
    int sibRbNum; //24//28-52
    int ssbPeriod; //20//只出现在0号slot
    int ssbRbStart; //30
//d
    int dmrsTypeDl; //type2
    int dmrsDurationDl; //0//dl=1
    int dmrsAddPosDl; //0//dl=pos0
    int pdcchSymInD;//1
//u
    int dmrsTypeUl; //type2
    int dmrsDurationUl; //1//ul=2
    int dmrsAddPosUl; //0//ul=pos0
    int srsSymInU; //1
    int shortPucchSymInU;//0
    int longPucchRbNum;//90
    int prachPeriod;//20
    int prachRbNum;//12
//s
    int shortPucchSymInS;//0
    int srsSymInS; //1
    int pdcchSymInS;//1
    int ulSymInS;//0
    int dlSymInS;//11//pdcchSym算在里面
}CfgInfo;

long long CalcUlThroughput();
long long CalcDlThroughput();
