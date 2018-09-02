
#ifndef I_RECEIVE_CALLBACK
#define I_RECEIVE_CALLBACK


class IReceiveCallback
{
	public:
		virtual ~IReceiveCallback(){}
		virtual void OnReceiveSource(int64_t timeStamp, char*mimeType, void* buffer, int dataLen){}
		virtual void OnNatSource(int64_t timeStamp, char*mimeType, void* buffer, int dataLen){}
};

#endif
