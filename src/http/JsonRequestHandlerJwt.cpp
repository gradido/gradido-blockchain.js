#include "gradido_blockchain/http/JsonRequestHandlerJwt.h"

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JWT/JWT.h"
#include "Poco/JWT/Signer.h"
#include "Poco/JWT/Token.h"

#include "rapidjson/writer.h"

using namespace rapidjson;

JsonRequestHandlerJwt::JsonRequestHandlerJwt(Poco::Net::IPAddress clientIp)
	: JsonRequestHandler(clientIp), mJWTToken(nullptr)
{

}

JsonRequestHandlerJwt::JsonRequestHandlerJwt()
	: mJWTToken(nullptr)
{

}

JsonRequestHandlerJwt::~JsonRequestHandlerJwt()
{
	if (mJWTToken) {
		delete mJWTToken;
		mJWTToken = nullptr;
	}
}

void JsonRequestHandlerJwt::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	auto method = request.getMethod();
	printf("[JsonRequestHandlerJwt::handleRequest] method: %s\n", method.data());
	if (method == "OPTIONS") {
		responseOptions(response);
		return;
	}
	auto authorization = request.find("Authorization");
	if (authorization == request.end()) {
		responseWithJson(stateError("Authorization not found"), request, response);
		return;
	}
	auto startBearer = authorization->second.find("Bearer");
	if (authorization->second.find("Bearer") == std::string::npos) {
		responseWithJson(stateError("Authorization invalid format"), request, response);
		return;
	}
	auto jwtTokenString = authorization->second.substr(startBearer + 7);

	mJWTToken = new Poco::JWT::Token(jwtTokenString);
	printf("jwtTokenString: %s\n", jwtTokenString.data());
	//Poco::JWT::Signer signer("0123456789ABCDEF0123456789ABCDEF");
	//Poco::JWT::Token token = signer.verify(jwtTokenString);
	JsonRequestHandler::handleRequest(request, response);
	
}