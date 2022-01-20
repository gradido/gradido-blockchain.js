#include "MemoryManager.h"
#include "sodium.h"
#include <memory.h>
#include <exception>

#define _DEFAULT_PAGE_SIZE 10

MemoryBin::MemoryBin(uint32_t size)
	: mSize(size), mData(nullptr)
{
	mData = (unsigned char*)malloc(size);
}

MemoryBin::~MemoryBin()
{
	if (mData) {
		free(mData);
		mData = nullptr;
	}
}

std::string MemoryBin::convertToHex()
{
	auto mm = MemoryManager::getInstance();
	
	uint32_t hexSize = mSize * 2 + 1;
	auto hexMem = mm->getFreeMemory(hexSize);
	//char* hexString = (char*)malloc(hexSize);
	memset(*hexMem, 0, hexSize);
	sodium_bin2hex(*hexMem, hexSize, mData, mSize);
	std::string hex = (char*)*hexMem;
	//	free(hexString);
	mm->releaseMemory(hexMem);

	return hex;
}

std::string MemoryBin::copyAsString()
{
	return std::string((const char*)mData, mSize);
}

int MemoryBin::convertFromHex(const std::string& hex)
{
	auto mm = MemoryManager::getInstance();
	size_t hexSize = hex.size();
	size_t binSize = (hexSize) / 2;
	if (binSize > mSize) {
		return -1;
	}
	memset(mData, 0, mSize);

	size_t resultBinSize = 0;

	if (0 != sodium_hex2bin(mData, binSize, hex.data(), hexSize, nullptr, &resultBinSize, nullptr)) {
		return -2;
	}
	return 0;
}

bool MemoryBin::isSame(const MemoryBin* b) const
{
	if (b->size() != size()) {
		return false;
	}
	return 0 == memcmp(data(), b->data(), size());
}


// *************************************************************

MemoryPageStack::MemoryPageStack(uint16_t size)
	: mSize(size)
{
	mMemoryBinStack.push(new MemoryBin(size));
}

MemoryPageStack::~MemoryPageStack()
{
	lock();
	while (mMemoryBinStack.size() > 0) {
		MemoryBin* memoryBin = mMemoryBinStack.top();
		mMemoryBinStack.pop();
		delete memoryBin;
		
	}
	mSize = 0;
	unlock();
}

MemoryBin* MemoryPageStack::getFreeMemory()
{
	std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);
	
	if (!mSize) {
		return nullptr;
	}
	if (mMemoryBinStack.size() == 0) {
		return new MemoryBin(mSize);
	}
	MemoryBin* memoryBin = mMemoryBinStack.top();
	mMemoryBinStack.pop();
	
	return memoryBin;
}
void MemoryPageStack::releaseMemory(MemoryBin* memory)
{
	if (!memory) return;
	std::scoped_lock<std::recursive_mutex> _lock(mWorkMutex);
	
	if (memory->size() != mSize) {
		throw std::runtime_error("MemoryPageStack::releaseMemory wron memory page stack");
	}
	mMemoryBinStack.push(memory);
	
}

// ***********************************************************************************

MemoryManager* MemoryManager::getInstance()
{
	static MemoryManager only;
	return &only;
}


MemoryManager::MemoryManager()
{
	mMemoryPageStacks[0] = new MemoryPageStack(32); // pubkey
	mMemoryPageStacks[1] = new MemoryPageStack(64); // privkey
	mMemoryPageStacks[2] = new MemoryPageStack(65); // pubkey hex
	mMemoryPageStacks[3] = new MemoryPageStack(96); // privkey encrypted
	mMemoryPageStacks[4] = new MemoryPageStack(161); // privkey hex 
	mMemoryPageStacks[5] = new MemoryPageStack(48); // word indices
}

MemoryManager::~MemoryManager()
{
	for (int i = 0; i < 5; i++) {
		delete mMemoryPageStacks[i];
	}
}

int8_t MemoryManager::getMemoryStackIndex(uint16_t size)
{
	switch (size) {
	case 32: return 0;
	case 64: return 1;
	case 65: return 2;
	case 96: return 3;
	case 161: return 4;
	case 48: return 5;
	default: return -1;
	}
	return -1;
}




MemoryBin* MemoryManager::getFreeMemory(uint32_t size)
{
	if (size != (uint32_t)((uint16_t)size)) {
		throw std::runtime_error("MemoryManager::getFreeMemory size is to large, only 16 Bit allowed");
	}
	auto index = getMemoryStackIndex(size);
	if (index < 0) {
		return new MemoryBin(size);
	}
	else {
		return mMemoryPageStacks[index]->getFreeMemory();
	}
	return nullptr;
}

void MemoryManager::releaseMemory(MemoryBin* memory)
{
	if (!memory) return;
	auto index = getMemoryStackIndex(memory->size());
	if (index < 0) {
		delete memory;
	}
	else {
		mMemoryPageStacks[index]->releaseMemory(memory);
	}
}
