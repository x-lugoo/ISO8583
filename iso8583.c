/*Auth:Jeff
 *function:unpack iso8583
 *data:2017/09/23
*/


#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>


#define Attr_a			0
#define Attr_b			1
#define Attr_n			2
#define Attr_z			3
#define Attr_Over		4

#define Attr_var1		0
#define Attr_var2		1
#define Attr_fix		2
#define FILE_NAME		"pkgLog"
	
typedef struct _tagFIELD_ATTR
{
   unsigned char 	ucIfExist;
   unsigned char	eElementAttr;
   unsigned char	eLengthAttr;
   unsigned int		uiLength;
}FIELD_ATTR;

// ISO8583 field attribute
static FIELD_ATTR DefaulFieldDef[] =
{
	{0,Attr_b, Attr_fix,  8},				// 1 - bitmap
	{0,Attr_n, Attr_var1, 19},				// 2 - PAN
	{0,Attr_n, Attr_fix,  6},				// 3 - process code
	{0,Attr_n, Attr_fix,  12},				// 4 - txn amount
	{0,Attr_n, Attr_fix,  12},				// 5 - Foreign Amt(DCC)
	{0,Attr_n, Attr_fix, 0},				// 6 - 
	{0,Attr_n, Attr_fix, 0},				// 7 - 
	{0,Attr_n, Attr_fix, 0},				// 8 -
	{0,Attr_a, Attr_fix, 8},				// 9 - DCC rate
	{0,Attr_n, Attr_fix, 0},				// 10 - 
	{0,Attr_n, Attr_fix, 6},				// 11 - STAN
	{0,Attr_n, Attr_fix, 6},				// 12 - local time
	{0,Attr_n, Attr_fix, 4},				// 13 - local date
	{0,Attr_n, Attr_fix, 4},				// 14 - expire
	{0,Attr_n, Attr_fix, 4},				// 15 
	{0,Attr_n, Attr_fix, 0},				// 16 -
	{0,Attr_n, Attr_fix, 0},				// 17 - 
	{0,Attr_n, Attr_fix, 0},				// 18 -
	{0,Attr_n, Attr_fix, 0},				// 19 - 
	{0,Attr_n, Attr_fix, 0},				// 20 - 
	{0,Attr_n, Attr_fix, 0},				// 21 - 
	{0,Attr_n, Attr_fix, 4},				// 22 - entry modes
	{0,Attr_n, Attr_fix, 3},				// 23 - PAN Seq
	{0,Attr_n, Attr_fix, 3},				// 24 - NII
	{0,Attr_n, Attr_fix, 2},				// 25 - condition code
	{0,Attr_n, Attr_fix, 0},				// 26 - 
	{0,Attr_n, Attr_fix, 0},				// 27 - 
	{0,Attr_n, Attr_fix, 0},				// 28 - 
	{0,Attr_n, Attr_fix, 0},				// 29 - 
	{0,Attr_n, Attr_fix, 0},				// 30 - 
	{0,Attr_n, Attr_fix, 0},				// 31 - 
	{0,Attr_n, Attr_var1, 11},				// 32 -
	{0,Attr_n, Attr_fix, 0},				// 33 - 
	{0,Attr_a, Attr_fix, 0},				// 34 - 
	{0,Attr_n, Attr_var1, 37},				// 35 - track 2
	{0,Attr_n, Attr_var2, 104},				// 36 - track 3
	{0,Attr_a, Attr_fix,  12},				// 37 - RRN
	{0,Attr_a, Attr_fix,  6},				// 38 - auth. code
	{0,Attr_a, Attr_fix,  2},				// 39 - response code
	{0,Attr_a, Attr_fix, 0},				// 40 -
	{0,Attr_a, Attr_fix, 8},				// 41 - TID
	{0,Attr_a, Attr_fix, 15},				// 42 - MID
	{0,Attr_n, Attr_fix, 0},				// 43 -
	{0,Attr_a, Attr_var1, 25},				// 44 - Add'l rsp data
	{0,Attr_n, Attr_var1, 76+1},			// 45 - track 1
	{0,Attr_n, Attr_fix, 0},				// 46 - 
	{0,Attr_n, Attr_fix, 0},				// 47 - 
	{0,Attr_b, Attr_var2, 100},				// 48 - add'l req data
	{0,Attr_a, Attr_fix,  3},				// 49 - FRN Curcy code
	{0,Attr_n, Attr_fix,  3},				// 50 - Currency code
	{0,Attr_n, Attr_fix, 0},				// 51 - 
	{0,Attr_b, Attr_fix, 8},				// 52 - PIN data
	{0,Attr_n, Attr_fix, 16},				// 53 - not used
	{0,Attr_a, Attr_var2, 12},				// 54 - Extra Amount
	{0,Attr_b, Attr_var2, 260},				// 55 - ICC data
	{0,Attr_b, Attr_var2, 110},				// 56 - ICC data 2
	{0,Attr_n, Attr_fix, 0},				// 57 - 
	{0,Attr_a, Attr_fix, 0},				// 58 - 
	{0,Attr_n, Attr_fix, 0},				// 59 -
	{0,Attr_n, Attr_var2, 999},				// 60 - Private used
	{0,Attr_n, Attr_var2, 999},				// 61 - POS SN/desc code
	{0,Attr_a, Attr_var2, 999},				// 62 - ROC/SOC
	{0,Attr_a, Attr_var2, 999},				// 63 - Private used
	{0,Attr_a, Attr_fix,  8},				// 64 - MAC
	{0,Attr_Over,   Attr_fix, 0}
};



//"12EF"-->0x12EF
void Asc2Bcd(const unsigned char *psIn, unsigned int uiLength, unsigned char *psOut)
{
    unsigned char   tmp;
    unsigned int    i;
	
  	if ((psIn == NULL) || (psOut == NULL))
	{
		return;
	}
	
	for(i = 0; i < uiLength; i += 2)
    {
        tmp = psIn[i];
        if( tmp > '9' )
        {
            tmp = (unsigned char)toupper((int)tmp) - 'A' + 0x0A;
        }
        else
        {
            tmp &= 0x0F;
        }
        psOut[i / 2] = (tmp << 4);
		
        tmp = psIn[i+1];
        if( tmp > '9' )
        {
            tmp = toupper((char)tmp) - 'A' + 0x0A;
        }else
        {
            tmp &= 0x0F;
        }
        psOut[i/2] |= tmp;
    }
}

//0x12EF-->"12EF"
void Bcd2Asc(const unsigned char *psIn, unsigned int uiLength, unsigned char *psOut)
{
    static const unsigned char ucHexToChar[16] = {"0123456789ABCDEF"};
    unsigned int   uiCnt;
	
	if ((psIn == NULL) || (psOut == NULL))
	{
		return;
	}

    for(uiCnt = 0; uiCnt < uiLength; uiCnt++)
    {
        psOut[2*uiCnt]     = ucHexToChar[(psIn[uiCnt] >> 4)];
        psOut[2*uiCnt + 1] = ucHexToChar[(psIn[uiCnt] & 0x0F)];
    }
}

void GetFieldLen(FIELD_ATTR *eElementAttr, int *iFieldLen)
{
	if(eElementAttr->eElementAttr == Attr_n)
	{
		*iFieldLen = (*iFieldLen % 2) ? ((*iFieldLen/2 + 1)):(*iFieldLen / 2);
	}
	else if(eElementAttr->eElementAttr == Attr_a || eElementAttr->eElementAttr == Attr_b )
	{
		*iFieldLen = *iFieldLen;
	}
}

int UnpackIso8583(void)
{
	int i;
	int j;
	int iCount = 0;
	int result;
	int iCurrentLen = 0;
	int iFieldLen = 0;
	unsigned char buf[9];
	unsigned char bug8583Pkg[2048];
	unsigned char bufTmp2[2048];
	int iBufLen = 0;
	int iFd;
	int iFileLen = 0;

	iFd = open(FILE_NAME,O_RDWR ,0644);
	if(iFd < 0){
		printf("open file%s error\n",FILE_NAME);
		return -1;
	}
	memset(bug8583Pkg,0,sizeof(bug8583Pkg));
	iFileLen = read(iFd,bug8583Pkg,1024);
	if(iFileLen < 0){
		printf("read file %s error\n",FILE_NAME);
		return -1;
	}
	printf("fileLen=%d\n",iFileLen);
	close(iFd);
	iCurrentLen += 2;
	for(i = 0;i < strlen(bug8583Pkg);i++)
	{
		if(bug8583Pkg[i] != ' ' && bug8583Pkg[i] != '\t'
		  && bug8583Pkg[i] != '\n' && bug8583Pkg[i] != '\r')
		{
			bufTmp2[iCount++] = bug8583Pkg[i];
		}
	}
	Asc2Bcd(bufTmp2,iCount,bug8583Pkg);;
	for(i = 0;i < iCount/2;i++)
	{
		sprintf(bufTmp2 + i*3,"%02X ",bug8583Pkg[i]);
	}
	printf("ISO8583:=%s\n",bufTmp2);
	printf("\nTotalLen=0x%02X(%03d)\n",iCount/2,iCount/2);

	/*TPDU*/
	sprintf(bufTmp2,"[TPDU]=");
	iBufLen = strlen(bufTmp2);
	for(i = 0;i < 5;i++)
	{
		sprintf(bufTmp2 + iBufLen + i * 3,"%02X ",(bug8583Pkg + iCurrentLen)[i]);
	}
	printf("%s\n",bufTmp2);
	iCurrentLen += 5;
	iCurrentLen += 6;

	 /*MSG CODE*/
	sprintf(bufTmp2,"[Msg Code]=");
	iBufLen = strlen(bufTmp2);
	for(i = 0;i < 2;i++)
	{
		sprintf(bufTmp2 + iBufLen + i * 3,"%02X ",(bug8583Pkg + iCurrentLen)[i]);
	}
	printf("%s\n",bufTmp2);
	iCurrentLen += 2;
	/*Bit Map */
	sprintf(bufTmp2,"[bitMap]=");
	iBufLen = strlen(bufTmp2);
	for(i = 0;i < 8;i++)
	{
		sprintf(bufTmp2 + iBufLen + i * 3,"%02X ",(bug8583Pkg + iCurrentLen)[i]);
	}
	printf("%s\n",bufTmp2);
	memcpy(buf,bug8583Pkg + iCurrentLen,8);
	iCurrentLen += 8;
	/*Field 2 -- Field 64*/
	for(i = 1;i < 64;i++)
	{
	   if(buf[i/8] & (0x80>>i%8))
	   {
	   	
	   	  DefaulFieldDef[i].ucIfExist = 1;
		 if(DefaulFieldDef[i].eLengthAttr == Attr_fix)
		 {
		 	iFieldLen = DefaulFieldDef[i].uiLength;
			sprintf(bufTmp2,"[bit%02d] len=%03d: ",i+1,iFieldLen);
			iBufLen = strlen(bufTmp2);
			GetFieldLen(&DefaulFieldDef[i],&iFieldLen);
			for(j = 0;j < iFieldLen;j++)
			{
				sprintf(bufTmp2 + iBufLen + j * 3,"%02X ",(bug8583Pkg + iCurrentLen)[j]);
			}
		 	iCurrentLen += iFieldLen;
		 }
		 else if(DefaulFieldDef[i].eLengthAttr == Attr_var1)
		 {
		 	iFieldLen = ((bug8583Pkg + iCurrentLen)[0] & 0x0F) 
							+ ((bug8583Pkg + iCurrentLen)[0] >> 4) * 10;
			iCurrentLen += 1;
			sprintf(bufTmp2,"[bit%02d] len=%03d: ",i+1,iFieldLen);
			iBufLen = strlen(bufTmp2);
			GetFieldLen(&DefaulFieldDef[i],&iFieldLen);
			for(j = 0;j < iFieldLen;j++)
			{
				sprintf(bufTmp2 + iBufLen + j * 3,"%02X ",(bug8583Pkg + iCurrentLen)[j]);
			}
			iCurrentLen += iFieldLen;
		 }
		else if(DefaulFieldDef[i].eLengthAttr == Attr_var2)
		 {
		 	iFieldLen = ((bug8583Pkg + iCurrentLen)[0] & 0x0F) * 100
						+ ((bug8583Pkg + iCurrentLen)[1] & 0x0F)
						+ ((bug8583Pkg + iCurrentLen)[1] >> 4) * 10;
			iCurrentLen += 2;
			
			sprintf(bufTmp2,"[bit%02d] len=%03d: ",i+1,iFieldLen);
			iBufLen = strlen(bufTmp2);
			GetFieldLen(&DefaulFieldDef[i],&iFieldLen);	
			for(j = 0;j < iFieldLen;j++)
			{
				sprintf(bufTmp2 + iBufLen + j * 3,"%02X ",(bug8583Pkg + iCurrentLen)[j]);
			}
			iCurrentLen += iFieldLen;
		 }
		 printf("%s\n",bufTmp2);
	   }
	}
	printf("pack ok\n");
	return 0;
}

int main(void)
{
	UnpackIso8583();
	return 0;
}

