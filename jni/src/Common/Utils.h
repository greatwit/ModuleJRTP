#ifndef UTILS_H_
#define UTILS_H_

#include <sys/types.h>
#include <arpa/inet.h>  //ÍøÂçÍ·ÎÄ¼þ

#include <jni.h>

// HRESULT
typedef int32_t HRESULT;
#define S_OK                             ((HRESULT)0)

const uint32_t 	STUN_COOKIE		= 0x2112A442;
//static uint32_t  STUN_COOKIE_NBO = htonl(STUN_COOKIE);

enum StunMessageClass
{
	StunMsgClassRequest=0x00,
	StunMsgClassIndication=0x01,
	StunMsgClassSuccessResponse=0x02,
	StunMsgClassFailureResponse=0x03,
	StunMsgClassInvalidMessageClass = 0xff
};

enum StunMessageType
{
	StunMsgTypeBinding = 0x0001,
	StunMsgTypeInvalid = 0xffff
};

const uint16_t STUN_TRANSACTION_ID_LENGTH = 16;
struct StunTransactionId
{
	uint8_t id[STUN_TRANSACTION_ID_LENGTH]; // first four bytes is usually the magic cookie field
};


typedef struct addrSeg
{
	char zero;
	char attr;
	short port;
	uint32_t  ip;
}addrSeg;

typedef struct bodydt
{
	short type;
	short leng;
	addrSeg addr;
}bodydef;

struct dataModel
{
	char header[4];
	char tranid[16];
	bodydef body[3]; 
};


#ifdef __cplusplus
extern "C"{
#endif

	int getCpuCoreNumber();
	int getCpuMainFreq();

	HRESULT getMessage(unsigned char *resStr, StunMessageType msgType, StunMessageClass msgClass);
	int		NetWortCallback(char outerip[], int port, char tranid[] );
	int		NetWortCallback2(JavaVM* jvm, jclass jcl, char outerip[], int port, char tranid[] );

#ifdef __cplusplus
}
#endif


#endif