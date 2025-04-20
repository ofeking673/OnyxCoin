#include "WebListener.h"

WebListener::WebListener(int port) {
	CROW_ROUTE(app, "/login")([](const crow::request& req) {
		auto query_params = crow::query_string(req.url_params);
		std::string seed = query_params.get("seed");


	});
}