#ifndef __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H

#include "protobufWrapper/GradidoTransaction.h"
#include "Poco/Timestamp.h"

class TransactionFactory
{
public:
	static std::unique_ptr<model::gradido::GradidoTransaction> createDeferredTransfer(std::unique_ptr<model::gradido::GradidoTransaction> transfer, Poco::Timestamp timeout);
	static std::unique_ptr<model::gradido::GradidoTransaction> createGlobalGroupAdd(const std::string& groupName, const std::string& groupAlias, uint32_t nativeCoinColor);
	static std::unique_ptr<model::gradido::GradidoTransaction> createGroupFriendsUpdate(bool colorFusion);
	static std::unique_ptr<model::gradido::GradidoTransaction> createRegisterAddress(
		const MemoryBin* userPubkey,
		proto::gradido::RegisterAddress_AddressType type,
		const MemoryBin* nameHash,
		const MemoryBin* subaccountPubkey
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createTransactionCreation(
		const MemoryBin* recipientPubkey,
		int64_t amountGddCent,
		uint32_t coinColor,
		Poco::DateTime targetDate
	);
	static std::unique_ptr<model::gradido::GradidoTransaction> createTransactionTransfer(
		const MemoryBin* senderPubkey,
		int64_t amountGddCent,
		uint32_t coinColor,
		const MemoryBin* recipientPubkey
	);

protected:
	static std::unique_ptr<proto::gradido::TransferAmount> createTransferAmount(
		const MemoryBin* recipientPubkey,
		int64_t amountGddCent,
		uint32_t coinColor
	);
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_TRANSACTION_FACTORY_H