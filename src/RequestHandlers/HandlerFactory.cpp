#include "HandlerFactory.hpp"


std::unique_ptr<RequestHandler> HandlerFactory::create_handler(RequestType type)
{
	switch (type)
	{
		case RequestType::GetRequest:
			return std::make_unique<GetRequestHandler>();
		case RequestType::PostRequest:
			return std::make_unique<PostRequestHandler>();
		case RequestType::DeleteRequest:
			return std::make_unique<DeleteRequestHandler>();
		default:
			return std::make_unique<BadRequestHandler>();
	}
}

