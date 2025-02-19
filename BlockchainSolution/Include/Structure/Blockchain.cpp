#include "pch.h"
#include "Blockchain.h"

Blockchain::Blockchain()
{
	// Create the genesis block
	Block genesisBlock = createGenesisBlock();
	_chain.push_back(genesisBlock);
	utxo = UTXOSet::getInstance();
}

Blockchain::~Blockchain()
{
	utxo = UTXOSet::getInstance();
	_chain.clear();
	_pendingTransactions.clear();
}

void Blockchain::testTransaction(std::string address, uint64_t amt)
{
	Transaction trans(
		{ TxInput(OutPoint("00000000c017ba5e00000000c017ba5e", 0), "Coinbase Coinbase")},
		{ TxOutput(amt, std::to_string(REGULARE_TRANSACTION_TYPE) + RIPEMD_160::hash(SHA256::digest(address))) });
	addTransaction(trans);
}

Block Blockchain::getLatestBlock() const
{
	return _chain.back();
}

bool Blockchain::isBlockValid(const Block& block) const
{
	BlockHeader lastBlock = getLatestBlock().getBlockHeader();
	BlockHeader thisBlock = block.getBlockHeader();
	return (lastBlock.getIndex() == thisBlock.getIndex() - 1
		&& thisBlock.getTimeStamp() > lastBlock.getTimeStamp()
		&& thisBlock.getPreviousHash() == lastBlock.getHash());
}

void Blockchain::addBlock(const Block& block)
{
	if (isBlockValid(block)) {
		_chain.push_back(block);
	}
}

void Blockchain::addTransaction(const Transaction& tx)
{
	// Check if new transaction is already in pending transactions.
	if (std::find(_pendingTransactions.begin(), _pendingTransactions.end(), tx) == _pendingTransactions.end())
	{
		// Not found, so insert the new transaction.
		_pendingTransactions.push_back(tx);
	}
}


void Blockchain::displayBlockchain() const
{
	for (const auto& block : _chain)
	{
		block.displayBlock();
		std::cout << "=========================" << std::endl;
	}
}

void Blockchain::commitBlock()
{
	Block newBlock(_chain.size(), getLatestBlock().getHash());
	for (const auto& tx : _pendingTransactions)
	{
		newBlock.addTransaction(tx);
	}

	_chain.push_back(newBlock);
	addBlockToUtxo(newBlock);
	_pendingTransactions.clear();
}

void Blockchain::addBlockToUtxo(Block block)
{
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

/// <summary>
/// Find transaction in pending transaction, searching for transaction ID
/// </summary>
/// <param name="txID">Transaction ID searching for</param>
/// <returns>Transaction if found. Else returns error transaction</returns>
const Transaction Blockchain::findTransactionInPending(const std::string& txID) const
{
	auto it = std::find_if(_pendingTransactions.begin(), _pendingTransactions.end(), [txID](const Transaction& tx)
		{
		return tx.getTransactionID() == txID;
		});

	if (it != _pendingTransactions.end())
	{
		// Found the transaction
		return *it;
	}
	else 
	{
		// Transaction ID not found
		return Transaction();
	}
}

/// <summary>
/// Find transaction in the blockchain, searching for transaction ID
/// </summary>
/// <param name="txID">Transaction ID searching for</param>
/// <returns>Transaction if found. Else returns error transaction</returns>
const Transaction Blockchain::findTransactionInChain(const std::string& txID) const
{
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
	if (findTransactionInPending(txID).isErrorTransaction() && findTransactionInChain(txID).isErrorTransaction())
	{
		return false;
	}
	return true;
}

bool Blockchain::hasBlock(const std::string& blockHash, const std::string& prevBlockHash) const
{
	if (findBlock(blockHash, prevBlockHash).isErrorBlock())
	{
		return false;
	}
	return true;
}

// Append new headers to the chain if they are valid and follow the current chain.
void Blockchain::appendHeaders(const std::vector<BlockHeader>& newHeaders)
{
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
		_chain.push_back(block);
	}
}

// Return the height (index) of a header if known, otherwise -1.
int Blockchain::getHeightByHash(const std::string& hash) const
{
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
	for (size_t i = 0; i < _chain.size(); i++)
	{
		if (_chain[i]._transactions.empty())
		{
			return getHeadersFrom(i, MAX_HEADERS, "");
		}
	}
	return std::vector<BlockHeader>();
}

Block Blockchain::createGenesisBlock()
{
	Block genesis(0, "0");
	return genesis;
}
