/*!
*
* \author: einhornimmond
*
* \date: 25.10.19
*
* \brief: Creation Transaction
*/
#ifndef GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE
#define GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE

#pragma warning(disable:4800)

#include "TransactionBase.h"
#include "proto/gradido/GradidoCreation.pb.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "Poco/DateTime.h"

namespace model {
	namespace gradido {

		class GRADIDOBLOCKCHAIN_EXPORT TransactionCreation : public TransactionBase
		{
		public:
			TransactionCreation(const proto::gradido::GradidoCreation& protoCreation);
			~TransactionCreation();

			int prepare();
			//! TODO: check created sum in the last 3 month if 1.000 per month isn't exceed
			//! maybe ask node server and hope the answer came fast
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, 
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
			) const;

			bool validateTargetDate(uint64_t receivedSeconds) const;

			std::vector<MemoryBin*> getInvolvedAddresses() const;
			uint32_t getCoinColor() const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			inline const std::string& getAmount() const { return mProtoCreation.recipient().amount(); }
			MemoryBin* getRecipientPublicKey() const;
			inline const std::string& getRecipientPublicKeyString() const { return mProtoCreation.recipient().pubkey(); }

			std::string getTargetDateString() const;
			inline Poco::DateTime getTargetDate() const { return DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()); }

		protected:
			const proto::gradido::GradidoCreation& mProtoCreation;
		};
	}
}


#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE