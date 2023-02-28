// g++ -static -o ɨ���¼ ɨ���¼.cpp

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <filesystem>
#include <cstdio>
#include <windows.h>

void removeFileIfExist(const char *fileName)
{
    if (std::remove(fileName) == 0)
    {
    }
}
void removeDirectoryIfExist(const std::string &dirName)
{
    if (std::filesystem::exists(dirName)) // ���Ŀ¼�Ƿ����
    {
        std::filesystem::remove_all(dirName); // ɾ��Ŀ¼������������
    }
}

const std::string SESSION_TOKEN_FILENAME = "session.token";
const std::string TOKEN_FILENAME = "token";

int main()
{
    // ����config.yml����ǰĿ¼����Ϊ��ɨ���¼����Ҫʹ��Ĭ�ϵ�config.yml
    std::filesystem::remove("config.yml");
    std::filesystem::copy_file("default/config.yml", "config.yml", std::filesystem::copy_options::overwrite_existing);

    // ��Ⲣɾ�� session.token �� token
    removeFileIfExist("session.token");
    removeFileIfExist("token");

    // ��ȡ�û������ qq ����
    std::string qqNumber;
    std::cout << "���������QQ��: ";
    std::cin >> qqNumber;

    removeDirectoryIfExist(qqNumber);

    // �ȴ�һ��
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // ���� go-cqhttp.exe ����
    std::string program = ".\\go-cqhttp.exe"; // ����·��
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcess(nullptr, &program[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success)
    {
        std::cerr << "��������ʧ�ܣ�����ϵ���߷���" << std::endl;
        return 1;
    }

    // ÿ��һ���Ӽ�� session.token �ļ��Ƿ����
    while (true)
    {
        std::ifstream file(SESSION_TOKEN_FILENAME);
        if (file.good())
        {
            // �رս���
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // ����ҵ��� session.token �ļ��������� go-cqhttpתoicq.exe ����
            std::cout << "session.token���ɳɹ�\n";
            std::system("start go-cqhttpתoicq.exe");
            file.close();
            break;
        }
        file.close();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // ÿ��һ����TOKEN_FILENAME�Ƿ���ڣ�������ھͼ�������
    while (true)
    {
        std::ifstream file(TOKEN_FILENAME);
        if (file.good())
        {
            // ����ҵ��� token �ļ����ͼ�������
            std::cout << "token���ɳɹ�\n";
            file.close();
            break;
        }
        file.close();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // �����ļ��в������ļ�
    std::string folderName = qqNumber;
    std::filesystem::create_directory(folderName);

    std::filesystem::rename("device.json", "device-" + qqNumber + ".json");
    std::filesystem::copy(TOKEN_FILENAME, folderName + "\\" + TOKEN_FILENAME);
    std::filesystem::copy("device-" + qqNumber + ".json", folderName + "\\device-" + qqNumber + ".json");
    // �ȴ�һ��
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::filesystem::remove("device-" + qqNumber + ".json");

    // �����ɫ���嵽����̨
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    // �����ʾ��Ϣ
    std::cout << "�������н�������ѱ�Ŀ¼�µ�QQ���ļ��и��Ƶ� Yunzai-bot/data �ļ����ڣ����ǵ�ԭ�е��ļ��У�Ȼ��node app�����������̼���\n";

    // �ָ�����̨Ĭ����ɫ
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    system("pause");

    return 0;
}
