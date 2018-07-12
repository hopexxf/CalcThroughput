#include "stdafx.h"
#include "calc_throughput_alg.h"

#define MAX_RB_NUM_DFT 100

typedef enum McsTableType
{
    QAM64_TABLE,
    QAM256_TABLE,
    QAM64TP_TABLE,
}McsTableType;

#define TRUE  1
#define FALSE 0

#define GET_MAX(a, b) ((a) > (b) ? (a) : (b))
#define GET_MIN(a, b) ((a) > (b) ? (b) : (a))

static bool isCodeRateCtrl = true;

static unsigned char qm4Mcs[3][32] =
{
    /* PDSCH 64QAM */
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 4, 6},
    /* PDSCH 256QAM */
    {2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 2, 4, 6, 8},
    /* PUSCH TP 64QAM */
    {1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4 ,6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 2, 4, 6},
};

static double codeRate1024ForMcs[3][32] =
{
    /* PDSCH 64QAM */
    {120,   157,   193,   251,   308,   379,   449,   526,   602,   679,   340,   378,   434,   490,   553,   616,
     658,   438,   466,   517,   567,   616,   666,   719,   772,   822,   873,   910,   948,     0,     0,     0},
    /* PDSCH 256QAM */
    {120,   193,   308,   449,   602,   378,   434,   490,   553,   616,   658,   466,   517,   567,   616,   666,
     719,   772,   822,   873,   682.5, 711,   754,   797,   841,   885,   916.5, 948,     0,     0,     0,     0},
    /* PUSCH TP 64QAM */
    {240,   314,   193,   251,   308,   379,   449,   526,   602,   679,   340,   378,   434,   490,   553,   616,
     658,   466,   517,   567,   616,   666,   719,   772,   822,   873,   910,   948,     0,     0,     0,     0}
};

static long tbsizeLess3824[93] =
{
    24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,
    176,184,192,208,224,240,256,272,288,304,320,336,352,368,384,408,432,456,480,
    504,528,552,576,608,640,672,704,736,768,808,848,888,928,984,1032,1064,1128,1160,
    1192,1224,1256,1288,1320,1352,1416,1480,1544,1608,1672,1736,1800,1864,1928,2024,2088,2152,2216,
    2280,2408,2472,2536,2600,2664,2728,2792,2856,2976,3104,3240,3368,3496,3624,3752,3824
};

extern CfgInfo g_cfg;

double RacFabs(double m, double n)
{
    if(m < n) return n - m;
    return m - n;
}

long RacLog(long x)
{
    float fx;
    unsigned long ix, exp;
    fx = (float)x;
    memcpy(&ix,&fx,sizeof(unsigned long));
    exp = (ix >> 23) & 0xFF;

    return exp - 127;
}

long RacPow(unsigned char i, unsigned char n)
{
    long sum = (long)i;
    unsigned char m = 1;

    if(n == 0) return (long)1;
    for(m = 1; m < n; m++) sum *= i;
    return sum;
}

unsigned char GetQmByMcs(McsTableType tableTpye, unsigned char mcs)
{
    return qm4Mcs[(unsigned char)tableTpye][mcs];
}

double Get1024CodeRateByMcs(McsTableType tableTpye, unsigned char mcs)
{
    return codeRate1024ForMcs[(unsigned char)tableTpye][mcs];
}

long GetTbsize4Less3824(long Ninfo)
{
    long i;
    for (i = 0; i < sizeof(tbsizeLess3824)/sizeof(tbsizeLess3824[0]); ++i)
    {
        if (Ninfo <= tbsizeLess3824[i])
        {
            return tbsizeLess3824[i];
        }
    }
    return 0;
}

long long CalcTbSizeUesFormula(McsTableType tableType, long reNumPerRB, int rbNum, unsigned char mcs, unsigned char v)
{
    unsigned char Qm = 0;
    float R = 0;
    double Ninfo = 0.0;
    long n;
    long powerN;
    long C;
    long Ninfolong = 0;

    Qm = GetQmByMcs(tableType, mcs);
    R =  Get1024CodeRateByMcs(tableType, mcs) / 1024;
    Ninfo = (reNumPerRB * rbNum) * R * Qm * v;
    if(Ninfo > 3824) Ninfo = GET_MAX(Ninfo,3840);

    if (Ninfo < 3824.0 || RacFabs(Ninfo, 3824.0) < 0.0000001)
    {
        long Ninfo2log = (long)(RacLog(Ninfo));
        n = Ninfo2log > 9 ? (Ninfo2log - 6) : 3;
        powerN = RacPow(2, n);
        Ninfolong = GET_MAX(24, (powerN * (long)(Ninfo / powerN)));
        return GetTbsize4Less3824((long)Ninfolong);
    }
    else
    {
        n = (long)(RacLog(Ninfo - 24.0)) - 5;
        powerN = RacPow(2,n);
        Ninfolong = powerN * ((long)(((Ninfo - 24.0) / (double)powerN) + 0.5));

        if (R < 0.25 || RacFabs(R, 0.25) < 0.0000001)
        {
            C = (Ninfolong + 24 + 3815) / 3816;
            return 8 * C * ((Ninfolong + 24 + 8 * C - 1)/(8 * C)) - 24;
        }
        else if (Ninfolong > 8424)
        {
            C = (Ninfolong + 24 + 8423) / 8424;
            return 8 * C * ((Ninfolong + 24 + 8 * C - 1)/(8 * C)) - 24;
        }
        else
        {
            return 8 * ((Ninfolong + 24 + 7)/8)  - 24;
        }
    }
}

/* ====================================================================================== */

int GetDmrsSymNum(int dmrsType, int dmrsAddPos, int layer)
{
    if((dmrsType == 0 && layer > 4) || (dmrsType == 1 && layer > 6))
    {
        return 2+dmrsAddPos;
    }
    return 1+dmrsAddPos;
}

int CalcAllDSym()
{
    int dmrsNum = GetDmrsSymNum(g_cfg.dmrsTypeDl, g_cfg.dmrsAddPosDl, g_cfg.totalLayer);
    int symNum = SYMBOL_NUM - dmrsNum - g_cfg.pdcchSymInD;

    return GET_MAX(symNum, 0);
}

int CalcAllUSym()
{
    int dmrsNum = GetDmrsSymNum(g_cfg.dmrsTypeUl, g_cfg.dmrsAddPosUl, g_cfg.totalLayer);
    int symNum = SYMBOL_NUM - dmrsNum - g_cfg.shortPucchSymInU - g_cfg.srsSymInU;

    return GET_MAX(symNum, 0);
}

int CalcSpecDlSym()
{
    int dmrsNum = GetDmrsSymNum(g_cfg.dmrsTypeDl, g_cfg.dmrsAddPosDl, g_cfg.totalLayer);
    int symNum = g_cfg.dlSymInS - g_cfg.pdcchSymInS - dmrsNum;

    return GET_MAX(symNum, 0);
}

/* ====================================================================================== */

static void CodeRateCtrl(McsTableType tableType, int reNumPerRb, int rbNum, int deltaRe, int layer, long long *uetbSize)
{
    if(!isCodeRateCtrl) return;

    int i = 0;
    long long tbSize = *uetbSize;

    while((rbNum*reNumPerRb-deltaRe)*CODE_RADIO_CTRL_THR*layer*qm4Mcs[tableType][g_cfg.mcs-i] < tbSize*100)
    {
        i++;
        tbSize = CalcTbSizeUesFormula(tableType, reNumPerRb, rbNum, g_cfg.mcs-i, layer);
        if(g_cfg.mcs-i == 0) break;
    }
    *uetbSize = tbSize;
}

long long CalcAllTbSizeInSlot(McsTableType tableType, int reNumPerRb, int rbNum, int deltaRe)
{
    long long tbSize = 0, uetbSize;
    int i = 0;
    int m = g_cfg.totalLayer / g_cfg.ueMaxLayer;
    int remaindLayer = g_cfg.totalLayer % g_cfg.ueMaxLayer;

    if(rbNum <= 0 || reNumPerRb <= 0) return 0;

    if(m > 0)
    {
        uetbSize = CalcTbSizeUesFormula(tableType, reNumPerRb, rbNum, g_cfg.mcs, g_cfg.ueMaxLayer);
        CodeRateCtrl(tableType, reNumPerRb, rbNum, deltaRe, g_cfg.ueMaxLayer, &uetbSize);
        tbSize = m*uetbSize;
    }

    i = 0;
    if(remaindLayer > 0)
    {
        uetbSize = CalcTbSizeUesFormula(tableType, reNumPerRb, rbNum, g_cfg.mcs, remaindLayer);
        CodeRateCtrl(tableType, reNumPerRb, rbNum, deltaRe, remaindLayer, &uetbSize);
        tbSize += uetbSize;
    }

    return tbSize;
}

McsTableType GetUlTableType()
{
    if(g_cfg.is256Qam == TRUE)
        return QAM256_TABLE;
    else if(g_cfg.waveform == WAVEFORM_DFT)
        return QAM64TP_TABLE;
    else
        return QAM64_TABLE;
}

McsTableType GetDlTableType()
{
    if(g_cfg.is256Qam == TRUE)
        return QAM256_TABLE;
    else
        return QAM64_TABLE;
}

long long CalcThroughputInD()
{
    McsTableType tableType = GetDlTableType();
    int symNum = CalcAllDSym();
    int reNumPerRbNoCsi = RE_NUM_PER_SC * symNum;
    int reNumPerRbWithCsi = reNumPerRbNoCsi - RE_NUM_PER_SC * g_cfg.csiSym;
    int rbNumWithSib = GET_MIN(g_cfg.rbNum, MAX_RB_NUM-g_cfg.sibRbNum-SIB_RB_START);
    int ssbFreeRb = MAX_RB_NUM-SSBLOCK_RB_NUM-g_cfg.ssbRbStart;
    int ssbDeltaRb = (g_cfg.rbNum > ssbFreeRb) ? GET_MIN(SSBLOCK_RB_NUM, g_cfg.rbNum-ssbFreeRb) : 0;
    int ssbDeltaRe = SSBLOCK_SYM_NUM*SSBLOCK_NUM*SUB_CARRIER_NUM*ssbDeltaRb;

    long long tbSize = CalcAllTbSizeInSlot(tableType, reNumPerRbNoCsi, g_cfg.rbNum, 0);
    long long tbSizeWithCsi = CalcAllTbSizeInSlot(tableType, reNumPerRbWithCsi, g_cfg.rbNum, 0);
    long long tbSizeWithSib = CalcAllTbSizeInSlot(tableType, reNumPerRbNoCsi, rbNumWithSib, 0);
    long long tbSizeWithSsb = CalcAllTbSizeInSlot(tableType, reNumPerRbNoCsi, g_cfg.rbNum, ssbDeltaRe);
    long long schdCnt = SLOT_NUM_IN_1S*2/4;
    long long schdCntWithCsi = (g_cfg.csiPeriod>0) ? 1000/g_cfg.csiPeriod : 0;
    long long scddCntWithSib = (g_cfg.sibPeriod>0) ? 1000/g_cfg.sibPeriod : 0;
    long long scddCntWithSsb = (g_cfg.ssbPeriod>0) ? 1000/g_cfg.ssbPeriod : 0;

    tbSize = tbSize*(schdCnt-schdCntWithCsi-scddCntWithSib-scddCntWithSsb);
    tbSizeWithCsi = schdCntWithCsi*tbSizeWithCsi;
    tbSizeWithSib = scddCntWithSib*tbSizeWithSib;
    tbSizeWithSsb = scddCntWithSsb*tbSizeWithSsb;

    return tbSize+tbSizeWithCsi+tbSizeWithSib+tbSizeWithSsb;
}

long long CalcThroughputInS()
{
    McsTableType tableType = GetDlTableType();
    int symNum = CalcSpecDlSym();
    int reNumPerRb = RE_NUM_PER_SC * symNum;

    long long tbSize = CalcAllTbSizeInSlot(tableType, reNumPerRb, g_cfg.rbNum, 0);
    long long schdCnt = SLOT_NUM_IN_1S/4;

    return tbSize*schdCnt;
}

static int Get235RbNum(int rbNum, bool isFloor)
{
    int mod2, mod3, mod5;

    if(isFloor)
    {
        mod2 = rbNum % 2;
        mod3 = rbNum % 3;
        mod5 = rbNum % 5;

        if(mod2 <= mod3 && mod2 <= mod5) return rbNum-mod2;
        if(mod3 <= mod2 && mod3 <= mod5) return rbNum-mod3;
        if(mod5 <= mod3 && mod5 <= mod2) return rbNum-mod5;
    }
    else
    {
        mod2 = (2 - ((rbNum+2) % 2)) % 2;
        mod3 = (3 - ((rbNum+3) % 3)) % 3;
        mod5 = (5 - ((rbNum+5) % 5)) % 5;

        if(mod2 <= mod3 && mod2 <= mod5) return rbNum+mod2;
        if(mod3 <= mod2 && mod3 <= mod5) return rbNum+mod3;
        if(mod5 <= mod3 && mod5 <= mod2) return rbNum+mod5;
    }
}

static int GetUlRbNum(int rbNum, int maxRbNum)
{
    if(g_cfg.waveform == 1) return rbNum;

    if(rbNum > maxRbNum) return Get235RbNum(maxRbNum, true);

    int rstRbNum = Get235RbNum(rbNum, false);

    if(rstRbNum <= maxRbNum) 
        return rstRbNum;
    else
        return Get235RbNum(maxRbNum, true);
}

long long CalcThroughputInU()
{
    McsTableType tableType = GetUlTableType();
    int rbNum = GetUlRbNum(g_cfg.rbNum, MAX_RB_NUM_DFT);
    int symNum = CalcAllUSym();
    int reNumPerRb = RE_NUM_PER_SC * symNum;
    int rbNumWithPrachPucch = GetUlRbNum(rbNum, MAX_RB_NUM-g_cfg.prachRbNum-g_cfg.longPucchRbNum);

    if(g_cfg.longPucchRbNum == 0)
    {
        long long tbSize = CalcAllTbSizeInSlot(tableType, reNumPerRb, rbNum, 0);
        long long tbSizeWithPrach = CalcAllTbSizeInSlot(tableType, reNumPerRb, rbNumWithPrachPucch, 0);
        long long schdCnt = SLOT_NUM_IN_1S/4;
        long long schdCntWithPrach = (g_cfg.prachPeriod>0) ? 1000/g_cfg.prachPeriod : 0;

        tbSize = tbSize*(schdCnt-schdCntWithPrach);
        tbSizeWithPrach = schdCntWithPrach*tbSizeWithPrach;

        return tbSize+tbSizeWithPrach;
    }
    else
    {
        long long tbSize = CalcAllTbSizeInSlot(tableType, reNumPerRb, rbNumWithPrachPucch, 0);
        long long schdCnt = SLOT_NUM_IN_1S/4;

        tbSize = tbSize*schdCnt;

        return tbSize;
    }
}

long long CalcUlThroughput()
{
    isCodeRateCtrl = true;

    return CalcThroughputInU();
}

long long CalcDlThroughput()
{
    isCodeRateCtrl = true;

    return CalcThroughputInD() + CalcThroughputInS();
}

long long CalcUlTbSize()
{
    McsTableType tableType = GetUlTableType();
    int rbNum = GetUlRbNum(g_cfg.rbNum, MAX_RB_NUM_DFT);
    int symNum = CalcAllUSym();
    int reNumPerRb = RE_NUM_PER_SC * symNum;
    int rbNumWithPrachPucch = GetUlRbNum(rbNum, MAX_RB_NUM-g_cfg.prachRbNum-g_cfg.longPucchRbNum);

    if(g_cfg.longPucchRbNum == 0)
    {
        return CalcAllTbSizeInSlot(tableType, reNumPerRb, rbNum, 0);
    }
    else
    {
        return CalcAllTbSizeInSlot(tableType, reNumPerRb, rbNumWithPrachPucch, 0);
    }
}

long long CalcDlTbSize()
{
    McsTableType tableType = GetDlTableType();
    int symNum = CalcAllDSym();
    int reNumPerRbNoCsi = RE_NUM_PER_SC * symNum;

    return CalcAllTbSizeInSlot(tableType, reNumPerRbNoCsi, g_cfg.rbNum, 0);
}

long long CalcUlTbSizeWithCodeRateCtrl()
{
    isCodeRateCtrl = true;

    return CalcUlTbSize();
}

long long CalcDlTbSizeWithCodeRateCtrl()
{
    isCodeRateCtrl = true;

    return CalcDlTbSize();
}

long long CalcUlTbSizeWithoutCodeRateCtrl()
{
    isCodeRateCtrl = false;

    return CalcUlTbSize();
}

long long CalcDlTbSizeWithoutCodeRateCtrl()
{
    isCodeRateCtrl = false;

    return CalcDlTbSize();
}
