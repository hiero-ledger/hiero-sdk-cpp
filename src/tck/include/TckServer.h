// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TCK_SERVER_H_
#define HIERO_TCK_CPP_TCK_SERVER_H_

#include "json/JsonRpcParser.h"
#include <functional>
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace Hiero::TCK
{
/**
 * Class encompassing all HTTP and JSON processing of TCK requests.
 */
class TckServer
{
public:
  /**
   * The default port on which to listen for HTTP requests from the TCK.
   */
  [[maybe_unused]] constexpr static const int DEFAULT_HTTP_PORT = 8544;

  /**
   * Methods involve taking JSON parameters, doing a function, then returning the status of the execution in another
   * JSON object.
   */
  using MethodHandle = std::function<nlohmann::json(const nlohmann::json&)>;

  /**
   * Notifications involve taking JSON parameters, doing a function, and then reporting nothing back.
   */
  using NotificationHandle = std::function<void(const nlohmann::json&)>;

  /**
   * Construct a TckServer with the default HTTP port.
   */
  TckServer();

  /**
   * Construct a TckServer with a port on which to listen for JSON requests.
   *
   * @param port The port on which to listen.
   */
  explicit TckServer(int port);

  friend class TckServerUnitTests;

  /**
   * Wrapper function to get a MethodHandle from a function pointer.
   *
   * @tparam ParamsType The type of the parameters for the function.
   * @param method The function to wrap.
   * @return The wrapped function.
   */
  template<typename ParamsType>
  MethodHandle getHandle(nlohmann::json (*method)(const ParamsType&));

  /**
   * Wrapper function to get a NotificationHandle from a function pointer.
   *
   * @tparam ParamsType The type of the parameters for the function.
   * @param notification The function to wrap.
   * @return The wrapped function.
   */
  template<typename ParamsType>
  NotificationHandle getHandle(void (*notification)(const ParamsType&));

private:
  /**
   * Handle an HTTP request sent from a client.
   *
   * @param request  The HTTP request to process.
   * @param response The HTTP response to fill out.
   */
  void handleHttpRequest(const httplib::Request& request, httplib::Response& response);

  /**
   * Handle a JSON request sent from the JSON RPC client.
   *
   * @param request The JSON request to handle.
   * @return The response to the request.
   */
  std::string handleJsonRequest(const std::string& request);

  /**
   * Set up the handler for the HTTP server.
   */
  void setupHttpHandler();

  /**
   * The JSON-RPC Parser responsible for dispatching requests.
   */
  JsonRpcParser mJsonRpcParser;

  /**
   * The HTTP server to use for the TCK.
   */
  httplib::Server mServer;
};

} // namespace Hiero::TCK

#endif // HIERO_TCK_CPP_TCK_SERVER_H_
