#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#define PARAM_NONE 0
#define PARAM_A 1
#define PARAM_L 2
#define PARAM_R 4
#define MAXROWLEN 80
unsigned long int g_maxlen;
unsigned long int g_leave_len=MAXROWLEN;
void display_attribute(struct stat buf)
{
    char buf_time[32];
    struct group *grp;
    struct passwd *psd;
    if(S_ISLNK(buf.st_mode))
        printf("l");
    if(S_ISREG(buf.st_mode))
        printf("-");
    if(S_ISDIR(buf.st_mode))
        printf("d");
    if(S_ISCHR(buf.st_mode))
        printf("c");
    if(S_ISBLK(buf.st_mode))
        printf("b");
    if(S_ISFIFO(buf.st_mode))
        printf("f");
    if(S_ISSOCK(buf.st_mode))
        printf("s");
    if(buf.st_mode&S_IRUSR)
    {
        printf("r");
    }
    else
        printf("-");
    if(buf.st_mode&S_IWUSR)
    {
        printf("w");
    }
    else
        printf("-");
    if(buf.st_mode&S_IXUSR)
    {
        printf("x");
    }
    else
        printf("-");
    if(buf.st_mode&S_IRGRP)
    {
        printf("r");
    }
    else
        printf("-");
    if(buf.st_mode&S_IWGRP)
    {
        printf("r");
    }else
        printf("-");
    if(buf.st_mode&S_IXGRP)
    {
        printf("x");
    }else 
        printf("-");
    if(buf.st_mode&S_IROTH)
    {
        printf("r");
    }else
        printf("-");
    if(buf.st_mode&S_IWOTH)
    {
        printf("w");
    }else 
        printf("-");
    if(buf.st_mode&S_IXOTH)
    {
        printf("x");
    }else 
        printf("-");
    printf("   ");
    psd=getpwuid(buf.st_uid);
    grp=getgrgid(buf.st_gid);
    printf("%4ld",buf.st_nlink);
    printf("  %-8s",psd->pw_name);
    printf("  %-8s",grp->gr_name);
    printf("  %6ld",buf.st_size);
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time)-1]='\0';
    printf(" %s",buf_time);
}
void display_single(char *name)
{
    int i,len;
    if(g_leave_len<g_maxlen)
    {
        printf("\n");
        g_leave_len=MAXROWLEN;
    }
    len=strlen(name);
    len=g_maxlen-len;
    printf("%-s",name);
    for(i=0;i<len;i++)
    {
        printf(" ");
    }
    printf("  ");
    g_leave_len-=(g_maxlen+2);
}
void display(int flag,char *pathname)
{
    struct stat buf;
    int j=0;
    unsigned long int i;
    char name[NAME_MAX+1];
    for(i=0;i<strlen(pathname);i++)
    {
        if(pathname[i]=='/')
        {
            j=0;
            continue;
        }
        name[j++]=pathname[i];
    }
    name[j]='\0';
    //printf("^^^");printf("%8s",name);printf("^^");
    lstat(pathname,&buf);
    switch(flag)
    {
    case PARAM_NONE:
        break;
    case PARAM_A:
        display_single(name);
        break;
    case PARAM_L:
        if(name[0]!='.')
        {
            display_attribute(buf);
            printf("   %-s\n",name);
        }
        break;
    case PARAM_R:
        if(name[0]!='.')
            display_single(name);
        break;
    case PARAM_A+PARAM_R:
            display_single(name);
        break;
    case PARAM_L+PARAM_R:
        if(name[0]!='.')
        {
            display_attribute(buf);
            printf("  %-s\n",name);
        }
        break;
    case PARAM_A+PARAM_L:
        display_attribute(buf);
        printf("   %-s\n",name);
        break;
    case PARAM_A+PARAM_L+PARAM_R:
            display_attribute(buf);
            printf("  %-s\n",name);
        break;
    default:
        break;
    }
}
void display_digui(int flag_param,char *path)                      
{                                                                  
    int i,k=0;                                                     
    unsigned long j;
    int len=strlen(path);                                          
    struct dirent *ptr;                                            
    struct stat buf;                                               
    DIR *dir;                                                      
    char names[NAME_MAX+1];
    int count=0;                                                   
    char filenames[256][PATH_MAX+1];                               
    dir=opendir(path);                                             
    while((ptr=readdir(dir))!=NULL)                                
    {                                                              
        if(g_maxlen<strlen(ptr->d_name))                           
            g_maxlen=strlen(ptr->d_name);                          
        count++;                                                   
    }                                                              
    closedir(dir);                                                 
    dir=opendir(path);                                             
    for(i=0;i<count;i++)                                           
    {                                                              
        ptr=readdir(dir);                                          
        strncpy(filenames[i],path,len);                            
        filenames[i][len]='\0';                                    
        strcat(filenames[i],ptr->d_name);                      
        filenames[i][len+strlen(ptr->d_name)]='\0';            
    } 
    printf("\n%-s:\n",path);
    for(i=0;i<count;i++)                                           
    {                                                              
        display(flag_param,filenames[i]);                          
    }                                                              
    for(i=0;i<count;i++)
    {
        memset(names,0,sizeof(names));
        for(j=0;j<strlen(filenames[i]);j++)
        {
            if(filenames[i][j]=='/')
            {
                k=0;
                continue;
            }
            names[k++]=filenames[i][j];
        }
        names[k]='\0';//youwenti
        if(lstat(filenames[i],&buf)==-1)
        {
            perror("lstat:");
            exit(1);
        }
        if(S_ISDIR(buf.st_mode)&&names[0]!='.') 
        {
            for(j=0;j<strlen(filenames[i]);j++)
            {
                continue;
            }
            filenames[i][j]='/';
            filenames[i][j+1]='\0';
            if(names[k-1]!='/')
            {
                names[k]='/';
                names[k+1]='\0';
            }
            else
                names[k]='\0';
            display_digui(flag_param,filenames[i]);
        }
        else 
        {
            continue;
        }
    }
    //printf("$$$$\n");
    closedir(dir);                                                 
}                                                                  
void display_dir(int flag_param,char *path)
{
    int i,len=strlen(path);
    DIR *dir;
    struct dirent *ptr;
    int count=0;
    char filenames[256][PATH_MAX+1];
    dir=opendir(path);
    while((ptr=readdir(dir))!=NULL)
    {
        if(g_maxlen<strlen(ptr->d_name))
            g_maxlen=strlen(ptr->d_name);
        count++;
    }
    closedir(dir);
    dir=opendir(path);
    for(i=0;i<count;i++)
    {
        ptr=readdir(dir);
        strncpy(filenames[i],path,len);
        filenames[i][len]='\0';
        strcat(filenames[i],ptr->d_name);
        filenames[i][len+strlen(ptr->d_name)]='\0';
    }
    for(i=0;i<count;i++)
        display(flag_param,filenames[i]);
    closedir(dir);
}
int main(int argc,char *argv[])
{
    int i,j=0;
    int num=0;
    unsigned long k;
    char path[PATH_MAX+1];
    int flag_param=PARAM_NONE;
    char param[32];
    struct stat buf;
    for(i=0;i<argc;i++)
    {
        if(argv[i][0]=='-')
        {
            for(k=1;k<strlen(argv[i]);k++)
            {
                param[j++]=argv[i][k];
            }
            num++;
        }
    }
    for(i=0;i<j;i++)
    {
        if(param[i]=='a')
        {
            flag_param|=PARAM_A;
            continue;
        }
        else if(param[i]=='l')
        {
            flag_param|=PARAM_L;
            continue;
        }
        else if(param[i]=='R')
        {
            flag_param|=PARAM_R;
            continue;
        }
        else
        {
            printf("my_ls:invalid option -%c\n",param[i]);
            exit(1);
        }
    }
    param[j]='\0';
    if((num+1)==argc)
    {
        strcpy(path,"./");
        path[2]='\0';
        if(flag_param==4||flag_param==5||flag_param==6||flag_param==7)
        {
            display_digui(flag_param,path);
        }
        else
            display_dir(flag_param,path);
        return 0;
    }
    i=1;
    do
    {
        if(argv[i][0]=='-')
        {
            i++;
            continue;
        }
        else
        {
            strcpy(path,argv[i]);
            stat(path,&buf);
            if(S_ISDIR(buf.st_mode))
            {
                if(path[strlen(argv[i]-1)]!='/')
                {
                    path[strlen(argv[i])]='/';
                    path[strlen(argv[i+1])]='\0';
                }
                else 
                    path[strlen(argv[i])]='\0';
                if(flag_param==4||flag_param==5||flag_param==6||flag_param==7)
                {
                    display_digui(flag_param,path);
                }
                else
                {
                    display_dir(flag_param,path);
                }
                i++;
            }
            else
            {
                display(flag_param,path);
                i++;
            }
        }
    }while(i<argc);
    return 0;
}

