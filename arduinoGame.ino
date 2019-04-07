#include <MsTimer2.h>
#include <string.h>
#include <stdlib.h>

int col[8] = {2,3,4,5,6,7,8,9};
int row[8] = {A2,A3,A4,A5,13,12,11,10};
int px = 4;//记录角色位置，初始时在第五格
int roundNum=0; //记录回合数
int matrix[8][8]={//代表8*8LED的输入矩阵
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,1,0,0,0},
                };

                
void setup() {
  //初始化8*8LED
  for(int i=0; i<8; i++){
    pinMode(row[i],OUTPUT);
    pinMode(col[i],OUTPUT);
    digitalWrite(row[i],LOW);
    digitalWrite(col[i],HIGH);
  }
  Serial.begin(9600);
  MsTimer2::set(1000,action);//每1秒更新一次游戏进度
  MsTimer2::start();
}

void loop() {
  show();
}

void show(){
  //将当前矩阵显示在8*8LED上
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      if(matrix[i][j]==1){
        digitalWrite(row[i],HIGH);
        digitalWrite(col[j],LOW);
        delay(1);
        digitalWrite(row[i],LOW);
        digitalWrite(col[j],HIGH);
      }
    }
  }
}

void action(){
  //游戏内容
  
  runRound();//进行一回合
  if( (roundNum%2)==0){
    newStep();//生成新台阶
  }
  roundNum++;//回合数加1

  //摇杆读数字并判断角色移动
  int rocker  = analogRead(A1); 
  if(rocker <= 600 && rocker>=450 ){
    return;
  }
  if((rocker > 600) && (px!=0) ){
    Serial.println("左移");  
    matrix[7][px] = 0;
    px--; 
    matrix[7][px] = 1;
  }else if( rocker<450 && px!=7){
    Serial.println("右移");
    matrix[7][px] = 0;
    px++;
    matrix[7][px] = 1;
  }else if(px==7||px==0){
    Serial.println("到头");
  }
  
}

void runRound(){
  
  if(isDead()==1){
    //死亡
    deathHanlder();
  }else{
    //未死亡，游戏继续
    for(int i=6; i>0; i--){
      for(int j=0; j<8; j++){
        if(matrix[i-1][j] == 1){
          matrix[i][j] = 1;
          matrix[i-1][j]=0;
        }else{
          matrix[i][j] = 0;
        }
      }
    }
  }
}

int isDead(){  
  //判断是否死亡
  if(matrix[6][px]==1){    
    return 1;
  }
  else{    
    return 0;
  }
}

void deathHanlder(){
  //死亡情况处理 
  MsTimer2::stop();//停止游戏
  MsTimer2::set(500,renew);//每500毫秒进行一次是否复活的指令监听
  MsTimer2::start();
}

void newStep(){
  //生成新台阶
  int stepWidth = 2;//台阶宽度
  int xstep = rand()%7;
  for(int i=0; i<stepWidth; i++){
    matrix[0][xstep+i]=1;
  }
}

void renew(){
  //监听复活指令
  int rocker  = analogRead(A0);
  if(rocker>900){
    MsTimer2::stop();//停止监听

    //重置矩阵
    for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        matrix[i][j] = 0;
      }
    }
    matrix[7][4] = 1;
    px = 4;//重置px
    MsTimer2::set(1000,action);//重新开始游戏
    MsTimer2::start();
  }
}
