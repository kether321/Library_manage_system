#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_BOOK 300
#define MAX_RECOMMEND 10
#define MAX_HISTORY 100

enum Type{
  A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z
};

typedef struct 
{
    char ISBN[30];
    char borrow_date[20];
    char return_date[20];
    int reader_id;
    int booksort;
}Borrow_History;

typedef struct {
  char name[20];
  char ISBNcode[30];
  char author[20];
  char publishHouse[20];
  char publishDate[12];
  char location[20];
  char  id[300];
  int  amount;
  int  value;
  int  timesBeenrent;
  int  baseValueforrentperday;
  enum Type type; 
}Books;

Books book[MAX_BOOK];
Borrow_History history[MAX_HISTORY];
int current_num;
int reader_record_count=0;                //用户借阅记录
int id;

//更新借阅次数
void updating_times(char *ISBN)
{  
  int found=0;
  for(int i=0;i<current_num;i++)
  {
    if(strcmp(book[i].ISBNcode,ISBN)==0)
    {
    book[i].timesBeenrent++;
    printf("借阅次数更新完毕！\n");
    found=1;
    return;
    }
  }
  if (found==0)
  {
    printf("未找到该ISBN码下的图书!\n");
  }
   
}

//按热度排序
void rank_by_popularity ()
{
 Books temp;
 for(int i=0;i<current_num-1;i++)
 {
  for(int j=0;j<current_num-i-1;j++)
  {
    if(book[j].timesBeenrent<book[j+1].timesBeenrent)
    {
       temp=book[j];
       book[j]=book[j+1];
       book[j+1]=temp;
    }
  }
 }
}

//根据图书热度推荐
void give_recommendations_by_records()
{
    if (current_num==0)
    {
        printf("暂无图书数据！");
        return;
    }
    
    rank_by_popularity();
    int recommends=0;
    printf("\n============推荐书单============\n");
    for(int i=0;i<current_num&&recommends<MAX_RECOMMEND;i++)
    {
        if (book[i].amount>0)
        {
           printf("%d.《%s》\n",recommends+1,book[i].name);
           printf("   作者：%s\n",book[i].author);
           printf("   位置：%s\n",book[i].location);
           recommends++;
        }
        
    }
    if(recommends==0)
      printf("热门书籍暂无库存！\n");
    printf("==================================\n");

}

//根据借阅历史推荐
void give_recommendations_by_history(int user_id)
{   
    int likesort=0;
    int sort[26]={0};
    if (current_num==0)
    {
        printf("暂无图书数据！");
        return;
    }
    
    
    for(int i=0;i<reader_record_count;i++)
    {
        if (history[i].reader_id==user_id)
       {
        sort[history[i].booksort]++;
       }
    }

    //找出最喜欢的类别
    int maxcount=0;               //某类别最大借阅次数
    for(int i=0;i<26;i++)
    {
        if(sort[i]>maxcount)
        {
            maxcount=sort[i];
            likesort=i;
        }
    }
    
    if (maxcount==0)
    {
        printf("您未借阅过图书，无法根据借阅历史推荐！");
    }
    else
    {
        printf("您最爱的类别为[%c]\n",'A'+likesort);
        printf("以下为推荐书单！\n");
    }
        
    int recommends=0;
    printf("\n============推荐书单============\n");
    for(int i=0;i<current_num&&recommends<MAX_RECOMMEND;i++)
    {
        if (book[i].type==likesort&&book[i].amount>0)
        {
           printf("%d:《%s》\n",recommends+1,book[i].name);
           printf("   作者：%s\n",book[i].author);
           printf("   位置：%s\n",book[i].location);
           recommends++;
        }
        
    }
    if(recommends==0)
      printf("该类别下热门书籍暂无库存！\n");
    printf("==================================\n");
}

int main(){
    init_test_data();
    int choose;
    char ISBN[30];
    while(1){
        printf("=============操作菜单=============\n");
        printf("1:更新借阅次数\n");
        printf("2:根据图书热度（借阅次数）获取图书推荐\n");
        printf("3:根据借阅记录获取图书推荐\n");
        printf("4.返回主系统\n");
        printf("==================================\n");
        printf("请输入您的选择(1/2/3/4):");
        scanf("%d",&choose);
        getchar();
      
        switch (choose)
        {
        case 1:
            printf("请输入ISBN码:");
            scanf("%s",ISBN);
            updating_times(ISBN);
            break;
        
        case 2:
            give_recommendations_by_records();
            break;

        case 3:
            printf("请输入您的id:");
            scanf("%d",&id);
            give_recommendations_by_history(id);
            break;

        case 4:
            printf("即将返回主系统！");
            return 0;

        default:
            printf("未识别到有效选择，请重新输入！");
            break;
        }
    }
    return 0;
}