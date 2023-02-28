// g++ -static -o 密码登录 密码登录.cpp

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
    if (filesystem::exists(dirName)) // 检测目录是否存在
    {
        filesystem::remove_all(dirName); // 删除目录及其所有内容
    }
    else
    {
        // 说明程序出现问题，直接退出
        exit(0);
    }
}

const string SESSION_TOKEN_FILENAME = "session.token";
const string TOKEN_FILENAME = "token";

int main()
{
    // 重置文件
    filesystem::remove("config.yml");
    filesystem::copy_file("default/config.yml", "config.yml", filesystem::copy_options::overwrite_existing);
    // 检测并删除 session.token 和 token
    removeFileIfExist("session.token");
    removeFileIfExist("token");

    // 打开文件
    ifstream inFile("config.yml");
    ofstream outFile("temp.yml");

    // 按行读取文件
    string line;
    string qqNumber;
    while (getline(inFile, line))
    {
        // 检查当前行是否包含 uin 或 password
        if (line.find("uin:") != string::npos)
        {
            // 提示用户输入 uin
            cout << "请输入QQ账号: ";
            getline(cin, line);
            qqNumber = line;
            line = "  uin: " + line;
        }
        else if (line.find("password:") != string::npos)
        {
            // 提示用户输入 password
            cout << "请输入密码: ";
            getline(cin, line);
            line = "  password: '" + line + "'";
        }

        // 写入更新后的行
        outFile << line << endl;
    }

    // 关闭文件
    inFile.close();
    outFile.close();

    // 删除原有的config，用temp替换原始文件
    remove("config.yml");
    rename("temp.yml", "config.yml");

    // 删除旧的qq号文件夹
    removeDirectoryIfExist(qqNumber);

    // 等待一秒
    this_thread::sleep_for(chrono::seconds(1));

    // 启动 go-cqhttp.exe 程序
    string program = ".\\go-cqhttp.exe"; // 程序路径
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcess(nullptr, &program[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success)
    {
        cerr << "程序启动失败，请联系作者反馈" << endl;
        exit(0);
    }

    // 每隔一秒钟检查 session.token 文件是否存在
    while (true)
    {
        if (filesystem::exists(SESSION_TOKEN_FILENAME))
        {
            // 关闭go-cqhttp进程
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // 运行 go-cqhttp转oicq.exe 程序
            cout << "session.token生成成功\n";
            system("start go-cqhttp转oicq.exe");
            break;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 每隔一秒检查TOKEN_FILENAME是否存在，如果存在就继续程序
    while (true)
    {
        if (filesystem::exists(TOKEN_FILENAME))
        {
            // 如果找到了 token 文件，就继续程序
            cout << "token生成成功\n";
            break;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 创建文件夹并复制文件
    string folderName = qqNumber;
    filesystem::create_directory(folderName);

    filesystem::rename("device.json", "device-" + qqNumber + ".json");
    filesystem::copy(TOKEN_FILENAME, folderName + "\\" + TOKEN_FILENAME);
    filesystem::copy("device-" + qqNumber + ".json", folderName + "\\device-" + qqNumber + ".json");
    // 等待一秒
    this_thread::sleep_for(chrono::seconds(1));
    filesystem::remove("device-" + qqNumber + ".json");

    // 输出绿色字体到控制台
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    // 输出提示信息
    cout << "程序运行结束，请把本目录下的QQ号文件夹复制到 Yunzai-bot/data 文件夹内，覆盖掉原有的文件夹，然后node app正常启动云崽即可\n";

    // 恢复控制台默认颜色
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    system("pause");

    return 0;
}
