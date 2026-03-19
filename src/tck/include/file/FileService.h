// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_FILE_SERVICE_H_
#define HIERO_TCK_CPP_FILE_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::FileService
{
/**
 * Forward declarations.
 */
struct AppendFileParams;
struct CreateFileParams;
struct DeleteFileParams;
struct GetFileContentsParams;
struct GetFileInfoParams;
struct UpdateFileParams;

/**
 * The parameters to use to append a file.
 *
 * @param params The parameters use to append a file.
 * @return A JSON response containing the status of the appended file.
 */
nlohmann::json appendFile(const AppendFileParams& params);

/**
 * Create a file.
 *
 * @param params The parameters to use to create a file.
 * @return A JSON response containing the created file ID and the status of the file creation.
 */
nlohmann::json createFile(const CreateFileParams& params);

/**
 * Delete a file.
 *
 * @param params The parameters to use to delete a file.
 * @return A JSON response containing the status of the file deletion.
 */
nlohmann::json deleteFile(const DeleteFileParams& params);

/**
 * Get file contents.
 *
 * @param params The parameters to use to get the content of file.
 * @return A JSON response containing the file contents.
 */
nlohmann::json getFileContents(const GetFileContentsParams& params);

/**
 * Get file info.
 *
 * @param params The parameters to use to get file info.
 * @return A JSON response containing the file info.
 */
nlohmann::json getFileInfo(const GetFileInfoParams& params);

/**
 * Update a file.
 *
 * @param params The parameters to use to update a file.
 * @return A JSON response containing the status of the file update.
 */
nlohmann::json updateFile(const UpdateFileParams& params);

} // namespace Hiero::TCK::FileService

#endif // HIERO_TCK_CPP_FILE_SERVICE_H_
