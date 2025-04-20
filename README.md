# C++ Blockchain with PBFT & UTXO

A lightweight blockchain implementation in modern C++, built from scratch with a focus on:

-  PBFT (Practical Byzantine Fault Tolerance) consensus
-  UTXO (Unspent Transaction Output) transaction model
-  Digital signatures and wallet management
-  Peer-to-peer communication
-  Cryptography and encryption

## Usage
This project is multiprocessed, as it needs multiple clients to function.
```bash
DiscoveryServer.exe
node.exe <Path to Private Key> port
# For GUI clients, Check out the GUI folder, as it is built on a web application, using the CROW package for WebSockets
```

## Project Overview

This project showcases how to build a secure, decentralized blockchain network using:
- Custom cryptographic key pairs (ECDSA)
- PBFT consensus for node agreement
- UTXO system for managing balances
- A minimal peer-to-peer network architecture



### Authors
Ofek Dory, Nesli Ozer

As a part of our final project at the Magshimim program, we wanted to break the myth about cryptography being broad and boring, so we chose this unpopular project idea.
We think that this project turned out great, and was very important for our education about cryptography and security, as well as blockchain technology
