#include "pch.h"
#include "Blockchain.h"

#undef min
#undef max

Blockchain* Blockchain::_instance = nullptr;
bool Blockchain::_isSignleton = false;


Blockchain::Blockchain(const std::string& publicKey)
	: gen(std::random_device{}()),
	dis(0, std::numeric_limits<uint64_t>::max())
{
	// Create the genesis block
	Block genesisBlock = createGenesisBlock(publicKey);
	_chain.push_back(genesisBlock);

	utxo = UTXOSet::getInstance();
	mempool = Mempool::getInstance();

	// Add the genesis block to the UTXOs
	addBlockToUtxo(genesisBlock);

	 
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

}

Blockchain::Blockchain(int)
	: gen(std::random_device{}()),
	dis(0, std::numeric_limits<uint64_t>::max())
{
	_chain.clear();
	std::cout << std::endl << std::endl << "Chain size: " << _chain.size() << std::endl << std::endl;
	
	utxo = UTXOSet::getInstance();
	mempool = Mempool::getInstance();

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
}

Blockchain::~Blockchain()
{
	_chain.clear();
}


void Blockchain::mineNewProposedBlock(Block& proposedBlock, const std::string& minerPublicKey)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	// Add the reward transaction to the miner (me)
	addRewardTransaction(minerPublicKey, proposedBlock);

	// Start with random nonce and check it till reaches a good hash starts with 0s
	uint64_t myNonce = getRandom();
	while (proposedBlock.checkHash(myNonce))
	{
		myNonce++;
	}
}

uint64_t Blockchain::getRandom()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	return dis(gen);
}

void Blockchain::setSingleton()
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_isSignleton = true;
}

Transaction Blockchain::testTransaction(std::string address, uint64_t amt)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	Transaction trans(
		{ TxInput(OutPoint("00000000c017ba5e00000000c017ba5e", 0), "Coinbase Coinbase")},
		{ TxOutput(amt, std::to_string(REWARD_TRANSACTION_TYPE) + RIPEMD_160::hash(SHA256::digest(address))) });
	/*addTransaction(trans);*/

	return trans;
}

void Blockchain::addRewardTransaction(const std::string& address, Block& newBlock)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	uint64_t amt = newBlock.calculateBlockReward();
	Transaction trans(
		{ TxInput(OutPoint("00000000c017ba5e00000000c017ba5e", 0), "Coinbase Coinbase") },
		{ TxOutput(amt, std::to_string(REWARD_TRANSACTION_TYPE) + RIPEMD_160::hash(SHA256::digest(address))) });
	newBlock.addTransaction(trans);
}

Block Blockchain::getLatestBlock() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	return _chain.back();
}

std::string Blockchain::getChain() const
{
	std::stringstream ss;
	for (int i = _chain.size(); i > _chain.size()-6; i--)
	{
		ss << _chain[i].toMessageString() << " ";
	}
	std::string final = ss.str();
	final.pop_back();
	return final;
}

bool Blockchain::isBlockValid(const Block& block) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	BlockHeader lastBlock = getLatestBlock().getBlockHeader();
	BlockHeader thisBlock = block.getBlockHeader();
	bool isHeaderValid = (lastBlock.getIndex() == thisBlock.getIndex() - 1
		&& thisBlock.getTimeStamp() > lastBlock.getTimeStamp()
		&& thisBlock.getPreviousHash() == lastBlock.getHash());

	bool areTransactionsValid = block.verifyBlockTransactions();

	return isHeaderValid && areTransactionsValid;
}

bool Blockchain::addBlock(const Block& block)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (_chain.empty())
	{
		_chain.push_back(block);
		return true;
	}
	else if (isBlockValid(block))
	{
		_chain.push_back(block);
		return true;
	}
	return false;
}

bool Blockchain::addFullBlockToFirstAwaitedHeader(const Block& block)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	// Find the index of the first awaited header
	int awaitedHeaderIndex = -1;
	for (size_t i = 0; i < _chain.size(); i++)
	{
		if (_chain[i]._isAwaitedHeaders)
		{
			awaitedHeaderIndex = i;
			break;
		}
	}
	if (awaitedHeaderIndex == -1)
	{
		// No awaited headers...
		return false;
	}

	// Complete the awaited headers to full block info. Check its correctness
	if (block.calculateHash() == _chain[awaitedHeaderIndex].getHash() &&
		block.getPreviousHash() == _chain[awaitedHeaderIndex].getPreviousHash())
	{
		if (block.verifyBlockTransactions())
		{
			_chain[awaitedHeaderIndex] = block;
			return true;
		}
	}

	// Block doesn't match the awaited header; Or the transactions aren't verified
	return false;
}

void Blockchain::addTransaction(const Transaction& tx)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	// Check if new transaction is already in pending transactions.
	Transaction trans = mempool->getTransaction(tx.getTransactionID());
	if (trans.isErrorTransaction())
	{
		// Not found, so insert the new transaction.
		mempool->addTransaction(tx);
	}


	//if (std::find(_pendingTransactions.begin(), _pendingTransactions.end(), tx) == _pendingTransactions.end())
	//{
	//	// Not found, so insert the new transaction.
	//	_pendingTransactions.push_back(tx);
	//}
}


bool Blockchain::isAvailableToCommitBlock()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (mempool->getPendingTransactionsAmount() >= 5)
	{ // At least 5 Transactions to create a block.
		return true;
	}
	return false;
}

void Blockchain::displayBlockchain() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (const auto& block : _chain)
	{
		block.displayBlock();
		std::cout << "=========================" << std::endl;
	}
}

Block Blockchain::commitBlock(std::string leadersPublicKey)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (!isAvailableToCommitBlock())
	{
		return Block();
	}


	Block newBlock(_chain.size(), getLatestBlock().getHash());

	std::unordered_map<std::string, Transaction> pendingTransactions = mempool->getPendingTransactions();
	for (const auto& tx : pendingTransactions)
	{
		// Add transaction to new block
		newBlock.addTransaction(tx.second);

		// Remove transaction from mempool
		mempool->removeTransaction(tx.second.getTransactionID());
	}

	//Create the reward transaction because we are the leader proposing the block
	addRewardTransaction(leadersPublicKey, newBlock);

	// Indicate the block is not an awaited header
	newBlock._isAwaitedHeaders = false;

	// Should not add yet the block to the chain.
	//_chain.push_back(newBlock);
	//addBlockToUtxo(newBlock);
	return newBlock;
}

void Blockchain::addBlockToUtxo(Block& block)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	//for (Transaction& tx : block._transactions) {
	//	//Need to compute outpoint and UTXOData
	//	auto outputs = tx.getOutputs();
	//	for (int i = 0; i < outputs.size(); i++)
	//	{
	//		OutPoint out = tx.generateOutpoint(outputs[i]);
	//		UTXOData data(outputs[i].getValue(), outputs[i].getScriptPubKey());
	//		utxo->addUTXO(out, data);
	//	}
	//}

	for (Transaction& tx : block._transactions)
	{
		//  Add TxOutputs to UTXO
		std::vector<TxOutput> outputs = tx.getOutputs();
		for (size_t i = 0; i < outputs.size(); i++)
		{
			TxOutput output = outputs[i];
			std::string outputPubKeyHash = tx.extractPublicKeyHash(output.getScriptPubKey());

			UTXOData utxodata(output.getValue(), output.getScriptPubKey());
			OutPoint outpoint(tx.getTransactionID(), i);

			utxo->addUTXO(outpoint, utxodata);			
		}

		//  If a TxInput references one of my outpoints, remove from UTXO
		std::vector<TxInput> inputs = tx.getInputs();
		for (size_t i = 0; i < inputs.size(); i++)
		{
			TxInput input = inputs[i];
			utxo->removeUTXO(input.getPreviousOutPoint());
		}
	}
}

bool Blockchain::isChainValid() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (int i = 1; i < _chain.size(); i++)
	{
		const Block& current = _chain[i];
		const Block& previous = _chain[i - 1];

		if (current.getHash() != current.calculateHash())
		{
			return false;
		}

		if (current.getPreviousHash() != previous.getHash())
		{
			return false;
		}
	}
	return true;
}

std::vector<Transaction> Blockchain::getPendingTransactions() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	std::vector<Transaction> pendingTransactions;
	std::unordered_map<std::string, Transaction> memTx = mempool->getPendingTransactions();
	for (auto& tx : memTx)
	{
		pendingTransactions.push_back(tx.second);
	}
	return pendingTransactions;
}

/// <summary>
/// Find transaction in pending transaction, searching for transaction ID
/// </summary>
/// <param name="txID">Transaction ID searching for</param>
/// <returns>Transaction if found. Else returns error transaction</returns>
const Transaction Blockchain::findTransactionInPending(const std::string& txID) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	return mempool->getTransaction(txID);
}

/// <summary>
/// Find transaction in the blockchain, searching for transaction ID
/// </summary>
/// <param name="txID">Transaction ID searching for</param>
/// <returns>Transaction if found. Else returns error transaction</returns>
const Transaction Blockchain::findTransactionInChain(const std::string& txID) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (auto& blk : _chain)
	{
		Transaction tx = blk.findTransaction(txID);

		if (!tx.isErrorTransaction())
		{
			// Found the transaction
			return tx;
		}
	}
	// Transaction ID not found 
	return Transaction();
}

const Block Blockchain::findBlock(const std::string& blockHash, const std::string& prevBlockHash) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	auto it = std::find_if(_chain.begin(), _chain.end(), [blockHash, prevBlockHash](const Block& block)
		{
			return block.getHash() == blockHash && block.getPreviousHash() == prevBlockHash;
		});

	if (it != _chain.end())
	{
		// Found the Block
		return *it;
	}
	else
	{
		// Block not found
		return Block();
	}
}

const BlockHeader Blockchain::findHeader(const std::string& blockHash, const std::string& prevBlockHash) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	auto it = std::find_if(_chain.begin(), _chain.end(), [blockHash, prevBlockHash](const Block& block)
		{
			return block.getHash() == blockHash && block.getPreviousHash() == prevBlockHash;
		});

	if (it != _chain.end())
	{
		// Found the Block
		return it->getBlockHeader();
	}
	else
	{
		// Block not found
		return BlockHeader();
	}
}

bool Blockchain::hasTransaction(const std::string& txID) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (findTransactionInPending(txID).isErrorTransaction() && findTransactionInChain(txID).isErrorTransaction())
	{
		return false;
	}
	return true;
}

bool Blockchain::hasBlock(const std::string& blockHash, const std::string& prevBlockHash) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (findBlock(blockHash, prevBlockHash).isErrorBlock())
	{
		return false;
	}
	return true;
}

// Append new headers to the chain if they are valid and follow the current chain.
void Blockchain::appendHeaders(const std::vector<BlockHeader>& newHeaders)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (const auto& header : newHeaders) 
	{
		// Example check: the new header's previousHash matches the last chain header's hash
		if (!_chain.empty()) {
			const auto& lastHash = _chain.back().getHash();
			if (header.getPreviousHash() != lastHash) 
			{
				std::cerr << "Header continuity mismatch. Expected prevHash = "
					<< lastHash << ", but got " << header.getPreviousHash() << "\n";
				break; // Stop or fork logic
			}
		}
		Block block(header);

		// Indicate the block is an awaited header
		block._isAwaitedHeaders = true;
		_chain.push_back(block);
	}
}

// Return the height (index) of a header if known, otherwise -1.
int Blockchain::getHeightByHash(const std::string& hash) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (size_t i = 0; i < _chain.size(); i++) 
	{
		if (_chain[i].getHash() == hash)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// Get a sub-range of headers from a given index up to a count or stop hash.
std::vector<BlockHeader> Blockchain::getHeadersFrom(int startIndex, int maxCount, const std::string& stopHash) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	std::vector<BlockHeader> result;
	for (int i = startIndex; i < (int)_chain.size() && result.size() < (size_t)maxCount; i++) 
	{
		if (!stopHash.empty() && _chain[i].getHash() == stopHash) {
			// Include the stopHash's header, then stop.
			result.push_back(_chain[i].getBlockHeader());
			break;
		}
		result.push_back(_chain[i].getBlockHeader());
	}
	return result;
}

// Get all the headers appended to the chain without a real block. 
// In order to request the full blocks
std::vector<BlockHeader> Blockchain::getAppendedHeaders() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (size_t i = 0; i < _chain.size(); i++)
	{
		if (_chain[i]._isAwaitedHeaders)
		{
			return getHeadersFrom(i, MAX_HEADERS, "");
		}
	}
	return std::vector<BlockHeader>();
}

bool Blockchain::isUTXOLocked(const OutPoint& op) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	return mempool->isUTXOReserved(op);
}

Block Blockchain::createGenesisBlock(const std::string& publicKey)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

	Block genesis(0, "0");

	// Create 5 transactions for the creators public key.
	for (size_t i = 1; i <= 5; i++)
	{
		Transaction tx = testTransaction(publicKey, i * 20);
		genesis.addTransaction(tx);
	}

	genesis.setHash(genesis.calculateHash());
	return genesis;
}
