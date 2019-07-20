#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <linux/limits.h>

#define PARAM_NONE 0
#define PARAM_A    1
#define PARAM_L    2
#define MAXROWLEN  80

int g_leave_len = MAXROWLEN;                                                                                   
int g_maxlen;  //存放某目录下最长文件名的长度

void my_err(char error[],int num){

	fprintf(stderr,"line :%d",num);
	perror(error);
}

void display_attribute(struct stat buf,char * name)
{
    char buf_time[32];
    struct passwd *psd;
    struct group *grp;

	if(S_ISREG(buf.st_mode))
   		printf("-");
   	else if(S_ISDIR(buf.st_mode))
   		printf("d");
	else if(S_ISCHR(buf.st_mode))
		printf("c");
	else if(S_ISBLK(buf.st_mode))
		printf("b");
	else if(S_ISFIFO(buf.st_mode))
		printf("f");
	else if(S_ISLNK(buf.st_mode))
		printf("l");
	else if(S_ISSOCK(buf.st_mode))
		printf("s");


      if(buf.st_mode & S_IRUSR){ //用户的权限
          printf("r");
      }else
        printf("-");

      if(buf.st_mode & S_IWUSR){
          printf("w");
      }
      else
        printf("-");
      if(buf.st_mode & S_IXUSR){
          printf("x");
      }
      else
        printf("-");

      if(buf.st_mode & S_IRGRP){ //用户组的权限
          printf("r");
      }
      else 
        printf("-");
      if(buf.st_mode & S_IWGRP){
          printf("w");
      }
      else
        printf("-");
      if(buf.st_mode & S_IXGRP){
          printf("x");
      }
      else
      	printf("-");


      if(buf.st_mode & S_IROTH){ //其他用户的权限
          printf("r");
      }
      else 
        printf("-");
      if(buf.st_mode & S_IWOTH){
          printf("w");
      }
      else
        printf("-");
      if(buf.st_mode & S_IXOTH){
          printf("x");
      }
      else
      	printf("-");

      printf("   ");

      psd=getpwuid(buf.st_uid);
      grp=getgrgid(buf.st_gid);
      printf("%4d ",buf.st_nlink);//打印文件连接数
      printf("%-8s",psd->pw_name);
      printf("%-8s",grp->gr_name);

      printf("%6d",buf.st_size);//打印文件大小

     strcpy(buf_time,ctime(&buf.st_mtime));  //文件的最后修改时间
      //ctime函数的作用为把时间转化为字符串
     buf_time[strlen(buf_time)-1]='\0';  //去掉换行符

      printf("  %s",buf_time);

}

/*在没有使用-l选项时
 */
void display_single(char *name){

    int len;
//如果本行不足打印一个文件名换行
    if(g_leave_len < g_maxlen){
        putchar('\n');
        g_leave_len = MAXROWLEN;   
    }

    len = strlen(name);
    len = g_maxlen - len;
    printf("-%s",name);
    for(int i = 0;i < len;i++){
        printf(" ");
    }
        printf(" ");
//下面的2指示空两格
    g_leave_len -= (g_maxlen + 2);

}


/*根据命令行参数和完整路径显示目标文件
 * 参数flag：命令行参数
 * 参数pathname:包含文件路径名*/

void display(int flag,char *pathname){

    int i,j;
    struct stat buf;
    char name[_PC_NAME_MAX+1];  //代表名称的最长值 不同系统可能不同

    for(i=0,j=0;i<strlen(pathname);i++)
    {
        if(pathname[i]=='/')  //目录之间的分隔符
        {
            j=0;
            continue;

        }
        name[j++]=pathname[i];
    }
    name[j]='\0';

    if(lstat(pathname,&buf) == -1){
        my_err("stat",__LINE__);
    }

    switch (flag)
    {
        case PARAM_NONE:
            if(name[0]!='.')    //一般情况不显示隐藏文件
            display_single(name);
            break;
        case PARAM_A:
            display_single(name);
            break;
        case PARAM_L:
            if(name[0]!='.')
            {
                display_attribute(buf,name);
                printf("  %s\n",name);
            }
            break;
        case PARAM_A+PARAM_L:
                display_attribute(buf,name);
                printf("  %s\n",name);
                break;
        default:
            break;
    }

}

void display_dir(int flag_param,char * path)
{
    DIR *dir;
    struct dirent *ptr;
    int count=0;
    char filename[256][256],temp[256];

    dir=opendir(path);
    if(dir==NULL)
    {
        my_err("opendir",__LINE__);
    }
    //统计出最长文件名长度和文件总数
    while((ptr=readdir(dir))!=NULL)
    {
        if(g_maxlen<strlen(ptr->d_name))
        {
            g_maxlen=strlen(ptr->d_name);
        }
        count++;
    }
    closedir(dir);
    if(count>256)
    {
        my_err("too many file under this dir!\n",__LINE__);
    }

    int i,j,len=strlen(path);
    
    //获取该目录下的所有文件名
    dir=opendir(path);

    for(int i=0;i<count;i++)
    {
        ptr=readdir(dir);
        if(ptr==NULL) my_err("readdir",__LINE__);
        
        strncpy(filename[i],path,len);
        filename[i][len]='\0';  //因为strcat的实现需要最后一位是‘\0’
        strcat(filename[i],ptr->d_name);
        filename[i][len+strlen(ptr->d_name)]='\0';
        /* display(flag_param,filename[i]); */
    }


    for(i = 0;i < count-1;i++){
        for(j = 0;j < count-1-i;j++){
            if(strcmp(filename[j],filename[j+1]) > 0){
                strcpy(temp,filename[j+1]);
                temp[strlen(filename[j+1])] = '\0';
                strcpy(filename[j+1],filename[j]);
                filename[j+1][strlen(filename[j])] = '\0';
                strcpy(filename[j],temp);
                filename[j][strlen(temp)] = '\0';
            }
        }
    }
    for(int i = 0;i < count;i++){
        display(flag_param,filename[i]);
    }

    closedir(dir);
    if(flag_param & PARAM_L == 0)  //没有l选项的时候打印一个换行符
        putchar('\n');
}

int main(int argc,char **argv){
      int i=0,j=0,k=0,num=0;                                                                                     
      char path[_PC_PATH_MAX+1];
      char param[40];//保存命令行参数
      int flag_param=0;//参数种类l、a，r
      struct stat buf;
      

        for(i=0;i<argc;i++){
            if(argv[i][0]=='-'){
                for(k=1;k<strlen(argv[i]);k++,j++)   //j是出现的参数总数
                    param[j]=argv[i][k];  //保存命令行参数
                    num++;  //保存 ’-‘ 的数量
            }
        }

        //只支持少量参数，其他参数无法使用，报错
        for(int i = 0;i < j;i++){
          if(param[i]=='a')
          {
              flag_param|=PARAM_A;
          }
          else if(param[i]=='l')
          {
              flag_param|=PARAM_L;
          }
          /* else if(param[i]=='R'){ */
          /*     flag_param|=PARAM_R; */
          /* } */
          else
          {
              printf("is a invaild param!\n");
              exit(1);
          }
        }
        
        i = 1;
        //如果输入没有目录，直接开始当前目录的
        if(num+1 == argc){
            path[0] = '.';
            path[1] = '/';
            path[2] = '\0';
            display_dir(flag_param,path);
            return 0;
        }

      while(i<argc){

          if(argv[i][0]=='-')
          {
              i++;
              continue;
          }
          else{
              //printf("%s\n",path);
             strcpy(path,argv[i]);
              if(stat(path,&buf)==-1)
              {
                  my_err("stat",__LINE__);
              }
              if(S_ISDIR(buf.st_mode))
              {
                 if(path[strlen(argv[i])-1]!='/')
                 {
                     path[strlen(argv[i])]='/';
                     path[strlen(argv[i])+1]='\0';//保证字符串末尾为空零 方便进行字符串操作
                 }
                 else{
                     path[strlen(argv[i])]='\0';
                  }
                  display_dir(flag_param,path);
              }
              else{
                  display(flag_param,path);//参数为一个文件
              }
              i++;
          }
     }

	return 0;
}
