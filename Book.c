#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
static int MAX_BOOK=300;
static int MAX_ID=300;
enum Type{
  A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,  
};
typedef struct {
    char name[20];
    char ISBNcode[30];
    char author[20];
    char publishHouse[20];
    char publishDate[12];
    char location[20];
    char id[300];
    int  amount;
    int  value;
    int  timesBeenrent;
    int  baseValueforrentperday;
    enum Type type; 
}Books;
typedef struct Node{
    Books book;
    struct Node *next;
}Node;
typedef struct{
    Node *head;
    int current_num;
}Bookslist;
bool isfirstenter;
bool checkifISBNisright(Books books){
    if (books.ISBNcode[3]!='-'&&books.ISBNcode[5]!='-'&&books.ISBNcode[10]!='-'&&books.ISBNcode[15]!='-')
    {   
        printf("请输入正确的ISBN编码:");
        return false;
    }else{
        int count=0,index=1;
        for (int i = 0; i < 16; i++)
        {
            if (books.ISBNcode[i]!='-')
            {
                if (index%2==1)
                {
                    count+=(int)books.ISBNcode[i]-'0';   
                }else{
                    count+=((int)books.ISBNcode[i]-'0')*3;   
                }
                index++;
            }
        }
        if ((int)books.ISBNcode[16]-'0'==10-count%10)
        {
            return true;
        }else{
            printf("请输入正确的ISBN编码:");
            return false;
        }
    }
}
bool checkifdateisright(Books books){
    if (books.publishDate[4]!='/'&&books.publishDate[7]!='/')
    {
        printf("请输入正确的日期格式:\n");
        return false;
    }
    return true;
}
bool checkID(Books books,Bookslist *h){
    Node *node=h->head;
    while (node)
    {
         if(!strcmp(books.id,node->book.id))
        {
            printf("此ID已存在：\n");
            return false;
        }else{
            node=node->next;
        }
    }
    return true;
}
Books initialBookinfo(Books books,Bookslist *h){
            printf("请输入书名：");
            scanf("%s", books.name);
            printf("请输入书的ISBN：");
            scanf("\t%s", books.ISBNcode);
             while (!checkifISBNisright(books))
            {
                scanf("\t%s", books.ISBNcode);
            }
            printf("请输入作者：");
            scanf("\t%s", books.author);
            printf("请输入出版社：");
            scanf("\t%s", books.publishHouse);
            printf("请输入出版日期：");
            scanf("\t%s", books.publishDate);
            while (!checkifdateisright(books))
            {
                scanf("\t%s", books.publishDate);
            }
            printf("请输入储藏位置：");
            scanf("\t%s", books.location);
            printf("请输入书本量：");
            scanf("\t%d", &books.amount);
            printf("请输入书本单价：");
            scanf("\t%d", &books.value);
            printf("是否查看书本分类表（ Y / N ）");
            char command[3];
            scanf("%s", command);
            if (!strcmp(command,"y")||!strcmp(command,"Y"))
            {
            explainThetypeofbooks();
            }
            printf("请输入书本类型：");
            char type[2];
            scanf("\t%s", type);
            books.type=type[0]-65;
            printf("请输入书的id：");
            scanf("\t%s", books.id);
            while (!checkID(books,h))
            {
                scanf("\t%s", books.id);
            }
            books.timesBeenrent=0;
            return books;
}
void info_init(Bookslist *h){
    FILE *fp;
    if((fp = fopen("D:\\Homework\\Library_manage_system\\BooksInfo.txt", "r")) == NULL){
        if((fp = fopen("D:\\Homework\\Library_manage_system\\BooksInfo.txt", "w")) == NULL)
            printf("创建书本信息文件失败！ \n");
    }
    else{
            Books book;
            Node *node;
            h->head = NULL;
            h->current_num=0 ;
            while(!feof(fp)){
                fscanf(fp, "\t%s", book.name);
                fscanf(fp, "\t%s", book.ISBNcode);
                fscanf(fp, "\t%s", book.author);
                fscanf(fp, "\t%s", book.publishHouse);
                fscanf(fp, "\t%s", book.publishDate);
                fscanf(fp, "\t%s", book.location);
                fscanf(fp, "\t%d", &book.amount);
                fscanf(fp, "\t%d", &book.value); 
                int type;
                fscanf(fp,"\t%d", &type);
                book.type=type;
                fscanf(fp, "\t%s", book.id);
                node = (Node *)malloc(sizeof(Node));
                node->book = book;
                node->next = h->head;
                h->head = node;
                h->current_num ++;
        }
    }
    fclose(fp);
    printf("构建书本信息链表完成! \n总量: %d \n", h->current_num);
}
void flush_Book(Bookslist *h){
    FILE *fp;
    Node *p = h->head;
    int count = 0;
    if((fp = fopen("D:\\Homework\\Library_manage_system\\BooksInfo.txt", "w")) == NULL){
        printf("打开文件失败 \n");
        return;
    }
    while(p){
        fprintf(fp, "\t%s\t", p->book.name);
        fprintf(fp, "%s\t", p->book.ISBNcode);
        fprintf(fp, "%s\t", p->book.author);
        fprintf(fp, "%s\t", p->book.publishHouse);
        fprintf(fp, "%s\t", p->book.publishDate);
        fprintf(fp, "%s\t", p->book.location);
        fprintf(fp, "%d\t", p->book.amount);
        fprintf(fp, "%d\t", p->book.value); 
        fprintf(fp, "%d\t", p->book.type);
        fprintf(fp, "%s", p->book.id);
        p = p->next;
        count ++;
    }
    fclose(fp);
    if(count >0)
        printf("将%d书本信息写入文件 \n", count);
    else
        printf("没有数据写入 \n");
    }
void print_books(Bookslist *h){
        if(h->current_num <=0){
            printf("链表是空的！\n");
        }
        Node * p = h->head;
        while (p) {
            print_one_book(p);
            p = p->next;
        }    
    }
void print_one_book(Node *p){
printf("=========================================\n");
            printf( "书名：%s \n", p->book.name);
            printf( "书的ISBN：%s \n", p->book.ISBNcode);
            printf( "作者：%s \n", p->book.author);
            printf( "出版社：%s \n", p->book.publishHouse);
            printf( "出版日期：%s \n", p->book.publishDate);
            printf( "储藏位置：%s \n", p->book.location);
            printf( "书本量：%d \n", p->book.amount);
            printf( "书本单价：%d \n", p->book.value);
            printf( "书本类别：%c \n", (char)(p->book.type+65));
            printf( "书本id：%s \n", p->book.id);
    printf("=======================================\n");
}

void add_book(Bookslist *h){
    if(h->current_num >= MAX_BOOK){
        printf("书籍信息的链表已满，请先删除一些书籍\n");
        return;   
     }
    Node *p = (Node *)malloc(sizeof(Node)), *tmp;
    Books books;
    p->book = initialBookinfo(books,h);
    printf(" 请输入表示您想插入位置的数字。\n0：开头；1：末尾；i：第i个位置\n");
    int index;
    scanf("%d", &index);
    switch (index) {
        case 0:
            p->next = h->head;
            h->head = p;
            h->current_num += 1;
            break;
        case 1:
            tmp = h->head;
            while(tmp->next){
                tmp = tmp->next;
            }
            tmp->next = p;
            p->next = NULL;
            h->current_num += 1;
            break;
        default:
            if(index <= MAX_BOOK){
                tmp = h->head;
                for(int i = 0; i < index; i ++){
                    tmp = tmp->next;
                }
                p->next = tmp->next;
                tmp->next = p;
                h->current_num += 1;
            }
            else
                printf("所输位置超过界限 \n");
            break;
    }
    printf("成功插入\n");
 }
void delete_book(Bookslist *h){
        if(h->current_num <=0){
        printf("书本链表为空 ! \n");
        return;    }
        printf(" 请输入要删除的书本ID（-1：退出）：\n");
        char id[MAX_ID];
        scanf("%s", id);
        if(!strcmp(id, "-1"))
            return ;
        Node *p = h->head;
        if(!strcmp(p->book.id, id)){
        h->head = p->next;
        free(p);
        return ;
        }
        while(p->next != NULL && strcmp(p->next->book.id, id)){
            p = p->next;
        }
        if(p->next == NULL){
              printf("这本书不存在 \n");
        }
        else{
            print_one_book(p->next);
        }
        if (p==NULL)
        {
            return;
        }
        printf("删除该书本？（Y/N） \n");
        char answer[2];
        scanf("%s", answer);
        if(answer[0] == 'Y' || answer[0] == 'y'){
            Node *tmp;
            tmp = p->next;
            p->next = tmp->next;
            free(tmp);
            h->current_num -= 1;
        }
        else
        return;
    } 
void update_book(Bookslist *h){
    printf("请输入要更新的书本ID（-1：退出）： \n");
    char id[MAX_ID];
    scanf("%s", id);
    if(!strcmp(id, "-1")){
        return;
    }
    Node *p = h->head;
    while(p != NULL && (strcmp(p->book.id, id))){
        p = p->next;
    }
    if(p == NULL){
        printf("书本不存在\n");
    }
    else{
        printf("输入新的书本信息 \n");
        p->book=initialBookinfo(p->book,h);
        printf("成功更改 \n");
        print_one_book(p);
    }
}

void explainThetypeofbooks(){
    printf("A 马克思主义、列宁主义、毛泽东思想、邓小平理论\nB 哲学、宗教\nC 社会科学总论\nD 政治、法律\nE 军事\nF 经济\nG 文化、科学、教育、体育\nH 语言、文字\nI 文学\nJ 艺术\nK 历史、地理\nN 自然科学总论\nO 数理科学和化学\nP 天文学、地球科学\nQ 生物科学\nR 医药、卫生\nS 农业科学\nT 工业技术\nU 交通运输\nV 航空、航天\nX 环境科学、安全科学\nZ 综合性图书\n");
}
void maincontral_book(Bookslist *list){
    char command[5];
    printf("请输入指令：\n1.查看所有书本信息\n2.创建新书本\n3.更新书本信息\n4.删除某本书\n5.退出至上一个界面\n");
    scanf("%s", command);
    if (!strcmp(command,"1"))
    {
        print_books(list);
    }else if (!strcmp(command,"2"))
    {
        add_book(list);
    }else if (!strcmp(command,"3"))
    {
        update_book(list);
    }else if (!strcmp(command,"4"))
    {
        delete_book(list);
    }else if (!strcmp(command,"5"))
    {
        return;
    }else{
        printf("请输入正确的指令\n");
    }
    flush_Book(list);
    maincontral_book(list);
}
int main(){
    Bookslist *list=(Bookslist *)malloc(sizeof(Bookslist));
    info_init(list);
    isfirstenter=true;
    maincontral_book(list);
    return 0;
}
//以上内容为杜沐华所写
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
typedef struct Node_reader{
    readers reader;
    struct Node_reader *next;
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
        int t = fscanf(fp, "%s",reader.name);
        if(t != 1) break; 
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
    printf("系统初始化成功!\n\n");
}
void sign_up(){
    node *p = (node *)malloc(sizeof(node));
    readers reader;
    char name[20];
    char id[21];
    char phone[20];
    char code[20];
    printf("欢迎您的注册!请输入注册信息\n\n");
    printf("请输入您的姓名:");
    scanf("%20s",name);
    printf("\n");
    while(true){
        printf("请输入您的账号:");
        scanf("%21s",id);
        int len = strlen(id);
        if(len >= 20){
            printf("账号太长!请重新输入\n\n");
            continue;
        }
        node *tmp = head;
        bool flag = 1;
        while(tmp != NULL){
            if(!strcmp(tmp->reader.id,id)){
                printf("账号已经存在!\n\n");
                flag = 0;
                break;
            }
            tmp = tmp->next;
        }
        if(!flag) continue;
        printf("\n");
        break;
    }
    while(true){
        printf("请输入您的电话号码：");
        scanf("%12s",phone);
        int len = strlen(phone);
        if(len != 11){
            printf("电话号码长度不符合要求！请重新输入！\n\n");
            continue;
        }
        bool flag = 1;
        for(int i = 0;i < len;i ++ ){
            if(phone[i] < '0' || phone[i] > '9'){
                printf("手机号包含除数字外的其他字符,请重新输入!\n\n");
                flag = 0;
                break;
            }
        }
        if(!flag) continue;
        if(phone[0] != '1' || phone[1] == '0' || phone[1] == '1' || phone[1] == '2'){
            printf("手机号不符合要求,请重新输入!\n\n");
            continue;
        }
        printf("\n");
        break;
    }
    while(true){
       printf("请输入您的密码(8~16位,包含大小写字母、数字和特殊字符):");
       scanf("%17s",code);
       int len = strlen(code);
       if(len < 8 || len > 16){
            printf("密码长度不符！请重新输入\n\n");
            continue;
       }
       int cnt1 = 0,cnt2 = 0,cnt3 = 0;
       for(int i = 0;i < len; i ++ ){
            if(code[i] >= 'a' && code[i] <= 'z') cnt1 ++ ;
            if(code[i] >= 'A' && code[i] <= 'Z') cnt2 ++ ;
            if(code[i] >= '0' && code[i] <= '9') cnt3 ++ ;
       }
       if(cnt1 && cnt2 && cnt3 && len - cnt1 - cnt2 - cnt3 > 0){
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
    printf("\n注册成功!请重新登录\n\n");
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
    printf("登录失败,请检查用户名或密码!\n\n");
    return NULL;
}
void log_out(){
    user = NULL;
}
void change_code(node *p){
    char c1[20],c2[20];
    while(true){
        while(true){
            printf("请输入您的密码(8~16位,包含大小写字母、数字和特殊字符):");
            scanf("%17s",c1);
            int len = strlen(c1);
            if(len < 8 || len > 16){
                printf("\n密码长度不符!请重新输入\n");
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
                printf("\n密码不满足条件!请重新输入\n");
            }
        }
        printf("请确认您的新密码:");
        scanf("%16s",c2);
        printf("\n");
        if(!strcmp(c1,c2)){
            strcpy(p->reader.code,c1);
            printf("修改成功！\n\n");
            break;
        } else {
            printf("\n两次密码不一致,请重新操作!\n\n");
        }
    }
}
void delete_account(node *p){
    printf("请确认是否要注销账号(Y/N)\n");
    getchar();
    char c = getchar();
    if(c != 'Y') return ;
    if(p == head){
        head = head->next;
    } else{
        node *now = head,*pre = NULL;
        while(now != p){
            pre = now;
            now = now->next;
        }
        pre->next = now->next;
    }
    free(p);
    log_out();
    printf("注销成功!\n\n");
}
void flush_reader(){
    FILE *fp = fopen("reader.txt","w");
    if(fp == NULL){
        printf("上传数据失败!\n\n");
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
    printf("%d\n\n",p->reader.power);
}
void maincontral_reader(){
int op;
init();
    while(true){
        if(user == NULL){
            printf("请注册或登录!\n");
            printf("请选择您的操作 1 注册 2 登录 3 退出系统\n");
            scanf("%d",&op);
            if(op == 1){
                sign_up();
            } else if(op == 2){
                user = log_in();
            } else {
                printf("成功退出系统!\n");
                break;
            }
        } else {
            // print_info(user);
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
    flush_reader();
}
