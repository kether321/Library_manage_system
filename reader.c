#include <stdio.h>
#include <string.h>
#include <stdbool.h>
typedef struct {
    char name[20];
    char id[30];
    char phone[20];
    char code[20];
    bool power;
}readers;
typedef struct Node{
    readers reader;
    struct Node *next;
}node;
node *head;
void init(){
    head = NULL;
}
void sign_up(){
    node *p = (node *)malloc(sizeof(node));
    readers reader;
    printf("欢迎您的注册！请输入注册信息\n");
    printf("姓名：");
    scanf("%s",reader.name);
    printf("\n");
    printf("学号：");
    scanf("%s",reader.id);
    printf("\n");
    printf("电话号码：");
    scanf("%s",reader.phone);
    printf("\n");
    printf("登录密码：");
    scanf("%s",reader.code);
    printf("\n");
    reader.power = true;
    p->reader = reader;
    p->next = head;
    head = p; 
    printf("注册成功！\n");
}
readers log_in(){
    char id[30],code[20];
    printf("欢迎您的登录！\n");
    printf("请输入您的学号：");
    scanf("%s",id);
    printf("\n");
    printf("请输入您的密码：");
    scanf("%s",code);
    printf("\n");
    node *p = head;
    while(p != NULL){
        if(!strcmp(p->reader.id,id) && !strcmp(p->reader.code,code)){
            printf("登录成功！\n");
            return p->reader;
        }
        p = p->next;
    }
    free(p);
    printf("登录失败，请检查用户名或密码！\n");
}
int main(){
    
    return 0;
}