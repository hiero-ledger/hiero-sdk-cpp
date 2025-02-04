// SPDX-License-Identifier: Apache-2.0
#include "NodeAddress.h"
#include "IPv4Address.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <iomanip>
#include <memory>
#include <sstream>

namespace Hiero
{
//-----
NodeAddress NodeAddress::fromProtobuf(const proto::NodeAddress& protoNodeAddress)
{
  NodeAddress outputNodeAddress;
  outputNodeAddress.mRSAPublicKey = protoNodeAddress.rsa_pubkey();
  outputNodeAddress.mNodeId = protoNodeAddress.nodeid();
  outputNodeAddress.mNodeAccountId = AccountId::fromProtobuf(protoNodeAddress.nodeaccountid());
  outputNodeAddress.mNodeCertHash = internal::Utilities::stringToByteVector(protoNodeAddress.nodecerthash());

  for (int i = 0; i < protoNodeAddress.serviceendpoint_size(); ++i)
  {
    outputNodeAddress.mEndpoints.push_back(Endpoint::fromProtobuf(protoNodeAddress.serviceendpoint(i)));
  }

  outputNodeAddress.mDescription = protoNodeAddress.description();

  return outputNodeAddress;
}

//-----
std::unique_ptr<proto::NodeAddress> NodeAddress::toProtobuf() const
{
  auto protoNodeAddress = std::make_unique<proto::NodeAddress>();
  protoNodeAddress->set_rsa_pubkey(mRSAPublicKey);
  protoNodeAddress->set_nodeid(mNodeId);
  protoNodeAddress->set_allocated_nodeaccountid(mNodeAccountId.toProtobuf().release());
  protoNodeAddress->set_nodecerthash(internal::Utilities::byteVectorToString(mNodeCertHash));

  for (const auto& endpoint : mEndpoints)
  {
    *protoNodeAddress->add_serviceendpoint() = *endpoint.toProtobuf();
  }

  protoNodeAddress->set_description(mDescription);

  return protoNodeAddress;
}

//-----
std::string NodeAddress::toString() const
{
  std::stringstream outputStream;

  int columnWidth = 20;
  outputStream << std::setw(columnWidth) << std::right << "NodeId: " << std::left << mNodeId << std::endl;
  outputStream << std::setw(columnWidth) << std::right << "AccountId: " << std::left << mNodeAccountId.toString()
               << std::endl;
  outputStream << std::setw(columnWidth) << std::right << "Description: " << std::left << mDescription << std::endl;
  outputStream << std::setw(columnWidth) << std::right << "RSA Public Key: " << std::left << mRSAPublicKey << std::endl;
  outputStream << std::setw(columnWidth) << std::right << "Certificate Hash: " << std::left
               << internal::HexConverter::bytesToHex(mNodeCertHash) << std::endl;
  outputStream << std::setw(columnWidth) << std::right << "Endpoints: ";

  if (size_t endpointCount = mEndpoints.size(); !endpointCount)
  {
    outputStream << "<None>";
  }
  else
  {
    int counter = 0;
    for (const auto& endpoint : mEndpoints)
    {
      if (counter == 0)
      {
        outputStream << endpoint.toString();
      }
      else
      {
        outputStream << std::setw(columnWidth) << endpoint.toString();
      }

      ++counter;

      if (counter != endpointCount)
      {
        outputStream << std::endl;
      }
    }
  }

  return outputStream.str();
}

//-----
NodeAddress& NodeAddress::setPublicKey(std::string_view publicKey)
{
  mRSAPublicKey = publicKey;
  return *this;
}

//-----
NodeAddress& NodeAddress::setNodeId(const int64_t& nodeId)
{
  mNodeId = nodeId;
  return *this;
}

//-----
NodeAddress& NodeAddress::setAccountId(const AccountId& accountId)
{
  mNodeAccountId = accountId;
  return *this;
}

//-----
NodeAddress& NodeAddress::setCertHash(std::string_view certHash)
{
  mNodeCertHash = internal::Utilities::stringToByteVector(certHash);
  return *this;
}

//-----
NodeAddress& NodeAddress::setCertHash(std::vector<std::byte> certHash)
{
  mNodeCertHash = std::move(certHash);
  return *this;
}

//-----
NodeAddress& NodeAddress::setEndpoints(const std::vector<Endpoint>& endpoints)
{
  mEndpoints = endpoints;
  return *this;
}

//-----
NodeAddress& NodeAddress::setDescription(std::string_view description)
{
  mDescription = description;
  return *this;
}

} // namespace Hiero
