// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_FILE_SERVICE_H_
#define HIERO_TCK_CPP_FILE_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::FileService
{
/**
 * Forward declarations.
 */
struct CreateFileParams;
struct UpdateFileParams;

/**
 * Create a file.
 *
 * @param params The parameters to use to create a file.
 * @return A JSON response containing the created file ID and the status of the file creation.
 */
nlohmann::json createFile(const CreateFileParams& params);

/**
 * Update a file.
 *
 * @param params The parameters to use to update a file.
 * @return A JSON response containing the status of the file update.
 */
nlohmann::json updateFile(const UpdateFileParams& params);

} // namespace Hiero::TCK::FileService

#endif // HIERO_TCK_CPP_FILE_SERVICE_H_