#include<stdio.h>
#include<dirent.h>
#include <stdlib.h>
 
void show_ls(char filename[]);
int main(int argc,char* argv[])
{
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);
	if(argc == 1)
		show_ls(".");
	while(--argc)
	{
		show_ls(*++argv);
		printf("\n");
	}
	return 0;
}
void show_ls(char filename[])
{
	DIR* dir_ptr;  //the directory
	struct dirent* direntp; //each entry
	
	if((dir_ptr = opendir(filename)) == NULL) {
        fprintf(stderr,"ls: cannot open [%s] \nUsage: ls DIR1 DIR2 ...\n",filename);
        exit(-1);
    }
		
	while((direntp = readdir(dir_ptr)) != NULL)
		printf("%s\n",direntp->d_name);
	closedir(dir_ptr);
}