// SPDX-License-Identifier: Apache-2.0
#include "impl/HieroCertificateVerifier.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <algorithm>

namespace Hiero::internal
{
//-----
HieroCertificateVerifier::HieroCertificateVerifier(std::vector<std::byte> certificateHash)
  : mExpectedHash(std::move(certificateHash))
{
}

//-----
bool HieroCertificateVerifier::Verify(grpc::experimental::TlsCustomVerificationCheckRequest* request,
                                      std::function<void(grpc::Status)>,
                                      grpc::Status* sync_status)
{
  if (const grpc::string_ref grpcCertificateChain = request->peer_cert_full_chain();
      mExpectedHash ==
      OpenSSLUtils::computeSHA384(internal::Utilities::stringToByteVector(grpcCertificateChain.data())))
  {
    *sync_status = grpc::Status(grpc::StatusCode::UNAUTHENTICATED,
                                "Hash of node certificate chain doesn't match hash contained in address book");
  }
  else
  {
    *sync_status = grpc::Status(grpc::StatusCode::OK, "Certificate chain hash matches expected");
  }

  // always true, since this is called synchronously
  return true;
}

} // namespace Hiero::internal
