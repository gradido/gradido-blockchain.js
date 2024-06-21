#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BUILDER_H

#include "Filter.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			class GRADIDOBLOCKCHAIN_EXPORT FilterBuilder
			{
			public:
				// default is 0
				inline FilterBuilder& setMinTransactionNr(uint64_t _minTransactionNr) { mFilter.minTransactionNr = _minTransactionNr; return *this;}
				// default is 0, no max use all
				inline FilterBuilder& setMaxTransactionNr(uint64_t _maxTransactionNr) { mFilter.maxTransactionNr = _maxTransactionNr; return *this; }
				// default is nullptr, don't filter for public keys
				inline FilterBuilder& setInvolvedPublicKey(memory::ConstBlockPtr _involvedPublicKey) { mFilter.involvedPublicKey = _involvedPublicKey; return *this; }
				// default is DESC
				inline FilterBuilder& setSearchDirection(SearchDirection _searchDirection) { mFilter.searchDirection = _searchDirection; return *this; }
				// default is no pagination, result all 
				inline FilterBuilder& setPagination(Pagination _pagination) { mFilter.pagination = _pagination; return *this;}
				// default is nullptr, don't filter for coin color
				inline FilterBuilder& setCoinCommunityId(std::string_view _coinCommunityId) { mFilter.coinCommunityId = _coinCommunityId; return *this; }
				// default is empty month, don't filter for month
				inline FilterBuilder& setMonth(date::month _month) { mFilter.month = _month; return *this;}
				// default is empty year, don't filter for year
				inline FilterBuilder& setYear(date::year _year) { mFilter.year = _year; return *this; }

				inline FilterBuilder& setFilterFunction(std::function<FilterFunctionResult(const TransactionEntry&)> _filterFunction) {
					mFilter.filterFunction = _filterFunction;
					return *this;
				}

				inline Filter build() const { return mFilter; }
			protected:
				Filter mFilter;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_BUILDER_H