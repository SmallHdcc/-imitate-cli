#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <dirent.h>

#define ASSERT(condition, message)                    \
    do                                                \
    {                                                 \
        if (!(condition))                             \
        {                                             \
            fprintf(stderr, "warning:%s\n", message); \
            exit(EXIT_FAILURE);                       \
        }                                             \
    } while (0)

void init_directory()
{
    if (access("shortcut", F_OK) == -1)
    {
        int flag = mkdir("shortcuts");
        assert(flag == 0);
        printf("初始化快捷方式文件夹成功！\n");
    }

    // 初始化json文件,来对应！
    // 如果不存在json文件,创建json文件
    if (access("config.json", F_OK) != -1)
    {
        return;
    }
    FILE *fp = fopen("config.json", "w");
    ASSERT(fp != NULL, "创建json文件失败!");
    // 向json文件中写入数据
    fputs("{\n", fp);
    fputs("\"shortcut\": [\n", fp);
    fputs("\"single_name_you_want_to_use\"\n", fp);
    fputs("]\n", fp);
    fputs("}", fp);
    // 关闭文件
    fclose(fp);
    printf("初始化json文件成功!\n");
}

char *process_data(char *filename)
{
    FILE *fp;
    char path[1035];
    char cmd[1024];      // 为了拼接
    char *output = NULL; // 返回值
    size_t output_len = 0;

    sprintf(cmd, "python main.py %s", filename);

    /* 打开Python脚本并读取其输出 */
    fp = popen(cmd, "r");
    assert(fp != NULL);

    /* 读取Python脚本的输出 */
    while (fgets(path, sizeof(path) - 1, fp) != NULL)
    {
        size_t path_len = strlen(path);
        output = realloc(output, output_len + path_len + 1); // +1 for the null-terminator
        memcpy(output + output_len, path, path_len + 1);     // +1 to copy the null-terminator
    }

    /* 关闭文件 */
    pclose(fp);
    if (output == NULL)
    {
        return filename;
    }
    return output;
}

void open_programmer(char *argv[])
{
    // json
    char *filename = process_data(argv[2]); // 遍历json文件，找到对应的程序
    char path[20];
    // 去除filename结尾换行
    filename[strlen(filename) - 1] = '\0';
    sprintf(path, "shortcut\\%s.lnk", filename);
    printf("%s\n", path);
    ASSERT(filename != NULL, "请输入要打开的程序！");
    if (access(path, F_OK) != -1)
    {
        printf("成功打开文件.\n");
        ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
    }
    else
    {
        printf("打开程序失败！\n");
    }
}

int main(int argc, char *argv[])
{
    init_directory();

    assert(argc == 3 || argc == 2);

    ASSERT(argv[1][0] == 'o' || argv[1][0] == 'h', "参数错误！");
    if (argv[1][0] == 'h')
    {
        printf("Usage: open [option] [filename]\n");
        printf("option: -o open a shortcut\n");
        printf("        -h get help info\n");
        printf("filename: the name of the file\n");
        return 0;
    }
    open_programmer(argv);
    return 0;
}
