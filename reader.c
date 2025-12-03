#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct {
    char name[20];
    char id[20];
    char phone[20];
    char code[20];
    bool power;
}readers;
typedef struct Node{
    readers reader;
    struct Node *next;
}node;
node *head,*user;
void init(){
    head = NULL;
    user = NULL;
    FILE *fp = fopen("reader.txt","r");
    if(fp == NULL){
        printf("系统获取信息失败！\n\n");
        return ;
    }
    readers reader;
    node *p;
    while(!feof(fp)){
        fscanf(fp, "%s",reader.name);
        fscanf(fp, " %s",reader.id);
        fscanf(fp, " %s",reader.phone);
        fscanf(fp, " %s",reader.code);
        fscanf(fp, " %d\n",&reader.power);
        p = (node *)malloc(sizeof(node));
        p->reader = reader;
        p->next = head;
        head = p;
    }
    fclose(fp);
    printf("系统初始化成功！\n\n");
}
void sign_up(){
    node *p = (node *)malloc(sizeof(node));
    readers reader;
    char name[20];
    char id[21];
    char phone[20];
    char code[20];
    printf("欢迎您的注册！请输入注册信息\n\n");
    printf("请输入您的姓名：");
    scanf("%s",name);
    printf("\n");
    while(true){
        printf("请输入您的账号：");
        scanf("%21s",id);
        int len = strlen(id);
        if(len >= 20){
            printf("账号太长！请重新输入\n\n");
            continue;
        }
        bool f = 1;
        for(int i = 0;i < len;i ++ ){
            if(id[i] == ' '){
                f = 0;
                break;
            }
        }
        if(!f){
            printf("账号中包含空格!请重新输入\n\n");
            continue;
        } else {
            printf("\n");
            break;
        }
    }
    while(true){
        printf("请输入您的电话号码：");
        scanf("%12s",phone);
        if(strlen(phone) != 11){
            printf("电话号码不符合要求！请重新输入！\n\n");
            continue;
        } else {
            printf("\n");
            break;
        }
    }
    while(true){
       printf("请输入您的密码（8~16位，包含大小写字母、数字和特殊字符）：");
       scanf("%17s",code);
       int len = strlen(code);
       if(len < 8 || len > 16){
            printf("密码长度不符！请重新输入\n\n");
            continue;
       }
       int cnt1 = 0,cnt2 = 0,cnt3 = 0,cnt4 = 0;
       for(int i = 0;i < len; i ++ ){
            if(code[i] >= 'a' && code[i] <= 'z') cnt1 ++ ;
            if(code[i] >= 'A' && code[i] <= 'Z') cnt2 ++ ;
            if(code[i] >= '0' && code[i] <= '9') cnt3 ++ ;
            if(code[i] == ' ') cnt4 ++ ;
       }
       if(cnt1 && cnt2 && cnt3 && !cnt4 && len - cnt1 - cnt2 - cnt3 > 0){
            printf("密码设置成功！\n\n");
            break;
       } else {
            printf("密码不满足条件！请重新输入\n\n");
       }
    }
    strcpy(reader.code,code);
    strcpy(reader.name,name);
    strcpy(reader.id,id);
    strcpy(reader.phone,phone);
    reader.power = true;
    p->reader = reader;
    p->next = head;
    head = p; 
    printf("注册成功！请重新登录\n\n");
}
node *log_in(){
    char id[20],code[20];
    printf("欢迎您的登录！\n");
    printf("请输入您的账号：");
    scanf("%20s",id);
    printf("\n");
    printf("请输入您的密码：");
    scanf("%16s",code);
    printf("\n");
    node *p = head;
    while(p != NULL){
        if(!strcmp(p->reader.id,id) && !strcmp(p->reader.code,code)){
            printf("登录成功！\n\n");
            return p;
        }
        p = p->next;
    }
    printf("登录失败，请检查用户名或密码！\n\n");
    return NULL;
}
void log_out(){
    user = NULL;
}
void change_code(node *p){
    char c1[20],c2[20];
    while(true){
        while(true){
            printf("请输入您的新密码（8~16位，包含大小写字母、数字和特殊字符）：");
            scanf("%17s",c1);
            int len = strlen(c1);
            if(len < 8 || len > 16){
                printf("\n密码长度不符！请重新输入\n");
                continue;
            }
            int cnt1 = 0,cnt2 = 0,cnt3 = 0,cnt4 = 0;
            for(int i = 0;i < len; i ++ ){
                if(c1[i] >= 'a' && c1[i] <= 'z') cnt1 ++ ;
                if(c1[i] >= 'A' && c1[i] <= 'Z') cnt2 ++ ;
                if(c1[i] >= '0' && c1[i] <= '9') cnt3 ++ ;
                if(c1[i] == ' ') cnt4 ++ ;
            }
            if(cnt1 && cnt2 && cnt3 && !cnt4 && len - cnt1 - cnt2 - cnt3 > 0){
                printf("\n");
                break;
            } else {
                printf("\n密码不满足条件！请重新输入\n");
            }
        }
        printf("请确认您的新密码：");
        scanf("%16s",c2);
        printf("\n");
        if(!strcmp(c1,c2)){
            strcpy(p->reader.code,c1);
            printf("修改成功！\n\n");
            break;
        } else {
            printf("\n两次密码不一致，请重新操作！\n");
        }
    }
}
void delete_account(node *p){
    printf("请确认是否要注销账号(Y/N)\n");
    char c = getchar();
    if(c == 'N') return ;
    node *q = head;
    while(q->next != p){
        q = q->next;
    }
    q->next = p->next;
    free(p);
    log_out();
    printf("注销成功！\n\n");
}
void flush(){
    FILE *fp = fopen("reader.txt","w");
    if(fp == NULL){
        printf("上传数据失败！\n\n");
        return ;
    }
    while(head != NULL){
        fprintf(fp,"%s ",head->reader.name);
        fprintf(fp,"%s ",head->reader.id);
        fprintf(fp,"%s ",head->reader.phone);
        fprintf(fp,"%s ",head->reader.code);
        fprintf(fp,"%d\n",head->reader.power);
        head = head->next;
    }
    fclose(fp);
    printf("数据更新成功！\n\n");
}
void print_info(node *p){
    printf("%s\n",p->reader.name);
    printf("%s\n",p->reader.id);
    printf("%s\n",p->reader.phone);
    printf("%s\n",p->reader.code);
    printf("%d\n",p->reader.power);
}
int op;
int main(){
    init();
    while(true){
        if(user == NULL){
            printf("请注册或登录！\n");
            printf("请选择您的操作 1 注册 2 登录 3 退出系统\n");
            scanf("%d",&op);
            if(op == 1){
                sign_up();
            } else if(op == 2){
                user = log_in();
            } else {
                printf("成功退出系统！\n");
                break;
            }
        } else {
            print_info(user);
            printf("请选择您的操作 1 修改密码 2 注销账号 3 登出 4 退出系统\n");
            scanf("%d",&op);
            if(op == 1){
                change_code(user);
            } else if(op == 2){
                delete_account(user);
            } else if(op == 3){
                log_out();
            } else {
                printf("成功退出系统！\n");
                break;
            }
        }
    }
    flush();
    return 0;
}