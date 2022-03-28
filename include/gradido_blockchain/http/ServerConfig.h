#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H

#include "Poco/Net/Context.h"
#include "Poco/Util/LayeredConfiguration.h"
#include "gradido_blockchain/export.h"


namespace ServerConfig
{
	// used with bit-operators, so only use numbers with control exactly one bit (1,2,4,8,16...)
	enum GRADIDOBLOCKCHAIN_EXPORT AllowUnsecure {
		NOT_UNSECURE = 0,
		UNSECURE_PASSWORD_REQUESTS = 1,
		UNSECURE_AUTO_SIGN_TRANSACTIONS = 2,
		UNSECURE_CORS_ALL = 4,
		UNSECURE_ALLOW_ALL_PASSWORDS = 8
	};

	GRADIDOBLOCKCHAIN_EXPORT extern AllowUnsecure g_AllowUnsecureFlags;
	GRADIDOBLOCKCHAIN_EXPORT extern Poco::Net::Context::Ptr g_SSL_Client_Context;

	GRADIDOBLOCKCHAIN_EXPORT bool initSSLClientContext(const char* cacertPath);
	GRADIDOBLOCKCHAIN_EXPORT bool readUnsecureFlags(const Poco::Util::LayeredConfiguration& config);
}

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H