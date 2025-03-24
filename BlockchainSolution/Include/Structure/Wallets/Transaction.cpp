#include "pch.h"
#include "Transaction.h"

Transaction::Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs)
	: _inputs(std::move(inputs))
	, _outputs(std::move(outputs))
	, _timestamp(std::time(nullptr))
{
	_transactionID = generateTransactionID();
}

Transaction::Transaction(const Transaction& other)
{
	this->_transactionID = other._transactionID;
	this->_inputs = other._inputs;
	this->_outputs = other._outputs;
	this->_timestamp = other._timestamp;
}

Transaction::Transaction()
{
	_transactionID = std::to_string(ERROR_TRANSACTION_ID);
	_timestamp = std::time(nullptr);
}

/// <summary>
/// Checks if the transaction indicating error
/// </summary>
/// <returns>True if error transaction, false if not error</returns>
bool Transaction::isErrorTransaction() const
{
	if (_transactionID == std::to_string(ERROR_TRANSACTION_ID))
	{
		return true;
	}
	return false;
}

Transaction::Transaction(const std::string& transactionID, const time_t& timestamp, const std::vector<TxInput>& inputs, const std::vector<TxOutput>& outputs)
	: _transactionID(transactionID)
		, _timestamp(timestamp)
		, _inputs(std::move(inputs))
		, _outputs(std::move(outputs))
{
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

void Transaction::setTimestamp(time_t timestamp)
{
	_timestamp = timestamp;
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
		tax += output.getValue(); //Tax rate
	}
	return tax * TAX_RATE;
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
	for (auto& input : _inputs)
	{
		input.setScriptSig("");
	}


	// Derive public key from private key
	Point* pointPublicKey = keyGenerator.ECMul(ECDSASigner::hexStringToCppInt(privateKey), keyGenerator.GPoint);
	//std::cout << "[Sign] Public key: " << pointPublicKey->_x << "|" << pointPublicKey->_y << std::endl;
	//std::string publicKey = pointPublicKey->ToStringNoSeperator();
	std::string publicKey = Point::usePointToHex(pointPublicKey); // --Try

	// Sign transaction
	Point* pointSignature = signer.signMessage(ECDSASigner::hexStringToCppInt(privateKey), transactionMessageToSign());
	//std::cout << "[Sign] Signature: " << pointSignature->_x << "|" << pointSignature->_y << std::endl;
	//std::string signature = pointSignature->ToStringNoSeperator();
	std::string signature = Point::usePointToHex(pointSignature); // --Try

	// Script signature = <signature + public key>
	std::string scriptSig = signature + " " + publicKey;

	//std::cout << "[Sign] Script sig: " << scriptSig << std::endl;
	// Set script signature of all transaction inputs 
	for (auto& input : _inputs)
	{
		if (input.getScriptSig() == "") {
			input.setScriptSig(scriptSig);
		}
	}

	delete pointPublicKey;
	delete pointSignature;
}

bool Transaction::verifyTransactionSignature(const UTXOSet& utxoset/*const std::string& scriptPubKey*/) const
{
	///////////////////////////////////////////////////////////////
		// 1. Check if the transaction was constructed properly. If there is no ID can't verify
	if (!isTransactionCreationSucceeded() || _transactionID.empty()) 
	{
		return false;
	}

	uint64_t totalInputValue = 0;
	uint64_t totalOutputValue = 0;

	// Prepare the transaction message for signature verification.
	std::string txMessage = transactionMessageToSign();


	bool isCoinbase = false;

	// 2. Verify each input.
	for (const auto& input : _inputs) 
	{
		// Parse scriptSig (assume it's "signature publicKey")
		std::istringstream iss(input.getScriptSig());
		std::string signatureHex, publicKeyHex;
		if (!(iss >> signatureHex >> publicKeyHex))
		{
			std::cerr << "Invalid scriptSig format." << std::endl;
			return false;
		}

		//std::cout << "[Verify] Public key str: " << publicKeyHex << std::endl;
		//std::cout << "[Verify] Signature str: " << signatureHex << std::endl;

		if (publicKeyHex == "Coinbase")
		{
			//If this transaction is initialized by Coinbase - no point in verifying it.
			isCoinbase = true;
			continue; 
		}

		
		OutPoint op = input.getPreviousOutPoint();

		// Check UTXO existence.
		if (!utxoset.hasUTXO(op))
		{
			std::cerr << "UTXO not found for " << op << std::endl;
			return false;
		}
		
		// Add the value of the input to the total value
		UTXOData referencedOutput = utxoset.getUTXOData(op);
		totalInputValue += referencedOutput.getValue();



		// Verify the public key hash.
		std::string computedPubKeyHash = Transaction::hashPublicKey(publicKeyHex);
		std::string expectedPubKeyHash = Transaction::extractPublicKeyHash(referencedOutput.getScriptPubKey());
		if (computedPubKeyHash != expectedPubKeyHash) 
		{
			std::cerr << "Public key hash mismatch." << std::endl;
			return false;
		}

		// ECDSA verify:
		//    Rebuild signature Point from signatureHex
		//Point* rs = Point::parseHexStringNoSeperator(signatureHex);
		//Point* rs = Point::reverseToStringNoSeperator(signatureHex);
		Point* rs = Point::useHexToPoint(signatureHex); // --Try
		if (!rs) {
			std::cerr << "Invalid signature format in input " << "\n";
			return false;
		}

		//    Rebuild publicKey Point from pubKeyHex
		//Point* pubKeyPoint = Point::parseHexStringNoSeperator(publicKeyHex);
		//Point* pubKeyPoint = Point::reverseToStringNoSeperator(publicKeyHex);
		Point* pubKeyPoint = Point::useHexToPoint(publicKeyHex); // --Try
		//Point* pubKeyPoint = Point::parseHexStringPoint(publicKeyHex);
		if (!pubKeyPoint) 
		{
			std::cerr << "Invalid public key format in input " << "\n";
			delete rs;
			return false;
		}

		//std::cout << "[Verify] Public key: " << pubKeyPoint->_x << ":" << pubKeyPoint->_y << std::endl;
		//std::cout << "[Verify] Signature: " << rs->_x << ":" << rs->_y << std::endl;

		//    Verify ECDSA
		ECDSASigner signer;
		bool verified = signer.verifySignature(rs, SHA256::digest(transactionMessageToSign()), pubKeyPoint);

		// Cleanup
		delete rs;
		delete pubKeyPoint;

		if (!verified)
		{
			std::cerr << "Signature verification failed in input " << "\n";
			return false;
		}

	}

	// 3. Check output values.
	for (const auto& output : _outputs)
	{
		totalOutputValue += output.getValue();
	}

	// Optionally, include fee or tax check.
	if (!isCoinbase && totalInputValue < totalOutputValue/* + tx.calculateTax()*/) 
	{
		std::cerr << "Input value less than output value." << std::endl;
		return false;
	}

	// 4. (Outside this function) Make sure that the UTXOs are updated to prevent double spending.

	return true;
	
	
	///////////////////////////////////////////////////////////////









	//// If there is no ID can't verify
	//if (_transactionID.empty()) 
	//{
	//	return false;
	//}

	//// For each input, parse the signature + pubKey, retrieve the *previous* output’s scriptPubKey,
	//// 
	//// check that the hashed pubKey matches that scriptPubKey’s <pubKeyHash>, 
	//// 
	//// then run ECDSA verification on the transaction ID (or partial-hash).
	//for (size_t i = 0; i < _inputs.size(); i++)
	//{
	//	const TxInput& input = _inputs[i];

	//	// Parse scriptSig => <signatureHex> + <publicKeyHex>
	//	std::istringstream iss(input.getScriptSig());
	//	std::string signatureHex;
	//	std::string pubKeyHex;
	//	if (!(iss >> signatureHex >> pubKeyHex)) 
	//	{
	//		std::cerr << "Invalid scriptSig format in input " << i << "\n";
	//		return false;
	//	}

	//	if (pubKeyHex == "Coinbase") {
	//		continue; //If this transaction is initialized by Coinbase - no point in verifying it.
	//	}

	//	// ECDSA verify:
	//	//    Rebuild signature Point from signatureHex
	//	Point* rs = Point::parseHexString(signatureHex);
	//	if (!rs) {
	//		std::cerr << "Invalid signature format in input " << i << "\n";
	//		return false;
	//	}

	//	//    Rebuild publicKey Point from pubKeyHex
	//	Point* pubKeyPoint = Point::parseHexString(pubKeyHex);
	//	if (!pubKeyPoint) {
	//		std::cerr << "Invalid public key format in input " << i << "\n";
	//		delete rs;
	//		return false;
	//	}

	//	//    Verify ECDSA
	//	ECDSASigner signer;
	//	bool verified = signer.verifySignature(rs, transactionMessageToSign(), pubKeyPoint);

	//	// Cleanup
	//	delete rs;
	//	delete pubKeyPoint;

	//	if (!verified) 
	//	{
	//		std::cerr << "Signature verification failed in input " << i << "\n";
	//		return false;
	//	}
	//}

	//// All inputs validated
	//return true;
}


void Transaction::displayTransaction() const
{
	std::cout << this->toString();
}

std::string Transaction::toString() const
{
	std::string time = HelperT::timeToStr(_timestamp);

	// A plain text representation
	std::ostringstream oss;
	oss << "Transaction:\n";
	oss << "  ID: " << _transactionID << "\n";
	oss << "  Timestamp: " << time << "\n";
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

json Transaction::toJson() const
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

	// Return JSON
	return j;
}

std::string Transaction::toMessageString() const
{
	// Transaction = txid$timestamp$inputs$outputs
	// inputs = i*previousPointTxID*previousPointIndex*scriptSig^...
	// outputs = i*value*scriptPubKey^...
	std::string inputs = "";
	for (size_t i = 0; i < _inputs.size(); ++i)
	{
		const TxInput& in = _inputs[i];
		inputs += i + "*" + in.getPreviousOutPoint().getTxID() + "*"
			+ std::to_string(in.getPreviousOutPoint().getIndex()) + "*"
			+ in.getScriptSig() + "^";
	}
	inputs.pop_back();

	std::string outputs = "";
	for (size_t i = 0; i < _outputs.size(); ++i)
	{
		const TxOutput& out = _outputs[i];

		outputs += i + "*" + std::to_string(out.getValue()) + "*" + out.getScriptPubKey() + "^";
	}
	outputs.pop_back();

	std::string transaction = _transactionID + "$" + std::to_string(_timestamp) + "$"
		+ inputs + "$" + outputs;

	return transaction;
}

Transaction Transaction::parseMessageString(const std::string& data)
{
	// The overall delimiter between the main fields is '$'.
	// Expected format: transactionID$timestamp$inputs$outputs
	std::vector<std::string> parts = HelperT::split(data, '$');
	if (parts.size() != 4) 
	{
		// Invalid input format in transaction
		return Transaction();
	}

	// Parse transactionID and timestamp. Use std::stoll and cast to time_t.
	std::string transactionID = parts[0];
	time_t timestamp = static_cast<time_t>(std::stoll(parts[1]));

	std::vector<TxInput> inputs;
	std::vector<TxOutput> outputs;

	// Parse inputs (each input is separated by '^')
	// Each input has the format: index*prevTxID*prevIndex*scriptSig
	if (!parts[2].empty()) {
		std::vector<std::string> inputsStr = HelperT::split(parts[2], '^');
		for (const auto& inputToken : inputsStr) 
		{
			std::vector<std::string> inputParts = HelperT::split(inputToken, '*');
			if (inputParts.size() != 4) 
			{
				// Invalid input format in transaction
				return Transaction();
			}

			// The first field (the index) is redundant and can be ignored.
			const std::string& prevTxID = inputParts[1];
			unsigned int prevIndex = static_cast<unsigned int>(std::stoul(inputParts[2]));
			const std::string& scriptSig = inputParts[3];

			OutPoint outPoint(prevTxID, prevIndex);
			TxInput txInput(outPoint, scriptSig);
			inputs.push_back(txInput);
		}
	}

	// Parse outputs (each output is separated by '^')
	// Each output has the format: index*value*scriptPubKey
	if (!parts[3].empty()) {
		std::vector<std::string> outputsStr = HelperT::split(parts[3], '^');
		for (const auto& outputToken : outputsStr) 
		{
			std::vector<std::string> outputParts = HelperT::split(outputToken, '*');
			if (outputParts.size() != 3) 
			{
				// Invalid input format in transaction
				return Transaction();
			}

			// The first field (the index) is redundant and can be ignored.
			uint64_t value = std::stoull(outputParts[1]);
			const std::string& scriptPubKey = outputParts[2];

			TxOutput txOutput(value, scriptPubKey);
			outputs.push_back(txOutput);
		}
	}

	// Construct and return the Transaction object
	return Transaction(transactionID, timestamp, inputs, outputs);
}

Transaction Transaction::fromJson(json jsonTx)
{
	// Parse the incoming string as JSON
	json j = jsonTx;

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

	//  Hash transaction ID
	std::string sha = SHA256::digest(transactionID);

	// Clear memory used
	delete[] buffer;
	delete[] hashedID;

	return sha;
}

void Transaction::refreshTransactionID()
{
	_transactionID = generateTransactionID();
}

bool Transaction::operator==(const Transaction& other) const
{
	return _transactionID == other._transactionID
		&& _timestamp == other._timestamp
		&& _inputs == other._inputs
		&& _outputs == other._outputs;
}

std::string Transaction::hashPublicKey(const std::string& hexPubKey)
{
	return RIPEMD_160::hash(SHA256::digest(hexPubKey));
}

std::string Transaction::extractPublicKeyHash(const std::string& scriptPubKey)
{
	//scriptPubKey = <type (1 byte)><public key hash (20 bytes)>
	return scriptPubKey.substr(2);
}

std::string Transaction::extractTransactionType(const std::string& scriptPubKey)
{
	// First byte of scriptPubKey is type
	return scriptPubKey.substr(0, 2);
}

bool Transaction::isTransactionCreationSucceeded() const
{
	if (_inputs.empty() && _outputs.empty())
	{
		return false;
	}
	return true;
}

std::string Transaction::transactionMessageToSign() const
{
	Transaction copyTx(*this);
	// Set script signature of all transaction inputs to empty
	for (auto& input : copyTx._inputs)
	{
		input.setScriptSig("");
	}

	return SHA256::digest(copyTx.toString());
}

