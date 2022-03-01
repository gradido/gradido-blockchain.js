#include "gradido_blockchain/model/protobufWrapper/TransactionCreation.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "Poco/DateTimeFormatter.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include <sodium.h>


namespace model {
	namespace gradido {

		TransactionCreation::TransactionCreation(const proto::gradido::GradidoCreation& protoCreation)
			: mProtoCreation(protoCreation)
		{
		}

		TransactionCreation::~TransactionCreation()
		{

		}

		int TransactionCreation::prepare()
		{
			if (mIsPrepared) return 0;

			if (!mProtoCreation.has_recipient()) {
				throw TransactionValidationInvalidInputException("not exist", "recipient", "TransferAmount");
			}
			auto recipient_amount = mProtoCreation.recipient();

			auto receiverPublic = recipient_amount.pubkey();
			if (receiverPublic.size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "pubkey", "public key");
			}
	
			//
			mMinSignatureCount = 1;
			auto mm = MemoryManager::getInstance();
			auto pubkey_copy = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			memcpy(*pubkey_copy, receiverPublic.data(), crypto_sign_PUBLICKEYBYTES);
			mForbiddenSignPublicKeys.push_back(pubkey_copy);

			mIsPrepared = true;
			return 0;
		}

		std::string TransactionCreation::getTargetDateString() const
		{
			// proto format is seconds, poco timestamp format is microseconds
			Poco::Timestamp pocoStamp(mProtoCreation.target_date().seconds() * 1000 * 1000);
			//Poco::DateTime(pocoStamp);
			return Poco::DateTimeFormatter::format(pocoStamp, "%d. %b %y");
		}

		bool TransactionCreation::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, 
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				auto mm = MemoryManager::getInstance();				
				auto amount = mProtoCreation.recipient().amount();
				if (amount > 1000 * 10000) {
					throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "integer");
				}
				if (amount < 10000) {
					throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "integer");
				}

				if (mProtoCreation.recipient().pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "recipient pubkey", "public key");
				}
				auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
				memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
				if (0 == memcmp(mProtoCreation.recipient().pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationInvalidInputException("empty", "recipient pubkey", "public key");
				}
			}

			if ((level & TRANSACTION_VALIDATION_DATE_RANGE) == TRANSACTION_VALIDATION_DATE_RANGE) 
			{
				Poco::DateTime targetDate = Poco::Timestamp(mProtoCreation.target_date().seconds() * Poco::Timestamp::resolution());
				assert(blockchain);
				assert(parentGradidoBlock);
					
				auto pubkey = mProtoCreation.recipient().pubkey();
				uint64_t sum = mProtoCreation.recipient().amount();				
				sum += blockchain->calculateCreationSum(pubkey, targetDate.month(), targetDate.year(), parentGradidoBlock->getReceived());
				
				auto id = parentGradidoBlock->getID();
				int lastId = 0;
				if (blockchain->getLastTransaction()) {
					lastId = blockchain->getLastTransaction()->getID();
				}
				if (id <= lastId) {
					// this transaction was already added to blockchain and therefor also added in calculateCreationSum
					sum -= mProtoCreation.recipient().amount();
				}
				// TODO: replace with variable, state transaction for group
				if (sum > 10000000) {
					throw TransactionValidationInvalidInputException("creation more than 1.000 GDD per month not allowed", "amount");
				}
			}
			
			return true;
		}

		bool TransactionCreation::validateTargetDate(uint64_t receivedSeconds) const
		{
			auto target_date = Poco::DateTime(DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()));
			auto received = Poco::DateTime(Poco::Timestamp(receivedSeconds * Poco::Timestamp::resolution()));
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == received.year())
			{
				if (target_date.month() + 2 < received.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is more than 2 month in past", "target_date", "date time");
				}
				if (target_date.month() > received.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is in future", "target_date", "date time");
				}
			}
			else if (target_date.year() > received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in future", "target_date", "date time");
			}
			else if (target_date.year() + 1 < received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in past", "target_date", "date time");
			}
			else
			{
				// target_date.year +1 == now.year
				if (target_date.month() + 2 < received.month() + 12) {
					throw TransactionValidationInvalidInputException("target date is more than 2 month in past", "target_date", "date time");
				}
			}
		}

		std::vector<MemoryBin*> TransactionCreation::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			auto recipientPubkeySize = mProtoCreation.recipient().pubkey().size();
			auto recipientPubkey = mm->getMemory(recipientPubkeySize);
			memcpy(*recipientPubkey, mProtoCreation.recipient().pubkey().data(), recipientPubkeySize);
			return { recipientPubkey };
		}

		uint32_t TransactionCreation::getCoinColor() const
		{
			return mProtoCreation.recipient().coin_color();
		}


	}
}

