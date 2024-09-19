#include <string>
#include <sstream>
#include <iomanip>
#include <ios>
#include <stdexcept>
#include <limits.h>
#include <cstddef>
#include <filesystem>
#include <unistd.h>


class Authenticate {
private:
    std::string str_;

public:
    Authenticate();
    Authenticate(const Authenticate&other);
    ~Authenticate();
    Authenticate& operator=(const Authenticate&other);

    std::string hexaToString(const std::string& hex);
    std::string stringToHexa(const std::string& str);
    std::string getPathToExe();
    int createFilePassword();

    /************GETTERs & SETTERS*************/

    std::string getString();
    void setString();
};
