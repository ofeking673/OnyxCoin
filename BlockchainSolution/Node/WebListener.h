#pragma once
#include "pch.h"
#include "crow.h"

class WebListener
{
public:
	WebListener();
	void start(int port);
private:
	crow::SimpleApp app;
	P2PNode* node;
};

