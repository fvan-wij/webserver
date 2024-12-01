#pragma once

#include <string>
#include <memory>
#include "HttpRequest.hpp"
#include "RequestHandler.hpp"
#include "GetRequestHandler.hpp"
#include "PostRequestHandler.hpp"
#include "DeleteRequestHandler.hpp"
#include "BadRequestHandler.hpp"

class HandlerFactory
{
	public:
		static std::unique_ptr<RequestHandler> create_handler(RequestType type);
};

