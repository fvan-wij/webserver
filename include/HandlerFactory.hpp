#pragma once

#include <string>
#include <memory>
#include "Logger.hpp"
#include "RequestHandler.hpp"
#include "GetRequestHandler.hpp"
#include "PostRequestHandler.hpp"
#include "BadRequestHandler.hpp"
#include "meta.hpp"

class HandlerFactory {
	public:
		static std::unique_ptr<RequestHandler> create_handler(const std::string &method)
		{
			if (method == "GET")
				return std::make_unique<GetRequestHandler>();
			else if (method == "POST")
				return std::make_unique<PostRequestHandler>();
			else if (method == "DELETE")
				UNIMPLEMENTED("METHOD == DELETE");
			else
				return std::make_unique<BadRequestHandler>();
			return nullptr;
		}
};

