#include <string>
#include <sstream>
#include <iomanip>
#include <ios>
#include <stdexcept>
#include <limits.h>
#include <cstddef>
#include <filesystem>
#include <unistd.h>
#include <sodium.h>
#include <openssl/md5.h>
#include <sys/types.h>


class Utils {
private:
    std::string str_;

public:
    Utils();
    Utils(const Utils&other);
    ~Utils();
    Utils& operator=(const Utils&other);

    std::string hexaToString(const std::string& hex);
    std::string stringToHexa(const std::string& str);
    std::string getPathToExe();
    int createFilePassword();
    std::string md5HashPassword(std::string str);

    /************GETTERs & SETTERS*************/

    std::string getString();
    void setString();
};
