#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_KEY_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_KEY_H

#include "gradido_blockchain/export.h"
#include "Block.h"

namespace memory {

	/*!
	* @author einhornimmond
	* @date 23.07.2024
	* @brief shorthash from block content for using as key in maps, collision can occure so check for it!
	*/

	struct GRADIDOBLOCKCHAIN_EXPORT BlockKey
	{
		BlockKey(ConstBlockPtr block);

		// Copy constructor
		BlockKey(const BlockKey& other)
			: shortHash(other.shortHash) {}


		// Copy assignment operator
		BlockKey& operator=(const BlockKey& other) {
			shortHash = other.shortHash;
			return *this;
		}

		inline bool operator<(const BlockKey& ob) const {
			return shortHash < ob.shortHash;
		}

		inline bool operator==(const BlockKey& ob) const {
			return shortHash == ob.shortHash;
		}
		inline bool operator!=(const BlockKey& ob) const {
			return shortHash != ob.shortHash;
		}
		int64_t shortHash;
	};
}


#endif //__GRADIDO_BLOCKCHAIN_MEMORY_BLOCK_KEY_H