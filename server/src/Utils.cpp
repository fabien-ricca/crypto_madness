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

std::string Utils::sha256HashPassword(std::string password, char storeSalted[12]) {
    // 1. Générer un sel de 96 bits (12 octets)
    unsigned char salt[12]; // 96 bits = 12 octets
    if (storeSalted == nullptr){
        if (RAND_bytes(salt, sizeof(salt)) != 1) {
            throw std::runtime_error("Erreur lors de la génération du sel");
        }
    }
    else{
        memcpy(salt, storeSalted, sizeof(salt));
    }


    // 2. Calculer le hash SHA-256 du mot de passe concaténé avec le sel
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, salt, sizeof(salt));
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(digest, &sha256);

    // 3. Convertir le sel et le digest en base64
    std::string saltBase64 = toBase64(salt, sizeof(salt));
    std::string digestBase64 = toBase64(digest, SHA256_DIGEST_LENGTH);

    // 4. Concaténer le sel à gauche du digest (format "sel|digest")
    return saltBase64 + ":" + digestBase64;
}


std::string Utils::toBase64(const unsigned char* input, int length) {
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BUF_MEM *bptr;
    BIO_get_mem_ptr(b64, &bptr);

    std::string output(bptr->data, bptr->length);
    BIO_free_all(b64);
    return output;
}
