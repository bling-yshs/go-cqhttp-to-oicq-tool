// g++ -static -o 扫码登录 扫码登录.cpp

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
    if (std::filesystem::exists(dirName)) // 检测目录是否存在
    {
        std::filesystem::remove_all(dirName); // 删除目录及其所有内容
    }
}

const std::string SESSION_TOKEN_FILENAME = "session.token";
const std::string TOKEN_FILENAME = "token";

int main()
{
    // 复制config.yml到当前目录，因为是扫码登录，需要使用默认的config.yml
    std::filesystem::remove("config.yml");
    std::filesystem::copy_file("default/config.yml", "config.yml", std::filesystem::copy_options::overwrite_existing);

    // 检测并删除 session.token 和 token
    removeFileIfExist("session.token");
    removeFileIfExist("token");

    // 获取用户输入的 qq 号码
    std::string qqNumber;
    std::cout << "请输入你的QQ号: ";
    std::cin >> qqNumber;

    removeDirectoryIfExist(qqNumber);

    // 等待一秒
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 启动 go-cqhttp.exe 程序
    std::string program = ".\\go-cqhttp.exe"; // 程序路径
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcess(nullptr, &program[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success)
    {
        std::cerr << "程序启动失败，请联系作者反馈" << std::endl;
        return 1;
    }

    // 每隔一秒钟检查 session.token 文件是否存在
    while (true)
    {
        std::ifstream file(SESSION_TOKEN_FILENAME);
        if (file.good())
        {
            // 关闭进程
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // 如果找到了 session.token 文件，就运行 go-cqhttp转oicq.exe 程序
            std::cout << "session.token生成成功\n";
            std::system("start go-cqhttp转oicq.exe");
            file.close();
            break;
        }
        file.close();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 每隔一秒检查TOKEN_FILENAME是否存在，如果存在就继续程序
    while (true)
    {
        std::ifstream file(TOKEN_FILENAME);
        if (file.good())
        {
            // 如果找到了 token 文件，就继续程序
            std::cout << "token生成成功\n";
            file.close();
            break;
        }
        file.close();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 创建文件夹并复制文件
    std::string folderName = qqNumber;
    std::filesystem::create_directory(folderName);

    std::filesystem::rename("device.json", "device-" + qqNumber + ".json");
    std::filesystem::copy(TOKEN_FILENAME, folderName + "\\" + TOKEN_FILENAME);
    std::filesystem::copy("device-" + qqNumber + ".json", folderName + "\\device-" + qqNumber + ".json");
    // 等待一秒
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::filesystem::remove("device-" + qqNumber + ".json");

    // 输出绿色字体到控制台
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    // 输出提示信息
    std::cout << "程序运行结束，请把本目录下的QQ号文件夹复制到 Yunzai-bot/data 文件夹内，覆盖掉原有的文件夹，然后node app正常启动云崽即可\n";

    // 恢复控制台默认颜色
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    system("pause");

    return 0;
}
