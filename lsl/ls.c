#include <dirent.h> 
#include <grp.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <time.h>
#include <pwd.h>
#include <unistd.h>
 
int main(int argc, char **argv) 
{ 
   char *curr_dir      = NULL; 
   DIR *dp             = NULL; 
   struct dirent *dptr = NULL; 
   unsigned int count  = 0; 
   struct stat fileStat;
   curr_dir = getenv("PWD"); 
   dp       = opendir((const char*)curr_dir);   

   while ((dptr = readdir(dp)) != NULL) 
   { 
       stat(dptr->d_name, &fileStat);
       uid_t uid          = getuid();
       gid_t gid          = getgid();
       struct passwd *pw  = getpwuid(uid);
       struct group  *grp = getgrgid(gid);
       char output[100];
       char date[20];

       strftime(date, 20, "%b %d %H:%M", localtime(&(fileStat.st_mtime)));
       sprintf (output, "%s%s%s%s%s%s%s%s%s%s %s %s %ld %5ld %s %s\n",
       
           (S_ISDIR(fileStat.st_mode))  ? "d"  : "-",
           (fileStat.st_mode & S_IRUSR) ? "r" : "-",
           (fileStat.st_mode & S_IWUSR) ? "w" : "-",
           (fileStat.st_mode & S_IXUSR) ? "x" : "-",
           (fileStat.st_mode & S_IRGRP) ? "r" : "-",
           (fileStat.st_mode & S_IWGRP) ? "w" : "-",
           (fileStat.st_mode & S_IXGRP) ? "x" : "-",
           (fileStat.st_mode & S_IROTH) ? "r" : "-",
           (fileStat.st_mode & S_IWOTH) ? "w" : "-",
           (fileStat.st_mode & S_IXOTH) ? "x" : "-",
           uid ? pw->pw_name  : "root",
           gid ? grp->gr_name : "root",
           fileStat.st_nlink,
           fileStat.st_size,
           date,
           dptr->d_name);

       printf("%s", output);
   } 

  
   return 0; 
}