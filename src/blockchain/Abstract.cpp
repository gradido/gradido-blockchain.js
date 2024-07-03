#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Exceptions.h"

namespace gradido {
	namespace blockchain {

		Abstract::Abstract(std::string_view communityId)
			: mCommunityId(communityId)
		{

		}

		std::shared_ptr<TransactionEntry> Abstract::findOne(const Filter& filter/* = Filter::LAST_TRANSACTION*/) const
		{
			auto results = findAll(filter);
			if (!results.size()) { 
				return nullptr; 
			}
			if (results.size() > 1) {
				throw TransactionResultCountException("to many transactions found with blockchain::Abstract::findOne", 1, results.size());
			}
			return results.front();
		}

			
		data::AddressType Abstract::getAddressType(memory::ConstBlockPtr publicKey, const Filter& filter/* = Filter::ALL_TRANSACTIONS */) const
		{
			// copy filter
			Filter f(filter);
			f.filterFunction = [&publicKey, filter](const TransactionEntry& entry) -> FilterResult {
				if (filter.filterFunction) {
					// evaluate filter from caller
					auto result = filter.filterFunction(entry);
					if ((result & FilterResult::USE) != FilterResult::USE) {
						return result;
					}
				}
				auto body = entry.getTransactionBody();
				if (body->isRegisterAddress() && body->isInvolved(publicKey)) {
					return FilterResult::USE | FilterResult::STOP;
				}
				return FilterResult::DISMISS;
			};
			auto transactionEntry = findOne(f);
			if (transactionEntry) {
				return transactionEntry->getTransactionBody()->registerAddress->addressType;
			}
			return data::AddressType::NONE;
		}

		std::shared_ptr<TransactionEntry> Abstract::findByMessageId(
			memory::ConstBlockPtr messageId,
			const Filter& filter /*= Filter::ALL_TRANSACTIONS*/
		) const
		{
			// copy filter
			Filter f(filter);
			f.filterFunction = [&messageId, filter](const TransactionEntry& entry) -> FilterResult {
				if (filter.filterFunction) {
					// evaluate filter from caller
					auto result = filter.filterFunction(entry);
					if ((result & FilterResult::USE) != FilterResult::USE) {
						return result;
					}
				}
				if (messageId->isTheSame(entry.getConfirmedTransaction()->messageId)) {
					return FilterResult::USE | FilterResult::STOP;
				}
				return FilterResult::DISMISS;
				};
			return findOne(f);
		}
	}
}