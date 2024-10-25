/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_TCK_SERVER_H_
#define HEDERA_TCK_CPP_TCK_SERVER_H_

#include <functional>
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

namespace Hedera::TCK
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
  [[maybe_unused]] constexpr static const int DEFAULT_HTTP_PORT = 80;

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
   * @param port The port on which to listen for JSON requests.
   */
  explicit TckServer(int port);

  /**
   * Add a JSON RPC method function.
   *
   * @param name The name of the function.
   * @param func The function to run.
   */
  void add(const std::string& name, const MethodHandle& func);

  /**
   * Add a JSON RPC notification function.
   *
   * @param name The name of the function.
   * @param func The function to run.
   */
  void add(const std::string& name, const NotificationHandle& func);

  /**
   * Start listening for HTTP requests. All JSON RPC functions should be added before this is called.
   */
  void startServer();

  /**
   * Create a method handle for the input method. The created method handles creating the method inputs from the
   * provided JSON.
   *
   * @tparam ParamsType The parameters to pass into to method.
   * @param  method     The method of which to get the handle.
   * @return The handle for the method.
   */
  template<typename ParamsType>
  MethodHandle getHandle(nlohmann::json (*method)(const ParamsType&));

  /**
   * Create a notification handle for the input notification. The created notification handles creating the notification
   * inputs from the provided JSON.
   *
   * @tparam ParamsType   The parameters to pass into to notification.
   * @param  notification The notification of which to get the handle.
   * @return The handle for the notification.
   */
  template<typename ParamsType>
  [[maybe_unused]] NotificationHandle getHandle(void (*notification)(const ParamsType&));

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
   * Handle a single JSON request.
   *
   * @param request The JSON request to handle.
   * @return The JSON response.
   */
  [[nodiscard]] nlohmann::json handleSingleRequest(const nlohmann::json& request);

  /**
   * Set up the handler for the HTTP server.
   */
  void setupHttpHandler();

  /**
   * Map of function names to their corresponding methods.
   */
  std::unordered_map<std::string, MethodHandle> mMethods;

  /**
   * Map of function names to their corresponding notifications.
   */
  std::unordered_map<std::string, NotificationHandle> mNotifications;

  /**
   * The HTTP server to use to receive JSON requests.
   */
  httplib::Server mHttpServer;

  /**
   * The port on which to listen for JSON requests.
   */
  int mPort = DEFAULT_HTTP_PORT;
};

} // namespace Hedera::TCK

#endif // HEDERA_TCK_CPP_TCK_SERVER_H_
