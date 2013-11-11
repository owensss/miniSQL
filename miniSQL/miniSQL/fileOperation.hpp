#pragma once

/** get all the names of the directory
  */
#include <vector>
/* get file names under some directory
 * need user append \\ in the directory
 */
extern const std::vector<std::string> getFileNames(const char* directory);

/* get fileNames with that prefix
 */
inline const std::vector<std::string> getFileNamesWithPrefix(const char* prefix){ return getFileNames(prefix); }