#include "Transaction.h"
#include "json.hpp"

using json = nlohmann::json;

Transaction::Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs, bool forMine)
	: _inputs(std::move(inputs))
	, _outputs(std::move(outputs))
	, _timestamp(std::time(nullptr))
{
	_transactionID = generateTransactionID(forMine);
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

OutPoint Transaction::generateOutpoint(const TxOutput& output)
{
	int i = getOutputIndex(output);
	if(i != -1) {
		OutPoint out(_transactionID, i);
		return out;
	}
}

uint64_t Transaction::calculateTax() const
{
	uint64_t tax = 0;
	for(const TxOutput& output : getOutputs()) 
	{
		tax += output.getValue() * TAX_RATE; //Tax rate
	}
	return tax;
}

int Transaction::getOutputIndex(const TxOutput& output)
{
	auto it = std::find(_outputs.begin(), _outputs.end(), output);
	if (it != _outputs.end()) {
		return std::distance(_outputs.begin(), it); // Compute the index
	}
	else {
		return -1; // Return -1 if the output is not found
	}
}

void Transaction::signTransaction(const std::string& privateKey)
{
	ECDSASigner signer;
	KeyGenerator keyGenerator;

	// Set script signature of all transaction inputs to empty
	for (auto input : _inputs)
	{
		input.setScriptSig("");
	}

	// Derive public key from private key
	Point* pointPublicKey = keyGenerator.ECMul(ECDSASigner::hexStringToCppInt(privateKey), keyGenerator.GPoint);
	std::string publicKey = pointPublicKey->ToStringNoSeperator();

	// Sign transaction
	Point* pointSignature = signer.signMessage(ECDSASigner::hexStringToCppInt(privateKey), transactionMessageToSign());
	std::string signature = pointSignature->ToStringNoSeperator();

	// Script signature = <signature + public key>
	std::string scriptSig = signature + " " + publicKey;

	// Set script signature of all transaction inputs 
	for (auto input : _inputs)
	{
		input.setScriptSig(scriptSig);
	}
}

bool Transaction::verifyTransactionSignature(const std::string& scriptPubKey)
{
	// If there is no ID can't verify
	if (_transactionID.empty()) 
	{
		return false;
	}

	// For each input, parse the signature + pubKey, retrieve the *previous* output’s scriptPubKey,
	// 
	// check that the hashed pubKey matches that scriptPubKey’s <pubKeyHash>, 
	// 
	// then run ECDSA verification on the transaction ID (or partial-hash).
	for (size_t i = 0; i < _inputs.size(); i++)
	{
		const TxInput& input = _inputs[i];

		// Parse scriptSig => <signatureHex> + <publicKeyHex>
		std::istringstream iss(input.getScriptSig());
		std::string signatureHex;
		std::string pubKeyHex;
		if (!(iss >> signatureHex >> pubKeyHex)) 
		{
			std::cerr << "Invalid scriptSig format in input " << i << "\n";
			return false;
		}

		// Extract the <pubKeyHash> from scriptPubKey
		std::string expectedPubKeyHash = extractPublicKeyHash(scriptPubKey);

		// Hash the pubKeyHex from scriptSig => check it matches
		std::string computedPubKeyHash = hashPublicKey(pubKeyHex);
		if (computedPubKeyHash != expectedPubKeyHash) 
		{
			std::cerr << "PubKey hash mismatch in input " << i << "\n";
			return false;
		}

		// ECDSA verify:
		//    Rebuild signature Point from signatureHex
		Point* rs = Point::parseHexString(signatureHex);
		if (!rs) {
			std::cerr << "Invalid signature format in input " << i << "\n";
			return false;
		}

		//    Rebuild publicKey Point from pubKeyHex
		Point* pubKeyPoint = Point::parseHexString(pubKeyHex);
		if (!pubKeyPoint) {
			std::cerr << "Invalid public key format in input " << i << "\n";
			delete rs;
			return false;
		}

		//    Verify ECDSA
		ECDSASigner signer;
		bool verified = signer.verifySignature(rs, transactionMessageToSign(), pubKeyPoint);

		// Cleanup
		delete rs;
		delete pubKeyPoint;

		if (!verified) 
		{
			std::cerr << "Signature verification failed in input " << i << "\n";
			return false;
		}
	}

	// All inputs validated
	return true;
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
	//inputs = transaction references
	j["inputs"] = inputsJson;

	json outputsJson = json::array();
	for (const auto& out : _outputs)
	{
		json outputObj;
		outputObj["value"] = out.getValue();
		outputObj["scriptPubKey"] = out.getScriptPubKey();
		outputsJson.push_back(outputObj);
	}
	//outputs = actions with those references
	j["outputs"] = outputsJson;

	// Return JSON string
	return j.dump();
}

Transaction Transaction::fromJson(const std::string& jsonStr)
{
	// Parse the incoming string as JSON
	json j = json::parse(jsonStr);

	// Extract transactionID and timestamp
	std::string txid = j["transactionID"].get<std::string>();
	uint64_t    ts = j["timestamp"].get<uint64_t>();
	time_t      tstamp = static_cast<time_t>(ts);

	// Reconstruct the inputs
	std::vector<TxInput> inputs;
	for (auto& inputData : j["inputs"])
	{
		std::string prevTxID = inputData["txid"].get<std::string>();
		uint32_t    index = inputData["index"].get<uint32_t>();
		std::string script = inputData["scriptSig"].get<std::string>();

		TxInput txin(OutPoint(prevTxID, index), script);
		inputs.push_back(txin);
	}

	// Reconstruct the outputs
	std::vector<TxOutput> outputs;
	for (auto& outputData : j["outputs"])
	{
		uint64_t    value = outputData["value"].get<uint64_t>();
		std::string scriptPubKey = outputData["scriptPubKey"].get<std::string>();

		TxOutput txout(value, scriptPubKey);
		outputs.push_back(txout);
	}

	// Construct a Transaction from the parsed data
	Transaction tx(inputs, outputs);

	// Overwrite the auto-generated fields with the parsed data
	tx._transactionID = txid;
	tx._timestamp = tstamp;

	return tx;
}



std::string Transaction::generateTransactionID(bool miningTrans)
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
	if (!miningTrans) { ss << _timestamp; }

	std::cout << __FUNCTION__": Transaction ID is: " << ss.str() << std::endl;

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

std::string Transaction::hashPublicKey(const std::string& hexPubKey)
{
	SHA256 sha256;
	return RIPEMD_160::hash(sha256.digest(hexPubKey));
}

std::string Transaction::extractPublicKeyHash(const std::string& scriptPubKey)
{
	//scriptPubKey = <type (1 byte)><public key hash (20 bytes)>
	return scriptPubKey.substr(2, scriptPubKey.size());
}

std::string Transaction::extractTransactionType(const std::string& scriptPubKey)
{
	// First byte of scriptPubKey is type
	return scriptPubKey.substr(0, 2);
}

bool Transaction::isTransactionCreationSucceeded()
{
	if (_inputs.empty() && _outputs.empty())
	{
		return false;
	}
	return true;
}

std::string Transaction::transactionMessageToSign()
{
	Transaction copyTx(*this);
	// Set script signature of all transaction inputs to empty
	for (auto input : copyTx._inputs)
	{
		input.setScriptSig("");
	}

	return SHA256::digest(copyTx.toString());
}

