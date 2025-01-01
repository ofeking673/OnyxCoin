#include "Blockchain.h"

Blockchain* Blockchain::_instance = nullptr;
Blockchain::Blockchain()
{
	// Create the genesis block
	Block genesisBlock = createGenesisBlock();
	_chain.push_back(genesisBlock);
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
		{ TxInput(OutPoint("", 0), "Coinbase") },
		{ TxOutput(amt, SHA256::digest(address)) });
	addTransaction(trans);
}

Block Blockchain::getLatestBlock() const
{
	return _chain.back();
}

void Blockchain::addTransaction(const Transaction& tx)
{
	_pendingTransactions.push_back(tx);
}

bool Blockchain::submitMiningHash(const std::string address, std::string finalHash, int nonce)
{
	std::string hash = SHA256::digest(getCurrentBlockInfo(address) + std::to_string(nonce));
	
	if (hash.starts_with('0') && hash == finalHash) {
		//loop over all transactions, get fees (0.01) from all outputs, make new transaction from source "Coinbase" HASHED SHA256->RIPE
		uint64_t taxAmt = 0;
		for (const auto& tx : _pendingTransactions)
		{
			taxAmt += tx.calculateTax();
		}

		Transaction trans(
			{ TxInput(OutPoint("", 0), "Coinbase") },
			{ TxOutput(taxAmt, SHA256::digest(address)) });
		addTransaction(trans);
		commitBlock();
		
	}
	return false;
}

std::string Blockchain::getCurrentBlockInfo(const std::string& minerAddress)
{
	Block newBlock(_chain.size(), getLatestBlock().getHash());
	uint64_t taxAmt = 0;
	for (const auto& tx : _pendingTransactions)
	{
		taxAmt += tx.calculateTax();
		newBlock.addTransaction(tx);
	}
	Transaction trans(
		{ TxInput(OutPoint("", 0), "Coinbase") },
		{ TxOutput(taxAmt, SHA256::digest(minerAddress)) });

	newBlock.addTransaction(trans);

	return newBlock.getCurrentBlockInfo();
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
	for (Transaction& tx : block._transactions) {
		//Need to compute outpoint and UTXOData
		auto outputs = tx.getOutputs();
		for (int i = 0; i < outputs.size(); i++)
		{
			OutPoint out = tx.generateOutpoint(outputs[i]);
			UTXOData data(outputs[i].getValue(), outputs[i].getScriptPubKey());
			utxo->addUTXO(out, data);
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

Block Blockchain::createGenesisBlock()
{
	Block genesis(0, "0");
	return genesis;
}
