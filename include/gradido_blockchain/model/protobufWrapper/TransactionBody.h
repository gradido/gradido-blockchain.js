#ifndef GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
#define GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H

#include "proto/gradido/TransactionBody.pb.h"

#include "DeferredTransfer.h"
#include "GroupFriendsUpdate.h"
#include "RegisterAddress.h"
#include "TransactionCreation.h"
#include "TransactionTransfer.h"

#include "Poco/DateTime.h"

#include "../../lib/MultithreadContainer.h"

#define GRADIDO_PROTOCOL_VERSION "3.2"

namespace model {
	namespace gradido {
		/*!
		 *  \addtogroup Gradido-Protocol
 		 *  @{
 		 */

		//!  Contain the actuall transaction details.
		/*!
	  		
			<a href="https://github.com/gradido/gradido_protocol/blob/master/gradido/TransactionBody.proto" target="_blank">Protobuf: TransactionBody</a>
		*/
		class GRADIDOBLOCKCHAIN_EXPORT TransactionBody : public MultithreadContainer
		{
		public:
			virtual ~TransactionBody();

			void setCreated(Poco::DateTime created);
			inline uint32_t getCreatedSeconds() const { return mProtoTransactionBody.created().seconds(); }

			static TransactionBody* load(const std::string& protoMessageBin);
			void upgradeToDeferredTransaction(Poco::Timestamp timeout);
			static TransactionBody* createGroupFriendsUpdate(bool colorFusion);
			static TransactionBody* createRegisterAddress(
				const MemoryBin* userPubkey,
				proto::gradido::RegisterAddress_AddressType type,
				const MemoryBin* nameHash,
				const MemoryBin* subaccountPubkey
			);
			static TransactionBody* createTransactionCreation(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, Poco::DateTime targetDate);
			static TransactionBody* createTransactionTransfer(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, const MemoryBin* recipientPubkey);

			void updateToOutbound(const std::string& otherGroup);
			void updateToInbound(const std::string& otherGroup);

			inline TransactionType getTransactionType() const { return mTransactionType; }
			inline proto::gradido::TransactionBody_CrossGroupType getCrossGroupType() const { return mProtoTransactionBody.type(); }
			inline const std::string& getVersionNumber() const { return mProtoTransactionBody.version_number(); }
			inline const std::string& getOtherGroup() const { return mProtoTransactionBody.other_group(); }
			static const char* transactionTypeToString(TransactionType type);
			std::string getMemo() const;
			void setMemo(const std::string& memo);

			const std::string& getGroupId(const IGradidoBlockchain* blockchain) const;

			inline bool isDeferredTransfer() const { return mTransactionType == TRANSACTION_DEFERRED_TRANSFER; }
			inline bool isGroupFriendsUpdate() const { return mTransactionType == TRANSACTION_GROUP_FRIENDS_UPDATE; }
			inline bool isRegisterAddress() const { return mTransactionType == TRANSACTION_REGISTER_ADDRESS; }
			inline bool isCreation() const { return mTransactionType == TRANSACTION_CREATION; }
			inline bool isTransfer() const  { return mTransactionType == TRANSACTION_TRANSFER; }		
			// cross group types
			inline bool isLocal() const { return mProtoTransactionBody.type() == proto::gradido::TransactionBody_CrossGroupType_LOCAL; }
			inline bool isInbound() const { return mProtoTransactionBody.type() == proto::gradido::TransactionBody_CrossGroupType_INBOUND; }
			inline bool isOutbound() const { return mProtoTransactionBody.type() == proto::gradido::TransactionBody_CrossGroupType_OUTBOUND; }
			inline bool isCross() const { return mProtoTransactionBody.type() == proto::gradido::TransactionBody_CrossGroupType_CROSS; }

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, 
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
				) const;
			bool isBelongToUs(const TransactionBody* pairingTransaction) const;

			std::unique_ptr<std::string> getBodyBytes() const;
			const proto::gradido::TransactionBody* getBody() const { return &mProtoTransactionBody; }

			const DeferredTransfer* getDeferredTransfer() const;
			const GroupFriendsUpdate* getGroupFriendsUpdate() const;
			const RegisterAddress* getRegisterAddress() const;
			const TransactionCreation* getCreationTransaction() const;
			const TransactionTransfer* getTransferTransaction() const;

			const TransactionBase* getTransactionBase() const;
		protected:
			TransactionBody();

			void initSpecificTransaction();
			// TODO: use Pool for reducing memory allocation for google protobuf objects
			proto::gradido::TransactionBody mProtoTransactionBody;
			TransactionBase* mTransactionSpecific;
			TransactionType mTransactionType;
		};
		/*! @} End of Doxygen Groups*/
	}
}

#endif //GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
