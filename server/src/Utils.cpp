#include "../headers/Utils.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <linux/limits.h>
#include <openssl/md5.h>
#include <sodium/runtime.h>
#include <sstream>
#include <string>

Utils::Utils() {}

Utils::Utils(const Utils &other) { this->str_ = other.str_; }

Utils::~Utils() {}

Utils &Utils::operator=(const Utils &other) {
  if (this != &other) {
    *this = other;
  }
  return *this;
}

std::string Utils::hexaToString(const std::string &hex) {
  std::stringstream ss;
  std::string output;

  if (hex.length() % 2 != 0) {
    throw std::invalid_argument("Hex string length must be even");
  }

  for (size_t i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
    output.push_back(byte);
  }
  return output;
}

std::string Utils::stringToHexa(const std::string &str) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  for (char c : str) {
    ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
  }
  return ss.str();
}

std::string Utils::getPathToExe() {
  char *buffer = new char[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX);
  if (len > 0) {
    buffer[len] = '\0';
    return std::string(buffer, len + 1);
  }
  return std::strerror(errno);
}

int Utils::createFilePassword() {
  std::string str = getPathToExe();
  std::filesystem::path exePath = str;
  std::filesystem::path pathProject = exePath.parent_path();
  std::filesystem::path targetDir = pathProject / "server";
  std::filesystem::path fileToCreate =
      targetDir / "actually_safe_this_time.txt";

  if (!std::filesystem::exists(fileToCreate)) {
    std::ofstream ofs(fileToCreate);
    ofs.close();
    return 1;
  } else {
    std::cout << "File already exists no need to create it" << std::endl;
    return -1;
  }
}

std::string Utils::md5HashPassword(std::string password) {
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5((unsigned char *)password.c_str(), password.size(),
      (unsigned char *)digest);
  std::ostringstream md5string;

  for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
    md5string << std::hex << std::setw(2) << std::setfill('0')
              << (int)digest[i];
  }
  return md5string.str();
}
