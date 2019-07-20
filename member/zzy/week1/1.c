#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>

// 处理错误
void error_printf(const char* );

// 处理路径下的文件
void list_dir(const char* );

// 所显示的文件信息
void display_dir(DIR* );


// 处理错误
void error_printf(const char* funname)
{
    perror(funname);
    exit(EXIT_FAILURE);
}

// 读取路径下的文件
int a = 1;
void list_dir(const char* pathname)
{
    char nextpath[PATH_MAX+1];

    DIR* ret_opendir = opendir(pathname); // 打开目录"pathname"
    if(ret_opendir == NULL)
        error_printf("opendir");
    
    printf("%s:\n",pathname); // 显示pathname目录路径
    display_dir(ret_opendir); // 显示pathname目录下所有非隐藏文件名称
    //printf("\n%d %s\n",a++,);
    struct dirent* ret_readdir = NULL; // 定义readdir函数返回的结构体变量
    while((ret_readdir = readdir(ret_opendir))) // 判断是否读取到目录尾
    {
        char* filename = ret_readdir->d_name; // 获取文件名
        //printf("ssss");
        int end = 0; // 优化显示路径（处理"./test/"与"./test"）
        while(pathname[end])
            end++;
        strcpy(nextpath,pathname);
        if(pathname[end-1] != '/')
            strcat(nextpath,"/");
        strcat(nextpath,filename);

        struct stat file_message = {}; // 定义stat函数返回的结构体变量
        int ret_stat = lstat(nextpath, &file_message); // 获取文件信息
        if(ret_stat == -1) // stat读取文件错误则输出提示信息
            printf("%s error!", filename);
        else if(S_ISDIR(file_message.st_mode)  && filename[0]!='.') // 筛选"."、".."与隐藏文件
        {
            list_dir(nextpath);
        }
    }
    closedir(ret_opendir);
}

// 打印所读取文件的信息
void display_dir(DIR* ret_opendir)
{
    struct dirent* ret_readdir = NULL; // 定义readdir函数返回的结构体变量
    while((ret_readdir = readdir(ret_opendir))) // 判断是否读取到目录尾
    {
        char* filename = ret_readdir->d_name; // 获取文件名
        if(filename[0]!='.') // 不输出当前目录、上一级目录与隐藏文件 
            printf("%s\n",ret_readdir->d_name); // 打印文件名
    }
    rewinddir(ret_opendir); // 非常重要，将文件流的指针拨回起始位置
    puts("");
    puts("");
}

int main(int argc, const char** argv)
{
    char path[PATH_MAX+1] = {};

    if(argc == 2 && !(strcmp(argv[1],"-R"))) // 判断命令格式
        strcpy(path,".");
    else if(argc != 3)
    {
        printf("格式有误！ \n");
        exit(EXIT_FAILURE);
    }
    else
        strcpy(path,argv[2]);

    if(!(strcmp(argv[1],"-R")))
    {
        struct stat file_message = {};
        int ret_stat = lstat(path, &file_message);

        if(ret_stat == -1)
               error_printf("stat");

        if(S_ISDIR(file_message.st_mode)) // 判断是否为目录
            list_dir(path);
        else
        printf("It is not dir!");
    }
    else
    {
        printf("error in main!\n");
        exit(EXIT_FAILURE);
    }                    
    return 0;
}
