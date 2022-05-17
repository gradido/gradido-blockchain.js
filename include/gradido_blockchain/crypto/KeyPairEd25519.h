#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H


/*!
 * \author: Dario Rekowski
 *
 * \date: 2020-06-04
 *
 * \brief: Key Pairs class for ed25519 keys, used for default gradido transactions
 * TODO: add verify method
*/


#include "sodium.h"
#include "SecretKeyCryptography.h"
#include "Passphrase.h"
#include "../lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

class GRADIDOBLOCKCHAIN_EXPORT KeyPairEd25519
{
public:
	//! \param privateKey: take ownership, release after object destruction
	//! \param publicKey: copy
	KeyPairEd25519(MemoryBin* privateKey);
	KeyPairEd25519(const unsigned char* publicKey);

	~KeyPairEd25519();

	//! \param passphrase must contain word indices
	//! \return create KeyPairEd25519, caller muss call delete at return after finish
	static std::unique_ptr<KeyPairEd25519> create(const std::shared_ptr<Passphrase> passphrase);

	//! \return caller take ownership of return value
	MemoryBin* sign(const MemoryBin* message) const { return sign(message->data(), message->size()); }
	inline MemoryBin* sign(const std::string& bodyBytes) const { return sign((const unsigned char*)bodyBytes.data(), bodyBytes.size()); }
	MemoryBin* sign(const unsigned char* message, size_t messageSize) const;

	//! \return true if signature is valid
	bool verify(const std::string& message, const std::string& signature) const;

	inline const unsigned char* getPublicKey() const { return mSodiumPublic; }
	MemoryBin* getPublicKeyCopy() const;
	inline std::string getPublicKeyHex() const { return DataTypeConverter::binToHex(mSodiumPublic, getPublicKeySize()); }
	const static size_t getPublicKeySize() { return crypto_sign_PUBLICKEYBYTES; }

	inline bool isTheSame(const KeyPairEd25519& b) const {
		return 0 == sodium_memcmp(mSodiumPublic, b.mSodiumPublic, crypto_sign_PUBLICKEYBYTES);
	}
	inline bool isTheSame(const unsigned char* pubkey) const {
		if (!pubkey)
			return false;
		return 0 == sodium_memcmp(mSodiumPublic, pubkey, crypto_sign_PUBLICKEYBYTES);
	}
	//! \return 0 if the same
	//! \return -1 if not the same
	//! \return 1 if hasn't private key
	inline int isTheSame(const MemoryBin* privkey) const {
		if (!mSodiumSecret) return 1;
		if (privkey->size() != mSodiumSecret->size()) return -1;
		return sodium_memcmp(*mSodiumSecret, *privkey, privkey->size());
	}

	inline bool operator == (const KeyPairEd25519& b) const { return isTheSame(b);  }
	inline bool operator != (const KeyPairEd25519& b) const { return !isTheSame(b); }

	inline bool operator == (const unsigned char* b) const { return isTheSame(b); }
	inline bool operator != (const unsigned char* b) const { return !isTheSame(b); }

	inline bool hasPrivateKey() const { return mSodiumSecret != nullptr; }
	inline const MemoryBin* getPrivateKey() const { return mSodiumSecret; }

	MemoryBin* getCryptedPrivKey(const std::shared_ptr<SecretKeyCryptography> password) const;

protected:

	KeyPairEd25519();


private:
	// 64 Byte
	//! \brief ed25519 libsodium private key
	//!
	//! Why it is a pointer and the public is an array?
	//! Because MemoryBin should be replaced by a memory obfuscation class which make it harder to steal the private key from computer memory
	//! And because private key can be nullptr for example to verify a signed message

	//! TODO: replace MemoryBin by a memory obfuscation class which make it hard to steal the private key from memory
	MemoryBin* mSodiumSecret;

	// 32 Byte
	//! \brief ed25519 libsodium public key
	unsigned char mSodiumPublic[crypto_sign_PUBLICKEYBYTES];
};

// *********************** Exceptions ****************************
class GRADIDOBLOCKCHAIN_EXPORT Ed25519SignException : public GradidoBlockchainException
{
public:
	explicit Ed25519SignException(const char* what, MemoryBin* pubkey, const std::string& message) noexcept;
	explicit Ed25519SignException(const char* what, const unsigned char* pubkey, const std::string& message) noexcept;
	~Ed25519SignException();
	std::string getFullString() const;

protected:
	MemoryBin* mPubkey;
	std::string mMessage;
};

#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_H
