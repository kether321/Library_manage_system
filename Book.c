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
void flush(Bookslist *h){
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
            print_one(p);
            p = p->next;
        }    
    }
void print_one(Node *p){
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

void add(Bookslist *h){
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
void delete(Bookslist *h){
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
            print_one(p->next);
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
void update(Bookslist *h){
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
        print_one(p);
    }
}

void explainThetypeofbooks(){
    printf("A 马克思主义、列宁主义、毛泽东思想、邓小平理论\nB 哲学、宗教\nC 社会科学总论\nD 政治、法律\nE 军事\nF 经济\nG 文化、科学、教育、体育\nH 语言、文字\nI 文学\nJ 艺术\nK 历史、地理\nN 自然科学总论\nO 数理科学和化学\nP 天文学、地球科学\nQ 生物科学\nR 医药、卫生\nS 农业科学\nT 工业技术\nU 交通运输\nV 航空、航天\nX 环境科学、安全科学\nZ 综合性图书\n");
}
void maincontral(Bookslist *list){
    char command[5];
    printf("请输入指令：\n1.查看所有书本信息\n2.创建新书本\n3.更新书本信息\n4.删除某本书\n5.退出至上一个界面\n");
    scanf("%s", command);
    if (!strcmp(command,"1"))
    {
        print_books(list);
    }else if (!strcmp(command,"2"))
    {
        add(list);
    }else if (!strcmp(command,"3"))
    {
        update(list);
    }else if (!strcmp(command,"4"))
    {
        delete(list);
    }else if (!strcmp(command,"5"))
    {
        return;
    }else{
        printf("请输入正确的指令\n");
    }
    flush(list);
    maincontral(list);
}
int main(){
    Bookslist *list=(Bookslist *)malloc(sizeof(Bookslist));
    info_init(list);
    isfirstenter=true;
    maincontral(list);
    return 0;
}