// framework.h
// Main header for the Blockchain Project
// This file centralizes the includes for standard libraries,
// third-party dependencies, and internal modules.
// Modify and expand this file as your project evolves.

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

// ---------------------------------------------------------------------------
// Third-Party Library Includes
// (Uncomment and modify if using external libraries)
// ---------------------------------------------------------------------------
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>


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


// --- Wallet Management ---
 

// --- Peer-to-Peer Networking ---
 

// --- Consensus Algorithm (PBFT) ---


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
