#include "../headers/Authenticate.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <linux/limits.h>
#include <string>
#include <sys/types.h>

Authenticate::Authenticate() {

}

Authenticate::Authenticate(const Authenticate&other) {
    this->str_ = other.str_;
}

Authenticate::~Authenticate() {

}

Authenticate& Authenticate::operator=(const Authenticate&other) {
    if(this != &other)
    {
        *this = other;
    }
    return *this;
}

std::string Authenticate::hexaToString(const std::string& hex) {
    std::stringstream ss;
    std::string output;

    if(hex.length() %2 != 0) {
        throw std::invalid_argument("Hex string length must be even");
    }

    for(size_t i = 0; i < hex.length(); i+=2) {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
        output.push_back(byte);
    }
    return output;
}

std::string Authenticate::stringToHexa(const std::string& str) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for(char c: str) {
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}

std::string Authenticate::getPathToExe() {
    char *buffer = new char[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX);
    if(len > 0) {
        buffer[len] = '\0';
        return std::string(buffer, len+1);
    }
    return std::strerror(errno);
}

int Authenticate::createFilePassword() {
    std::string str = getPathToExe();
    std::filesystem::path exePath = str;
    std::filesystem::path pathProject = exePath.parent_path();
    std::filesystem::path targetDir = pathProject / "server";
    std::filesystem::path fileToCreate = targetDir / "actually_safe_this_time.txt";

    if(!std::filesystem::exists(fileToCreate)) {
        std::ofstream ofs(fileToCreate);
        ofs.close();
        return 1;
    }else {
        std::cout << "File already exists no need to create it" << std::endl;
        return -1;
    }
}
