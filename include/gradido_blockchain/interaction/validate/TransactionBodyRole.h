#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace validate {

            class TransactionBodyRole: public AbstractRole 
            {
            public:
				TransactionBodyRole(const data::TransactionBody& body) : mBody(body) {}
				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);

				inline void checkRequiredSignatures(const data::SignatureMap& signatureMap) { getSpecificTransactionRole().checkRequiredSignatures(signatureMap); }
            protected:
				AbstractRole& getSpecificTransactionRole();

				std::unique_ptr<AbstractRole> mSpecificTransactionRole;
				const data::TransactionBody& mBody;
            };
		}
	}
}



#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H