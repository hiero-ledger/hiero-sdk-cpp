// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_HIERO_CERTIFICATE_VERIFIER_H_
#define HIERO_SDK_CPP_IMPL_HIERO_CERTIFICATE_VERIFIER_H_

#include <grpcpp/security/credentials.h>
#include <string>

namespace Hiero::internal
{
/**
 * Custom TLS verifier which checks if node TLS cert chain matches the expected hash
 */
class HieroCertificateVerifier : public grpc::experimental::ExternalCertificateVerifier
{
public:
  /**
   * Construct with a node certificate chain hash.
   *
   * @param certificateHash The claimed hash of the node certificate chain.
   */
  explicit HieroCertificateVerifier(std::vector<std::byte> certificateHash);

private:
  /**
   * The hash of the certificate chain for the node, from the address book.
   */
  std::vector<std::byte> mExpectedHash;

  /**
   * The verification logic that will be performed after the TLS handshake completes.
   *
   * @param request     The verification information associated with this request.
   * @param callback    Callback for asynchronous requests. This is unused since the SDK does all requests
   *                    synchronously.
   * @param sync_status Pointer to the gRPC status that should be updated to indicate the success or the failure of the
   *                    check.
   * @return \c TRUE because all checks are done synchronously.
   */
  bool Verify(grpc::experimental::TlsCustomVerificationCheckRequest* request,
              std::function<void(grpc::Status)> callback,
              grpc::Status* sync_status) override;

  /**
   * Cancels a verification request previously started via Verify(). Used when the connection attempt times out or is
   * cancelled while an async verification request is pending. The impl should abort whatever async operation it is
   * waiting for and quickly invoke the callback that was passed to Verify() with a status indicating the cancellation.
   *
   * @param request A pointer to the verification information associated with this request.
   */
  [[maybe_unused]] void Cancel(grpc::experimental::TlsCustomVerificationCheckRequest* request) override
  { // Intentionally unimplemented
  }
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_HIERO_CERTIFICATE_VERIFIER_H_
