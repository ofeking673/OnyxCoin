#include "WebListener.h"

WebListener::WebListener() {
	CROW_ROUTE(app, "/")([]() {
		crow::response res;
		res.code = 301;
		res.set_header("Location", "/login");
		return res;
		});

	CROW_ROUTE(app, "/login")([this](const crow::request& req) {
		auto query_params = crow::query_string(req.url_params);
		std::string seed = query_params.get("seed");

		// If seed has 12 words
		if (std::count_if(seed.begin(), seed.end(), [](char c) { return c == ' '; }) + 1 == 12 &&
			!node) {
			node = new P2PNode(seed);
			node->start("127.0.0.1", 1234);
			return crow::response("success");
		}

		return crow::response("failed");
	});

	CROW_ROUTE(app, "/transaction")([this](const crow::request& req) {
		auto query_params = crow::query_string(req.url_params);
		std::string dst = query_params.get("dst");
		int amt = std::stoi(query_params.get("amt"));

		if (!node && node->createTransaction(dst, amt)) {
			return crow::response("success");
		}
		return crow::response("failed");

		});

	CROW_ROUTE(app, "/blockchain")([this]() {
		if(!node)
			return crow::response(node->getBlockchain()->getChain());
		return crow::response("failed");
	});
}

void WebListener::start(int port)
{
	app.port(port).multithreaded().run();
}
