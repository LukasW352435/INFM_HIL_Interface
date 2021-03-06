/**
 * HIL - REST Dummy DuT
 * This is a sample component description for a REST server as dummy DuT
 *
 * The version of the OpenAPI document: 1.0.0
 * 
 *
 * NOTE: This class is auto generated by OpenAPI-Generator 5.2.1.
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */

/*
 * DefaultApi.h
 *
 * 
 */

#ifndef DefaultApi_H_
#define DefaultApi_H_


#include <memory>
#include <utility>

#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>

#include "Message.h"
#include <string>

namespace dummy_dut::rest::api {

using namespace dummy_dut::rest::model;

/// <summary>
/// Load messages to view received messages and send from DuT
/// </summary>
/// <remarks>
/// 
/// </remarks>
class  DefaultApiDashboardResource: public restbed::Resource
{
public:
    DefaultApiDashboardResource();
    virtual ~DefaultApiDashboardResource();
    void GET_method_handler(const std::shared_ptr<restbed::Session> session);
    void POST_method_handler(const std::shared_ptr<restbed::Session> session);

	void set_handler_GET(
		std::function<std::pair<int, std::string>(

		)> handler
	);

	void set_handler_POST(
		std::function<std::pair<int, std::string>(
			std::shared_ptr<Message> const &
		)> handler
	);

private:
	std::function<std::pair<int, std::string>(

	)> handler_GET_;

	std::function<std::pair<int, std::string>(
		std::shared_ptr<Message> const &
	)> handler_POST_;

    std::shared_ptr<Message> message{};
};

/// <summary>
/// Open message stream to receive new messages continuously
/// </summary>
/// <remarks>
/// 
/// </remarks>
class  DefaultApiDashboardMessagesResource: public restbed::Resource
{
public:
	DefaultApiDashboardMessagesResource();
    virtual ~DefaultApiDashboardMessagesResource();
    void GET_method_handler(const std::shared_ptr<restbed::Session> session);

    std::vector<std::shared_ptr<restbed::Session>> sessions;
};

/// <summary>
/// Send a message to the dummy DuT
/// </summary>
/// <remarks>
/// 
/// </remarks>
class  DefaultApiSendResource: public restbed::Resource
{
public:
	DefaultApiSendResource();
    virtual ~DefaultApiSendResource();
    void PUT_method_handler(const std::shared_ptr<restbed::Session> session);

	void set_handler_PUT(
		std::function<std::pair<int, std::string>(
			std::shared_ptr<Message> const &
		)> handler
	);


private:
	std::function<std::pair<int, std::string>(
		std::shared_ptr<Message> const &
	)> handler_PUT_;


	std::shared_ptr<Message> message{};
};

/// <summary>
/// Subscribe client to receive messages from dummy DuT
/// </summary>
/// <remarks>
/// Messages will be in the same format as for the /send endpoint (see &lt;a href&#x3D;\&quot;#model-Messages\&quot;&gt;Messages&lt;/a&gt;)
/// </remarks>
class  DefaultApiReadResource: public restbed::Resource
{
public:
	DefaultApiReadResource();
    virtual ~DefaultApiReadResource();
    void POST_method_handler(const std::shared_ptr<restbed::Session> session);
    void DELETE_method_handler(const std::shared_ptr<restbed::Session> session);

	void set_handler_POST(
		std::function<std::pair<int, std::string>(
			std::string const &
		)> handler
	);

	void set_handler_DELETE(
		std::function<std::pair<int, std::string>(
			std::string const &
		)> handler
	);

private:
	std::function<std::pair<int, std::string>(
		std::string const &
	)> handler_POST_;

	std::function<std::pair<int, std::string>(
		std::string const &
	)> handler_DELETE_;
};


///
/// <summary>
/// The restbed service to actually implement the REST server
/// </summary>
///
class  DefaultApi: public restbed::Service
{
public:
	DefaultApi();
	~DefaultApi();
	void startService(int const& port);
	void stopService();

protected:
	std::shared_ptr<DefaultApiDashboardResource> m_spDefaultApiDashboardResource;
	std::shared_ptr<DefaultApiDashboardMessagesResource> m_spDefaultApiDashboardMessagesResource;
	std::shared_ptr<DefaultApiSendResource> m_spDefaultApiSendResource;
	std::shared_ptr<DefaultApiReadResource> m_spDefaultApiReadClientUrlResource;
};


}

#endif /* DefaultApi_H_ */

