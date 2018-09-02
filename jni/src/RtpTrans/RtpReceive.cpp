/*
 * RtpReceive.cpp
 *
 *  Created on: Dec 28, 2015
 *      Author: 
 */

#include "RtpReceive.h" 


RtpReceive::RtpReceive():
	mRunning(false),
	mPreStop(false),
	mStreamBase(NULL)
{
}

RtpReceive::~RtpReceive() 
{ 
	// TODO Auto-generated destructor stub
}

bool RtpReceive::initSession(unsigned short localPort)
{
	int iStatus;
	int packet_num = 0;
	mLocalPort = localPort;

	GLOGV("function %s,line:%d init localport:%d ",__FUNCTION__,__LINE__, mLocalPort);

	RTPSessionParams sessionparams;
	RTPUDPv4TransmissionParams transparams;

	sessionparams.SetOwnTimestampUnit(1.0/8000.0);
	sessionparams.SetAcceptOwnPackets(false); 

	transparams.SetPortbase(mLocalPort);
	iStatus = mSession.Create(sessionparams, &transparams);

	if (iStatus >= 0)
	{
		mSession.registerCallback(this);
		mSession.SetDefaultPayloadType(96);
		mSession.SetDefaultMark(true);
		mSession.SetMaximumPacketSize(MAX_PACKET_SIZE);
		mSession.SetDefaultTimestampIncrement(160);
	}
	else
	{
		GLOGE("function: %s, line: %d, mSession.Create failed error: %s", __FUNCTION__, __LINE__, RTPGetErrorString(iStatus).c_str());
		return false;
	}


	return true;
}

bool RtpReceive::deinitSession()
{
	mSession.BYEDestroy(RTPTime(1,0), 0, 0);

	return true;
}

void RtpReceive::registerCallback(IReceiveCallback *base)
{
	mStreamBase = base;
}

bool RtpReceive::startThread()
{
	mRunning = true;
	GLOGD("function %s,line:%d",__FUNCTION__,__LINE__);
	//run("RtpReceive", PRIORITY_URGENT_DISPLAY);
	GLOGD("function %s,line:%d",__FUNCTION__,__LINE__);

	return true;
}

bool RtpReceive::stopThread()
{
	if(false == mRunning)
		return false;

	mRunning = false;
	requestExit();

	return true;
}

bool RtpReceive::extStopThread()
{
	mPreStop = true;
	return stopThread();
}

string& RtpReceive::getSourceIp()
{
	return mSession.getSourceIp();
}

bool RtpReceive::connect(std::string sDestIp, short destPort)
{
	int iStatus=0;

	uint32_t uiDestip = inet_addr(sDestIp.c_str());
	if (uiDestip == INADDR_NONE)
	{
		GLOGE("function %s,line:%d Bad IP address specified",__FUNCTION__,__LINE__);
		return false;
	}

	uiDestip = ntohl(uiDestip);

	mDestAddr.SetIP(uiDestip);
	mDestAddr.SetPort(destPort);

	iStatus = mSession.AddDestination(mDestAddr);
	if(iStatus<0)
	{
		GLOGE("function: %s, line: %d, error: %s", __FUNCTION__, __LINE__, RTPGetErrorString(iStatus).c_str());
		return false;
	}

	return true;
}


bool RtpReceive::disConnect()
{
	mSession.DeleteDestination(mDestAddr);
	GLOGW("function %s,line:%d RtpSender::disConnect 1",__FUNCTION__,__LINE__);
	mSession.ClearDestinations();
	GLOGW("function %s,line:%d RtpSender::disConnect 2",__FUNCTION__,__LINE__);
	return true;
}

int	RtpReceive::SetOnlyAcceptAddrs(const RTPAddress &addr)
{
	return mSession.SetOnlyAcceptAddrs(addr);
}

int RtpReceive::SendNatMessage(const void *data,size_t len, const char*ip, short port, bool isRtpSock)
{
	return mSession.SendNatMessage(data, len, ip, port, isRtpSock);
}

bool RtpReceive::sendBuffer(void*buff, int dataLen, char*hdrextdata, int numhdrextwords, int64_t timeStamp)
{
	int iStatus=0;

	if(dataLen>=MAX_PACKET_SIZE)
	{
		GLOGE("function: %s, line: %d, data len big than max_packet_size.", __FUNCTION__, __LINE__);
		return false;
	}

	GLOGW("function: %s, line: %d, ========================3 len:%d", __FUNCTION__, __LINE__, dataLen);
	iStatus = mSession.SendPacketEx((void *)buff, dataLen, 0, hdrextdata, numhdrextwords); 
	GLOGW("function: %s, line: %d, ========================4 ", __FUNCTION__, __LINE__);

	if(iStatus<0)
	{
		GLOGE("function: %s, line: %d, error: %s", __FUNCTION__, __LINE__, RTPGetErrorString(iStatus).c_str());
		return false;
	}

	return true;
}

bool RtpReceive::threadLoop()
{
	int iStatus;
	while(mRunning)
	{
		mSession.BeginDataAccess();
		unsigned char *buff = NULL;
		
		if (mSession.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				while( mRunning && (pack = mSession.GetNextPacket()) !=NULL)
				{

					GLOGD("got packet! GetExtensionData:%s payloadlength:%d id:%d count:%d source ip:%s \n",
						pack->GetExtensionData(), pack->GetPayloadLength(), pack->GetExtensionID(), pack->GetTimestamp(), mSession.getSourceIp().c_str());

					if (pack->GetPayloadLength()>0)
					{
						buff = pack->GetPayloadData();
						
						if(mStreamBase!=NULL && mRunning)
						{
								mStreamBase->OnReceiveSource(pack->GetTimestamp(), (char*)pack->GetExtensionData(), (void *)buff, pack->GetPayloadLength());
						}
						else
							return false;
					}
					
					if(pack->HasMarker())
					{
					}
					mSession.DeletePacket(pack);
				}
				//
			}while(mRunning && mSession.GotoNextSourceWithData());
		}

		mSession.EndDataAccess();
		usleep(20000);
	}

	if(mPreStop)
	{
		usleep(100000);
		mStreamBase->OnReceiveSource(0, (char*)MIME_H264, 0, 0);
	}

	//RTPTime::Wait(RTPTime(0, 5*1000));
	
	return false;
}

void RtpReceive::OnNatSource(int64_t timeStamp, char*mimeType, void* buffer, int dataLen)
{
	if(mStreamBase)
		mStreamBase->OnReceiveSource(timeStamp, mimeType, buffer, dataLen);
}
