#include "pch.h"
#include "WebListener.h"

WebListener::WebListener() {

	CROW_ROUTE(app, "/login")([this](const crow::request& req) {
		auto query_params = crow::query_string(req.url_params);
		std::string seed = query_params.get("seed");

		// If seed has 12 words
		if (std::count_if(seed.begin(), seed.end(), [](char c) { return c == ' '; }) + 1 == 12 &&
			!node) {
			node = new P2PNode(seed);
			std::thread(&P2PNode::start, node, "127.0.0.1", 1234).detach();
			std::cout << "[Info] New node was initialized on port 1234!\n";
			crow::response res("success");
			res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
			res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
			res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
			return res;
		}

		crow::response res("failed");
		res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
		res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
		res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
		return res;
	});

	CROW_ROUTE(app, "/transaction")([this](const crow::request& req) {
		auto query_params = crow::query_string(req.url_params);
		std::string dst = query_params.get("dst");
		int amt = std::stoi(query_params.get("amt"));

		if (node && node->createTransaction(dst, amt)) {
			crow::response res("success");
			res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
			res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
			res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
			return res;
		}
		crow::response res("failed");
		res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
		res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
		res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
		return res;

		});

	CROW_ROUTE(app, "/blockchain")([this]() {
		if (node) {
			crow::response res(node->getBlockchain()->getChain());
			res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
			res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
			res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
			return res;
		}
		crow::response res("failed");
		res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
		res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
		res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
		return res;	});

	CROW_ROUTE(app, "/transactions")([this]() {
		if (!node) {
			crow::response res("failed");
			res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
			res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
			res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
			return res;
		}
		crow::response res(json(node->getUTXOs()).dump());
		res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
		res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
		res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
		return res;
		});

	CROW_ROUTE(app, "/balance") ([this]() {
		if (!node) {
			crow::response res("failed");
			res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
			res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
			res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
			return res;
		}
		crow::response res(std::to_string(node->getBalance()));
		res.add_header("Access-Control-Allow-Origin", "*"); // CORS header
		res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // Allowed methods
		res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization"); // Allowed headers
		return res;
		});
}

void WebListener::start(int port)
{
	app.port(port).multithreaded().run();
}
