#ifndef __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK
#define __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK

#include "proto/gradido/GradidoBlock.pb.h"
#include "GradidoTransaction.h"

#include "Poco/SharedPtr.h"

#define GRADIDO_BLOCK_PROTOCOL_VERSION 3
// MAGIC NUMBER: max allowed Timespan between creation date of transaction and receiving date (iota milestone timestamp) 
// taken 2 minutes from hedera but maybe the time isn't enough if gradido is more used
#define MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION_IN_MINUTES 2

namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoBlock : public MultithreadContainer, public Poco::RefCountedObject
		{
		public:
			GradidoBlock(const std::string* serializedGradidoBlock);
			~GradidoBlock();

			static Poco::SharedPtr<GradidoBlock> create(std::unique_ptr<GradidoTransaction> transaction, uint64_t id, int64_t received, const MemoryBin* messageId);

			std::string toJson();
			rapidjson::Value toJson(rapidjson::Document& baseDocument);
			inline const GradidoTransaction* getGradidoTransaction() const { return mGradidoTransaction; }

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;
			
			// proto member variable accessors
			inline uint64_t getID() const { return mProtoGradidoBlock->id(); }
			inline const std::string& getTxHash() const { return mProtoGradidoBlock->running_hash(); }
			inline const std::string& getFinalBalance() const { return mProtoGradidoBlock->final_gdd(); }

			inline void setTxHash(const MemoryBin* txHash) { mProtoGradidoBlock->set_allocated_running_hash(txHash->copyAsString().release()); }
			// convert from proto timestamp seconds to poco DateTime
			inline Poco::Timestamp getReceivedAsTimestamp() const {
				return Poco::Timestamp(mProtoGradidoBlock->received().seconds() * Poco::Timestamp::resolution());
			}
			// return as seconds since Jan 01 1970
			inline int64_t getReceived() const { return mProtoGradidoBlock->received().seconds(); }

			std::unique_ptr<std::string> getSerialized();
			//! \return MemoryBin containing message id binar, must be freed from caller
			MemoryBin* getMessageId() const;
			std::string getMessageIdHex() const;

			//! \return caller must free result
			MemoryBin* calculateTxHash(const GradidoBlock* previousBlock) const;

			//! \brief calculate final gdd for this transaction and set value into proto structure
			//! 
			//! for transfer transaction, it is the final balance from sender
			void calculateFinalGDD(IGradidoBlockchain* blockchain);

		protected:
			// TODO: use Pool for reducing memory allocation for google protobuf objects
			proto::gradido::GradidoBlock* mProtoGradidoBlock;
			GradidoTransaction* mGradidoTransaction;
			GradidoBlock(std::unique_ptr<GradidoTransaction> transaction);
		};
	}
}

#endif //__GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK