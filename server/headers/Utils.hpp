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
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
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
    std::string sha256HashPassword(int option, std::string str, std::string salt);
    std::string toBase64(const unsigned char* input, int length);
    std::string fromBase64(const std::string& input);

    /************GETTERs & SETTERS*************/

    std::string getString();
    void setString();
};
