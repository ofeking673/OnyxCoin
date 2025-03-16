// framework.h
// Main header for the Blockchain Project
// This file centralizes the includes for standard libraries,
// third-party dependencies, and internal modules.

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

// ---------------------------------------------------------------------------
// Standard Library Includes
// ---------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <bitset>
#include <iomanip>
#include <ctime>
#include <random>
#include <array>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <map>
#include <chrono>
#include <functional>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <winsock2.h>
#include <Windows.h>
#include <exception>


// ---------------------------------------------------------------------------
// Third-Party Library Includes
// (Uncomment and modify if using external libraries)
// ---------------------------------------------------------------------------
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>

#include "Networking/json.hpp"
using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Project-Specific Modules
// ---------------------------------------------------------------------------

// --- Encryption & Cryptography ---
#include "Encryptions/AddressGenerator.h"
#include "Encryptions/AES256CBC.h"
#include "Encryptions/Argon2.h"
#include "Encryptions/Base58.h"
#include "Encryptions/BIP39SeedMaker.h"
#include "Encryptions/Blake2b.h"
#include "Encryptions/ECDSASigner.h"
#include "Encryptions/HelperT.h"
#include "Encryptions/KeyGenerator.h"
#include "Encryptions/Point.h"
#include "Encryptions/RIPEMD_160.h"
#include "Encryptions/SHA256.h"
#include "Encryptions/Test.h"


// --- Blockchain Core Structure ---
#include "Structure/Block.h"
#include "Structure/Blockchain.h"
#include "Structure/BlockHeader.h"

// --- Wallet Management ---
#include "Structure/Wallets/Mempool.h"
#include "Structure/Wallets/OutPoint.h"
#include "Structure/Wallets/OutPointHash.h"
#include "Structure/Wallets/TxInput.h"
#include "Structure/Wallets/TxOutput.h"
#include "Structure/Wallets/Transaction.h"
#include "Structure/Wallets/UTXOData.h"
#include "Structure/Wallets/UTXOSet.h"
#include "Structure/Wallets/Wallet.h"

// --- Peer-to-Peer Networking ---
#include "Networking/DiscoveryServer.h"
//#include "Networking/FullNode.h"
//#include "Networking/P2PNode.h"
#include "Networking/PeerInfo.h"
#include "Networking/WSAInitializer.h"

// --- Messages and Handling ---
//#include "Networking/Messages/FullNodeMessageHandler.h"
#include "Networking/Messages/IMessageHandler.h"
#include "Networking/Messages/InventoryData.h"
#include "Networking/Messages/MessageDispatcher.h"
//#include "Networking/Messages/MessageManager.h"
#include "Networking/Messages/MessageP2P.h"
#include "Networking/Messages/MessageParser.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/Messages/PhaseState.h"


// ---------------------------------------------------------------------------
// Utility Modules
// ---------------------------------------------------------------------------
// Uncomment or add additional utility headers as needed.
// #include "Utilities/Logger.h"      // Logging facilities
// #include "Utilities/Config.h"      // Configuration management

// ---------------------------------------------------------------------------
// Global Project Settings or Macros
// ---------------------------------------------------------------------------
#define BLOCKCHAIN_VERSION "1.0.0"
// Add more global definitions or macros here

#endif // FRAMEWORK_H
