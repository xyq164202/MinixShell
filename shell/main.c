//
//  main.c
//  shell
//
//  Created by Yangyang Xie, Yuzhang Hu, Qinyao Xu  on 2/10/15.
//
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "myStack.h"
#include "main.h"



char environment[2][155];                               //store the environment variables (PATH AND HOME)
char c = '\0';
char **my_argv;                                         // this string array  is used to store the arguments of the commend
                                                        // when you press the ctrl+c, you will stop it.
char **environ;                                         //external environement viriables
char cwd[155];
void handle_signal(int signo)
{
    char input=
    printf("\n[Commend terminal, Are you sure?[y/n]] ");
    scanf("%c",&input);
    if(input=='y'|| input=='\n')
    {

        printf("\n[process terminal]\n ");
                exit(0);
    }
    else{
        fflush(stdout);
    }
}
void seperate_argv(char* tmp)                           //seperate the argument variable
{
    my_argv=(char **)malloc(20*sizeof(char *));
        strncat(tmp," ", 1);                            //add a space after each command word
    char *foo = tmp;
    int index=0;
    char part[100];
    bzero(part, 100);
    while(*foo !='\0')
    {
        if (index== 10)
            break;
        if(*foo == ' ')
        {
            if(my_argv[index]==NULL)
                my_argv[index] = (char *)malloc(strlen(part)* sizeof(char)+1);
            else
                bzero(my_argv[index], strlen(my_argv[index]));
            strncpy(my_argv[index], part, strlen(part));
            strncat(my_argv[index], "\0", 1);
            bzero(part, 100);
            index++;
            
            
        }
        else
            strncat(part,foo,1);
        foo++;
    }
    
}
void testAndExecute(){                                      //execute addressed command lines
    char ** inv_path =(char **)malloc(sizeof(char *)*2);
    inv_path[0]=(char *)malloc(strlen(environment[0])*sizeof(char)+1);
    inv_path[1]=(char *)malloc(strlen(environment[1])*sizeof(char)+1);
    strcpy(inv_path[0],environment[0]);
    strncat(inv_path[0], "\0", 1);
    strcpy(inv_path[1],environment[1]);
    strncat(inv_path[1], "\0", 1);
    if(fork() == 0) {
        execve(my_argv[0],&my_argv[0],inv_path);
        environ=inv_path;
        int i=execvp(my_argv[0], &my_argv[0]);
        if(i<0)
        {
            printf("%s: %s\n", my_argv[0], "command not found");
            exit(1);
        }
         kill(0, SIGKILL);
    }
    else {
        wait(NULL);
    }
}


int preTest(int flag){                      //deal with "$" symbol, and set golabl variables.
    int count=0;
    if (my_argv[1]==NULL || strlen(my_argv[1])==0)
    {
        FILE * pf;
        while(count<strlen(my_argv[0]))
        {
            if(*(my_argv[0])=='$')
            {
                flag =2;
                break;
            }
            
            if(*(my_argv[0]+count++)=='=')
                flag = 1;
        }
        if(flag ==1)
        {
            
            char * dir=malloc(sizeof(char)*100);
            bzero(dir, 100);
            strcpy(dir, cwd);
            strcat(dir,"/variables");
            pf=fopen(dir,"a+");
            fwrite("\n", sizeof(char),1,pf);
            fwrite(my_argv[0], sizeof(char),strlen(my_argv[0]),pf);
            fclose(pf);
            
        }
        if(flag==2)
        {
            char * dir=malloc(sizeof(char)*100);
            int new =0;
            char c;
            bzero(dir, 100);
            strcpy(dir, cwd);
            strcat(dir,"/variables");
             pf=fopen(dir,"r");
            char *val = malloc(sizeof(char)*10);
            bzero(val, 10);
            do{
                c=fgetc(pf);
                while(c!=EOF&& new==0)
                {
                    if(c=='\n')
                    new =1;
                    c=fgetc(pf);
                }
                if(new !=0&& c==*(my_argv[0]+sizeof(char)))
                {
                    while(c!=EOF && c!='\n')
                    {
                    strncat(val,&c,1);
                        c=fgetc(pf);
                    }
                    new=0;
                }
                else
                    new =0;

            }
            while(c!=EOF);
            printf("%s\n\n",val);
        }
        
    }
    return flag;
}



void executeCommend(char* tmp)
{
    int flag =0;
    seperate_argv(tmp);
    if(preTest(flag)==0)
            testAndExecute();
    
}


void readEnv()                          //read the environment vriables from the scripts
{
    int i=0;
    FILE *env;
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        exit(0);
    char * dir=malloc(sizeof(char)*100);
    bzero(dir, 100);
    strcpy(dir, cwd);
    strcat(dir, "/profile");
    env=fopen(dir,"r");
    char line [255];
    if(!env)
    {
        printf("could not find the profile");
        exit(0);
    }
    while(fgets(line ,sizeof line,env)!=NULL)
          {
              strncpy(environment[i++], line, 100);
              bzero(line, 255);
          }
    fclose(env);
}
int main(int argc, const char **argv, char **envp) {
    readEnv();
   	signal(SIGINT, SIG_IGN);
    signal(SIGINT, handle_signal);
    
    char tmp[100];               // a string to store the whole commend.
    bzero(tmp, 100);
    Stack str_stk,tmp_stk;
    str_stk = initial(sizeof(char), freeStr);
    tmp_stk = initial(sizeof(char), freeStr);
    printf("%s\n[shell]:",cwd);
    while (c !=EOF) {
        c =getchar();
        if(c!=EOF)
        {
            switch (c) {
                case '\n': /* parse and execute. */
                    printf("%s",stackStorage(str_stk));
                    bzero(tmp, sizeof(tmp));
                    printf("%s\n[shell]:",cwd);
                    break;
                default: {
                    push(str_stk, &c);
                }
                    break;
            }
        }
    }
        printf("%s",tmp);

    printf("\n");
    return 0;
}
char* stackStorage(Stack str_stk)
{
    char *character_in_stack=(char *)calloc(1,sizeof(char));
    char *parenthesis_l = "(";                                             // a sign of "("
    char *parenthesis_r= ")";
    char lineWithoutBacket=1;
    char * tmp_exchange;
    int i=0;                                                        //this is used for count.
    FILE   *stream;
    int WORD_LEN_MAX=100;
    char word[WORD_LEN_MAX];
    char *result;                                                   //结果存在这个变量里
    result=(char* )calloc(100,sizeof(char));
    char *part ="\0";
    char * commend_wait_excuted=(char* )calloc(500,sizeof(char));
    while ( !isEmpty(str_stk) ) {
        top(str_stk, character_in_stack);
        if (strcmp(character_in_stack, parenthesis_l)==0)
        {
            lineWithoutBacket=0;
            while(result[i]!='\0')
            {
                if(result[i]=='\n')
                    result[i]=' ';
                i++;
            }
            strcpy(commend_wait_excuted,part);
            strcat(commend_wait_excuted, result);
            stream= popen(commend_wait_excuted, "r");
            bzero(commend_wait_excuted, 500);
            bzero(result, 100);
            if (stream == NULL)
            {
                printf("commends usage error !");
                exit(0);
            }
            while (fgets(word, WORD_LEN_MAX, stream) != NULL)
            {
                strcat(result, word);
            }

            int status = pclose(stream);
            if (status == -1) {
                printf("ERROR");
            } else {
                printf("SUCCESS!");
            }
        }
        else {
            if(strcmp(character_in_stack,parenthesis_r)!=0)
            {
                tmp_exchange=malloc(strlen(part)+sizeof(char)+2);
                strcpy(tmp_exchange,character_in_stack);
                strcat(tmp_exchange, part);
                strncat(tmp_exchange, "\0", 1);
                part=tmp_exchange;

            }
        }
    }
    if(lineWithoutBacket)
    {
        executeCommend(part);
        return "";
    }
    else{
    return result;
    }

}
