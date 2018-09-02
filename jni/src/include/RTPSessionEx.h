#ifndef RTP_SESSION_EXTERNT
#define RTP_SESSION_EXTERNT

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtprawpacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsourcedata.h"
#include <iostream>
#include <string>

#include "IReceiveCallback.h"

#include "ComDefine.h"
#define	 TAG "RTPSessionEx"

using namespace std;
using namespace jrtplib;

namespace android 
{
	class RTPSessionEx : public RTPSession
	{
		public:
			RTPSessionEx();
			~RTPSessionEx();
			string& getSourceIp();
			bool IsSameAddress(const RTPAddress *addr) const;
			bool IsFromSameHost(const RTPAddress *addr) const;
			
			void registerCallback(IReceiveCallback *base);

		protected:
			void OnNewSource(RTPSourceData *dat);
			void OnBYEPacket(RTPSourceData *dat);
			void OnRemoveSource(RTPSourceData *dat);
			void OnNATPacket(RTPRawPacket *pack);

		private:
			string	mSourceIp;
			IReceiveCallback		*mStreamBase;
	};
};

#endif