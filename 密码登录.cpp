// g++ -static -o �����¼ �����¼.cpp

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <filesystem>
#include <cstdio>
#include <windows.h>
#include <sstream>

using namespace std;

void removeFileIfExist(const char *fileName)
{
    remove(fileName);
}

void removeDirectoryIfExist(const string &dirName)
{
    if (filesystem::exists(dirName)) // ���Ŀ¼�Ƿ����
    {
        filesystem::remove_all(dirName); // ɾ��Ŀ¼������������
    }
    else
    {
        // ˵������������⣬ֱ���˳�
        exit(0);
    }
}

const string SESSION_TOKEN_FILENAME = "session.token";
const string TOKEN_FILENAME = "token";

int main()
{
    // �����ļ�
    filesystem::remove("config.yml");
    filesystem::copy_file("default/config.yml", "config.yml", filesystem::copy_options::overwrite_existing);
    // ��Ⲣɾ�� session.token �� token
    removeFileIfExist("session.token");
    removeFileIfExist("token");

    // ���ļ�
    ifstream inFile("config.yml");
    ofstream outFile("temp.yml");

    // ���ж�ȡ�ļ�
    string line;
    string qqNumber;
    while (getline(inFile, line))
    {
        // ��鵱ǰ���Ƿ���� uin �� password
        if (line.find("uin:") != string::npos)
        {
            // ��ʾ�û����� uin
            cout << "������QQ�˺�: ";
            getline(cin, line);
            qqNumber = line;
            line = "  uin: " + line;
        }
        else if (line.find("password:") != string::npos)
        {
            // ��ʾ�û����� password
            cout << "����������: ";
            getline(cin, line);
            line = "  password: '" + line + "'";
        }

        // д����º����
        outFile << line << endl;
    }

    // �ر��ļ�
    inFile.close();
    outFile.close();

    // ɾ��ԭ�е�config����temp�滻ԭʼ�ļ�
    remove("config.yml");
    rename("temp.yml", "config.yml");

    // ɾ���ɵ�qq���ļ���
    removeDirectoryIfExist(qqNumber);

    // �ȴ�һ��
    this_thread::sleep_for(chrono::seconds(1));

    // ���� go-cqhttp.exe ����
    string program = ".\\go-cqhttp.exe"; // ����·��
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcess(nullptr, &program[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success)
    {
        cerr << "��������ʧ�ܣ�����ϵ���߷���" << endl;
        exit(0);
    }

    // ÿ��һ���Ӽ�� session.token �ļ��Ƿ����
    while (true)
    {
        if (filesystem::exists(SESSION_TOKEN_FILENAME))
        {
            // �ر�go-cqhttp����
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // ���� go-cqhttpתoicq.exe ����
            cout << "session.token���ɳɹ�\n";
            system("start go-cqhttpתoicq.exe");
            break;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    // ÿ��һ����TOKEN_FILENAME�Ƿ���ڣ�������ھͼ�������
    while (true)
    {
        if (filesystem::exists(TOKEN_FILENAME))
        {
            // ����ҵ��� token �ļ����ͼ�������
            cout << "token���ɳɹ�\n";
            break;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    // �����ļ��в������ļ�
    string folderName = qqNumber;
    filesystem::create_directory(folderName);

    filesystem::rename("device.json", "device-" + qqNumber + ".json");
    filesystem::copy(TOKEN_FILENAME, folderName + "\\" + TOKEN_FILENAME);
    filesystem::copy("device-" + qqNumber + ".json", folderName + "\\device-" + qqNumber + ".json");
    // �ȴ�һ��
    this_thread::sleep_for(chrono::seconds(1));
    filesystem::remove("device-" + qqNumber + ".json");

    // �����ɫ���嵽����̨
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    // �����ʾ��Ϣ
    cout << "�������н�������ѱ�Ŀ¼�µ�QQ���ļ��и��Ƶ� Yunzai-bot/data �ļ����ڣ����ǵ�ԭ�е��ļ��У�Ȼ��node app�����������̼���\n";

    // �ָ�����̨Ĭ����ɫ
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    system("pause");

    return 0;
}
