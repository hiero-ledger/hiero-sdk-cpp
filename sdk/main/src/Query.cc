/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2022 Hedera Hashgraph, LLC
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
#include "Query.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "ContractCallQuery.h"
#include "ContractFunctionResult.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"

namespace Hedera
{
/**
 * Explicit template instantiations.
 */
template class Query<AccountBalanceQuery, AccountBalance>;
template class Query<AccountRecordsQuery, AccountRecords>;
template class Query<ContractCallQuery, ContractFunctionResult>;
template class Query<TransactionReceiptQuery, TransactionReceipt>;
template class Query<TransactionRecordQuery, TransactionRecord>;

} // namespace Hedera