#include "Transaction.h"
#include "json.hpp"

using json = nlohmann::json;

Transaction::Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs)
	: _inputs(std::move(inputs))
	, _outputs(std::move(outputs))
	, _timestamp(std::time(nullptr))
{
	_transactionID = generateTransactionID();
}

std::vector<TxInput> Transaction::getInputs() const
{
	return _inputs;
}

std::vector<TxOutput> Transaction::getOutputs() const
{
	return _outputs;
}

std::string Transaction::getTransactionID() const
{
	return _transactionID;
}

time_t Transaction::getTimestamp() const
{
	return _timestamp;
}

void Transaction::addInput(const TxInput& input)
{
	_inputs.push_back(input);
}

void Transaction::addOutput(const TxOutput& output)
{
	_outputs.push_back(output);
}

void Transaction::signTransaction(const std::string& privateKey)
{
	ECDSASigner signer;
	KeyGenerator keyGenerator;

	// Derive public key from private key
	Point* pointPublicKey = keyGenerator.ECMul(ECDSASigner::hexStringToCppInt(privateKey), keyGenerator.GPoint);
	std::string hexPublicKeyPart1 = ECDSASigner::cppIntToHexString(pointPublicKey->_x);
	std::string hexPublicKeyPart2 = ECDSASigner::cppIntToHexString(pointPublicKey->_y);
	std::string hexPublicKey = hexPublicKeyPart1 + hexPublicKeyPart2;

	// Sign transaction
	Point* pointSignature = signer.signMessage(ECDSASigner::hexStringToCppInt(privateKey), this->toString());
	std::string signaturePart1 = ECDSASigner::cppIntToHexString(pointSignature->_x);
	std::string signaturePart2 = ECDSASigner::cppIntToHexString(pointSignature->_y);
	std::string signature = signaturePart1+signaturePart2;

	// Script signature = <signature + public key>
	std::string scriptSig = signature + hexPublicKey;

	// Set script signature of all transaction inputs 
	for (auto input : _inputs)
	{
		input.setScriptSig(scriptSig);
	}
}

void Transaction::displayTransaction() const
{
	std::cout << this->toString();
}

std::string Transaction::toString() const
{
	// A plain text representation
	std::ostringstream oss;
	oss << "Transaction:\n";
	oss << "  ID: " << _transactionID << "\n";
	oss << "  Timestamp: " << _timestamp << "\n";
	oss << "  Inputs:\n";
	for (size_t i = 0; i < _inputs.size(); ++i)
	{
		const TxInput& in = _inputs[i];
		oss << "    [" << i << "] TxID: " << in.getPreviousOutPoint().getTxID()
			<< ", Index: " << in.getPreviousOutPoint().getIndex()
			<< ", ScriptSig: " << in.getScriptSig() << "\n";
	}
	oss << "  Outputs:\n";
	for (size_t i = 0; i < _outputs.size(); ++i)
	{
		const TxOutput& out = _outputs[i];
		oss << "    [" << i << "] Value: " << out.getValue()
			<< ", ScriptPubKey: " << out.getScriptPubKey() << "\n";
	}
	return oss.str();
}

// Use the nlohmann/json library for JSON serialization
std::string Transaction::toJson() const
{
	json j;
	j["transactionID"] = _transactionID;
	j["timestamp"] = static_cast<uint64_t>(_timestamp);

	json inputsJson = json::array();
	for (const auto& in : _inputs)
	{
		json inputObj;
		inputObj["txid"] = in.getPreviousOutPoint().getTxID();
		inputObj["index"] = in.getPreviousOutPoint().getIndex();
		inputObj["scriptSig"] = in.getScriptSig();
		inputsJson.push_back(inputObj);
	}
	j["inputs"] = inputsJson;

	json outputsJson = json::array();
	for (const auto& out : _outputs)
	{
		json outputObj;
		outputObj["value"] = out.getValue();
		outputObj["scriptPubKey"] = out.getScriptPubKey();
		outputsJson.push_back(outputObj);
	}
	j["outputs"] = outputsJson;

	// Return JSON string
	return j.dump();
}








std::string Transaction::generateTransactionID()
{
	std::stringstream ss;

	// Include all transasction data in ss
	for (auto input : _inputs)
	{
		ss << input;
	}
	for (auto output : _outputs)
	{
		ss << output;
	}
	ss << _timestamp;



	// Hash the data to create a 16 bytes transaction ID

	// Convert ss into uint8_t*
	std::string strTx = ss.str();
	uint8_t* buffer = new uint8_t[strTx.size()];
	std::memcpy(buffer, strTx.data(), strTx.size());

	// Create output hash buffer
	uint8_t* hashedID = new uint8_t[TRANSACTION_ID_LENGTH];

	// Hash the transaction data
	Blake2b::hash(buffer, strTx.size(), hashedID, TRANSACTION_ID_LENGTH);

	//  Convert transaction ID to hex string
	std::string transactionID = Blake2b::bytesToHex(hashedID, TRANSACTION_ID_LENGTH);

	// Clear memory used
	delete[] buffer;
	delete[] hashedID;

	return transactionID;
}


