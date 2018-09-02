
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "Utils.h"

#include "ComDefine.h"
#define TAG "Utils"

//extern JavaVM*	g_javaVM;
//extern jclass   g_mClass;

int getCpuCoreNumber()
{
	char *cmd_get_num_cores = "cat /proc/cpuinfo | grep 'core id' | uniq |wc -l"; 
	FILE *pd = NULL;    
	int num_cores;
	assert( (pd = popen( cmd_get_num_cores, "r")) != NULL );
	int nc = fscanf(pd, "%d", &num_cores);
	assert( nc == 1 && num_cores > 0 );
	pclose(pd);
	return nc;
}

int getCpuMainFreq()
{
	char *cmd_get_main_freq = "dmesg | grep -e 'Detected' | grep -e 'processor' | sed -e 's/.*\\s\\+\\([.0-9]\\+\\)\\s\\+MHz.*/\\1/'";
	FILE *pd = NULL;   
	int num_cores;
	assert( (pd = popen( cmd_get_main_freq, "r")) != NULL );
	int nc = fscanf(pd, "%d", &num_cores);
	assert( nc == 1 && num_cores > 0 );
	pclose(pd);
	return num_cores;
}

HRESULT getMessage(unsigned char *resStr, StunMessageType msgType, StunMessageClass msgClass)
{
	uint16_t msgTypeField=0;
	HRESULT hr = S_OK;

	// merge the _msgType and _msgClass, and the leading zero bits into a 16-bit field
	msgTypeField =  (msgType & 0x0f80)	<< 2;
	msgTypeField |= (msgType & 0x0070)	<< 1;
	msgTypeField |= (msgType & 0x000f);
	msgTypeField |= (msgClass & 0x02)	<< 7;
	msgTypeField |= (msgClass & 0x01)	<< 4;

	msgTypeField = htons(msgTypeField);
	memcpy(resStr, &msgTypeField, 2);

	StunTransactionId transid;
	uint32_t stun_cookie_nbo = htonl(STUN_COOKIE);


	// the first four bytes of the transaction id is always the magic cookie
	// followed by 12 bytes of the real transaction id
	memcpy(transid.id, &stun_cookie_nbo, sizeof(stun_cookie_nbo));
	for (int x = 4; x < (STUN_TRANSACTION_ID_LENGTH-4); x++)
	{
		transid.id[x] = (uint8_t)(rand() % 256);
	}

	memcpy(resStr + 4, transid.id, 16);

	return hr;
}
/*
int NetWortCallback(char outerip[], int port, char tranid[] )
{
	jint result = -1;
	JNIEnv*		menv;
	jobject		mobj;
	jclass		tmpClass;


	if(NULL == g_mClass)
	{
		GLOGE("g_mClass is null.");
		return result;
	}

	if(g_javaVM)
	{
		result = g_javaVM->AttachCurrentThread( &menv, NULL);  
		if(NULL == menv)
		{
			GLOGE("function: %s, line: %d, GetEnv failed!", __FUNCTION__, __LINE__);
			return g_javaVM->DetachCurrentThread();
		}
	}
	else
	{
		GLOGE("function: %s, line: %d, JavaVM is null!", __FUNCTION__, __LINE__);
		return result;
	}

	jbyteArray tranidArray = menv->NewByteArray(STUN_TRANSACTION_ID_LENGTH);  
	jbyte *bytes = menv->GetByteArrayElements( tranidArray, 0);  
	for ( int i = 0; i < STUN_TRANSACTION_ID_LENGTH; i++ )
	{
		bytes[i] = tranid[i]; //tranid
	}

	menv->SetByteArrayRegion(tranidArray, 0, STUN_TRANSACTION_ID_LENGTH, bytes ); 

	if(NULL != g_mClass)
	{
		tmpClass = menv->GetObjectClass(g_mClass);
		if(tmpClass == NULL)
		{
			GLOGE("function: %s, line: %d, find class error", __FUNCTION__, __LINE__);
			return g_javaVM->DetachCurrentThread();
		}
		mobj = menv->AllocObject(tmpClass);
		if(NULL == mobj)
		{
			GLOGE("function: %s, line: %d, find jobj error!", __FUNCTION__, __LINE__);
			return g_javaVM->DetachCurrentThread();
		}
		jmethodID methodID_func = menv->GetMethodID(tmpClass, "NatInfo", "(Ljava/lang/String;I[B)V");
		if(methodID_func == NULL)
		{
			GLOGE("function: %s, line: %d,find method error!", __FUNCTION__, __LINE__);
			return g_javaVM->DetachCurrentThread();
		}
		else
		{
			jstring strOuterIp = menv->NewStringUTF(outerip);
			menv->CallVoidMethod(mobj, methodID_func, strOuterIp, port, tranidArray);
		}
	}

	menv->ReleaseByteArrayElements(tranidArray, bytes, 0);
	menv->DeleteLocalRef(tranidArray);

	return g_javaVM->DetachCurrentThread();
}
*/
int		NetWortCallback2(JavaVM* jvm, jclass jcl, char outerip[], int port, char tranid[] )
{
	jint result = -1;
	JNIEnv*		menv;
	jobject		mobj;
	jclass		tmpClass;


	if(NULL == jcl)
	{
		GLOGE("function: %s, line: %d,jClass is null!", __FUNCTION__, __LINE__);
		return result;
	}

	if(jvm)
	{
		result = jvm->AttachCurrentThread( &menv, NULL);  
		if(NULL == menv)
		{
			GLOGE("function: %s, line: %d,GetEnv failed!", __FUNCTION__, __LINE__);
			return jvm->DetachCurrentThread();
		}
	}
	else
	{
		GLOGE("function: %s, line: %d,JavaVM is null!", __FUNCTION__, __LINE__);
		return result;
	}

	jbyteArray tranidArray = menv->NewByteArray(STUN_TRANSACTION_ID_LENGTH);  
	jbyte *bytes = menv->GetByteArrayElements( tranidArray, 0);  
	for ( int i = 0; i < STUN_TRANSACTION_ID_LENGTH; i++ )
	{
		bytes[i] = tranid[i]; //tranid
	}

	menv->SetByteArrayRegion(tranidArray, 0, STUN_TRANSACTION_ID_LENGTH, bytes ); 


	tmpClass = menv->GetObjectClass(jcl);
	if(tmpClass == NULL)
	{
		GLOGE("function: %s, line: %d,find class error!", __FUNCTION__, __LINE__);
		return jvm->DetachCurrentThread();
	}
	mobj = menv->AllocObject(tmpClass);
	if(NULL == mobj)
	{
		GLOGE("function: %s, line: %d,find jobj error!", __FUNCTION__, __LINE__);
		return jvm->DetachCurrentThread();
	}
	jmethodID methodID_func = menv->GetMethodID(tmpClass, "NatInfo", "(Ljava/lang/String;I[B)V");
	if(methodID_func == NULL)
	{
		GLOGE("function: %s, line: %d,find method error!", __FUNCTION__, __LINE__);
		return jvm->DetachCurrentThread();
	}
	else
	{
		jstring strOuterIp = menv->NewStringUTF(outerip);
		menv->CallVoidMethod(mobj, methodID_func, strOuterIp, port, tranidArray);
	}


	menv->ReleaseByteArrayElements(tranidArray, bytes, 0);
	menv->DeleteLocalRef(tranidArray);

	return jvm->DetachCurrentThread();
}

