// ./code contains our code to be checked

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<ctype.h>


void compareFiles(FILE *fp1, FILE *fp2) // Comparing our code output with testcase output
{ 
    char ch1 = getc(fp1); 
    char ch2 = getc(fp2); 
    int error = 0, pos = 0, line = 1; 
    while (ch1 != EOF && ch2 != EOF) 
    { 
        if (ch1 != ch2) 
        { 
            printf("WRONG ANSWER\n");
            return;
        } 
        ch1 = getc(fp1); 
        ch2 = getc(fp2); 
    } 
  
    printf("ACCEPTED CODE\n");
} 


int main()
{
    // Create copies of stdin and stdout
    int std_in = dup(STDIN_FILENO);
    int std_out = dup(STDOUT_FILENO);

    // open test case input and code output file
    FILE* tc_in = fopen("tc_in.txt", "r+");
    FILE* code_out = fopen("code_out.txt", "w+");

    // transfer the stdout to the output file
    int fd = fileno(code_out);
    dup2(fd, 1);

    // popen the code (the stdout still points to code output)
    FILE* code_w = popen("./code", "w");
    int wfd = fileno(code_w);
    dup2(wfd, 1);    // transfer the stdout to code input
    
    char buffer[1024];
    memset(buffer, '\0', 1024);
    while(1)
    {
        char c = fgetc(tc_in);
        if(feof(tc_in))
            break;
        write(1, &c, sizeof(char));
    }

    pclose(code_w);
    fclose(code_out);
    fclose(tc_in);
    
    // transfer stdout back to screen
    dup2(std_out, 1);

    // compare
    FILE *fp1 = fopen("tc_out.txt", "r"); 
    FILE *fp2 = fopen("code_out.txt", "r");
    compareFiles(fp1, fp2);
    fclose(fp1);
    fclose(fp2);

    return 0;
}
