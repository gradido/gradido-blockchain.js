#ifndef GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H
#define GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H

#include "sodium.h"
#include "gradido_blockchain/MemoryManager.h"
#include "rapidjson/document.h"

#include "Poco/Mutex.h"

#include <map>

class GRADIDOBLOCKCHAIN_EXPORT AuthenticatedEncryption
{
public:
	AuthenticatedEncryption();
	AuthenticatedEncryption(MemoryBin* privateKey);
	AuthenticatedEncryption(const unsigned char pubkey[crypto_box_PUBLICKEYBYTES]);
	~AuthenticatedEncryption();

	MemoryBin* encrypt(MemoryBin* message, AuthenticatedEncryption* recipiantKey);
	MemoryBin* encrypt(MemoryBin* message, int precalculatedSharedSecretIndex);
	MemoryBin* decrypt(MemoryBin* encryptedMessage, AuthenticatedEncryption* senderKey);
	MemoryBin* decrypt(MemoryBin* encryptedMessage, int precalculatedSharedSecretIndex);

	static size_t getPublicKeySize() { return crypto_box_PUBLICKEYBYTES; }
	static size_t getPrivateKeySize() { return crypto_box_SECRETKEYBYTES; }

	//! return index for the shared secret for this recipiant public key
	int precalculateSharedSecret(AuthenticatedEncryption* recipiantKey);
	bool removePrecalculatedSharedSecret(int index);
	
	unsigned char mPubkey[crypto_box_PUBLICKEYBYTES];
	inline const unsigned char* getPublicKey() const { return mPubkey; }
	inline const MemoryBin* getPrivateKey() const { return mPrivkey; }
protected:
	
	MemoryBin* mPrivkey;

	Poco::FastMutex mPrecalculatedSharedSecretsMutex;
	std::map<int, MemoryBin*> mPrecalculatedSharedSecrets;
	int mPrecalculatedSharedSecretLastIndex;
};


#endif //GRADIDO_NODE_LIB_AUTHENTICATED_ENCRYPTION_H