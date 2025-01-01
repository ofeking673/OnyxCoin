#include "RequestHandler.h"
RequestHandler* RequestHandler::_instance = nullptr;
Blockchain* RequestHandler::blockchain = nullptr;
UTXOSet * RequestHandler::utxo = nullptr;

RequestHandler::RequestHandler()
{
	blockchain = Blockchain::getInstance();
	utxo = UTXOSet::getInstance();
}

std::string RequestHandler::mine(std::string& address, json j)
{
	//Get the nonce and new hash from sock
	auto info = blockchain->getCurrentBlockInfo(address);
	std::string expectedHash = j["hash"];
	int nonce = j["nonce"];
	std::string hash = SHA256::digest(info + std::to_string(nonce));
	std::cout << "\nExpected hash: " << hash << "\n\nRecieved hash: " << expectedHash << std::endl << std::endl;
	if (hash == expectedHash) {
		blockchain->submitMiningHash(address, hash, nonce);
	}
	std::string response = JsonPacketSerializer::serializeMiningResponse((hash == expectedHash), 1);
	return response;
}

std::string RequestHandler::transaction(std::string& address, json j)
{
	//parse the transaction
	//make outpoint
	//verify outpoint in utxoSet
	//verify signature of transaction
	//check if input sum is equal to output sum
	//remove input from UTXOSet and add outputs to the set
	//KAPISH

	uint64_t TxInputSum = 0, TxOutputSum = 0;

	Transaction tran = Transaction::fromJson(j.dump());
	std::vector<TxInput> inputs = tran.getInputs();

	for(const TxInput& inp : inputs)
	{
		if (!utxo->hasUTXO(inp.getPreviousOutPoint()))
		{
			throw new std::runtime_error(__FUNCTION__": One of the inputs were used!");
		}
		TxInputSum += utxo->getUTXOData(inp.getPreviousOutPoint()).getValue();
	}

	if(!tran.verifyTransactionSignature(tran.getOutputs()[0].getScriptPubKey())) {
		throw new std::runtime_error(__FUNCTION__": Transaction signature is incorrect!");
	}

	std::vector<TxOutput> outputs = tran.getOutputs();
	for(const TxOutput& out : outputs) {
		TxOutputSum += out.getValue();
	}

	if(TxInputSum != TxOutputSum) {
		throw new std::runtime_error(__FUNCTION__ ": Issue with transaction sum");
	}

	for (const TxInput& inp : inputs) {
		utxo->removeUTXO(inp.getPreviousOutPoint());
	}
	for (const TxOutput& out : outputs) {
		UTXOData data = UTXOData(out.getValue(), out.getScriptPubKey());
		utxo->addUTXO(tran.generateOutpoint(out), data);
	}

	return JsonPacketSerializer::serializeTransactionResponse(true);
}
