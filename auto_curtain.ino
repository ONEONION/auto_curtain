/* auto-curtain 智能窗帘 */
/* author: tttt-top */
/* date:20160329 */

#include"IRremote.h"
#include "TimeLib.h"

int RECV_PIN=11;  //定义红外接收器的引脚为11
IRrecv irrecv(RECV_PIN);  
decode_results results;   

int enablepin=10;//H桥的使能端
int in1a=4;//H桥的两个控制引脚
int in2a=7;

int t[4]={};
//int n=0;
int curtainState = 0; //窗帘初始为拉开
int i=0,j=0;
int houra=0,minu=0,sec=0;
int inputhour=0,inputminu=0;
int setopenhour=8,setopenminu=0;
int setclosehour=20,setcloseminu=0;
void setup() {
  pinMode(enablepin,OUTPUT);
  pinMode(in1a,OUTPUT);
  pinMode(in2a,OUTPUT);
  digitalWrite(enablepin,LOW);
  irrecv.enableIRIn(); // 初始化红外接收器
  Serial.begin(9600);
  boolean a=false;
  do{
    a=inita();
    delay(3000);
  }while(!a);
  runtime();
  
}

void loop() {
  runtime();
  delay(1000);
  btinsert();
  if(irrecv.decode(&results)){
   // Serial.print("shu ru zhi=");
   // Serial.println(results.value);
    if(((results.value==16756815)||(results.value==16750695))){
     // Serial.print("fu he yao qiu =");
     // Serial.println(results.value);
      insert(results.value);
    }
    if(results.value==16712445){//现在打开窗帘
      if(curtainState==1){//如果窗帘是关着的
        digitalWrite(in1a,HIGH);
        digitalWrite(in2a,LOW);
        digitalWrite(enablepin,HIGH); 
        Serial.println("it's morning, opening the curtain");//正转拉开窗帘
        delay(4000);
        digitalWrite(enablepin,LOW);
        curtainState=0;
        Serial.println("curtain is opened, enjoy your day!");
      }
    }
    if(results.value==16761405){
      if(curtainState==0){
        digitalWrite(in1a,LOW);
        digitalWrite(in2a,HIGH);
        digitalWrite(enablepin,HIGH);
        Serial.println("it's time for bed, closing the curtain");
        delay(4000);
        digitalWrite(enablepin,LOW);
        curtainState = 1; //窗帘拉上    
        Serial.println("curtain is colsed, good night!");
      }
    }
    irrecv.resume(); // 接收下一个值
  }
  check();
}

void check(){
  //晚上了且窗帘没拉
  if (houra==setclosehour && minu==setcloseminu && curtainState==0)    
  {  
    digitalWrite(in1a,LOW);
    digitalWrite(in2a,HIGH);
    digitalWrite(enablepin,HIGH);
    Serial.println("it's time for bed, closing the curtain");
    delay(4000);
    digitalWrite(enablepin,LOW);
    curtainState = 1; //窗帘拉上    
  //  Serial.print("curtain state = ");
    Serial.println("curtain is colsed, good night!");
    } 
    // 早晨了且窗帘拉上     
    if (houra==setopenhour && minu==setopenminu &&curtainState == 1)
   {
    digitalWrite(in1a,HIGH);
    digitalWrite(in2a,LOW);
    digitalWrite(enablepin,HIGH);   
    Serial.println("it's morning, opening the curtain");//正转拉开窗帘
    delay(4000);
    digitalWrite(enablepin,LOW);
    curtainState=0;
    //Serial.print("curtain state = ");
    Serial.println("curtain is opened, enjoy your day!");
    }
}
boolean match(int houra,int minu){
  if(houra>24||minu>60){
    Serial.println("the time is wrong, please input again.");
    return false;
  }
  else{
    return true;
  }
}
void insert(long unsigned int a){   //设置开关窗帘的时间
  if(!a){
    Serial.println("please reenter the time to open or close the curtain in 4 numbers, then prase the kaiguan to end");
    irrecv.resume(); // 接收下一个值
  //  return false;
  }
  else{
    if(a==16750695){//拉开窗帘
      Serial.println("please input the time to open the curtain.");
      do{
        gettime();
        setopenhour=t[0]*10+t[1];
        setopenminu=t[2]*10+t[3];
      }while(!match(setopenhour,setopenminu));
      Serial.print("set done. "); 
      Serial.print("the time to open the curtain is ");
      printDigits(setopenhour);
      Serial.print(":");
      printDigits(setopenminu);
      Serial.println();
      delay(3000);
      irrecv.resume(); // 接收下一个值
    }
    if(a==16756815){
      Serial.println("please input the time to close the curtain.");
      do{
        gettime();
        setclosehour=t[0]*10+t[1];
        setcloseminu=t[2]*10+t[3];
      }while(!match(setclosehour,setcloseminu));
      Serial.print("set done. "); 
      Serial.print("the time to close the curtain is ");
      printDigits(setclosehour);
      Serial.print(":");
      printDigits(setcloseminu);
      Serial.println();
*      delay(3000);
      irrecv.resume(); // 接收下一个值
    }
  }
}
void gettime(){
  for(int i=0;i<4;i++){
        t[i]=recv_num(irinput());
        do{
        t[i]=recv_num(irinput());
        delay(1000);
       }while(t[i]==-1);
       irrecv.resume(); // 接收下一个值
    }
}
boolean inita(){   //初始化函数，输入初始时间
  if((!irrecv.decode(&results))&(!Serial.available())){ //如何判断蓝牙发送了数据？用Seria.available()!!!
    Serial.println("please enter the current hour and minute in 4 numbers, then prase the switch to end");
    return false;
  }

    if(irrecv.decode(&results)){
      do{
        for(int i=0;i<4;i++){
          t[i]=recv_num(irinput());
          do{
            t[i]=recv_num(irinput());
            delay(500);
          }while(t[i]==-1);
          irrecv.resume(); // 接收下一个值
        }
        Serial.print("set done. "); 
        inputhour=t[0]*10+t[1];
        inputminu=t[2]*10+t[3];
      }while(!match(inputhour,inputminu));
      houra=inputhour;
      minu=inputminu;
      sec=0;
      printnow();
      return true;
    }
    if(Serial.available()){
      int a=0,n=0;
      do{
        do{
          a=Serial.read();
          for(int i=48;i<58;i++){
            if(a==i){
            Serial.println(i-48);//将ASCII码转换为数字
            t[n]=i-48;
            n++; 
            }
          }
        }while(n<4);
        if(n==4){
          n=0;
          inputhour=t[0]*10+t[1];
          inputminu=t[2]*10+t[3];  }    
      }while(!match(inputhour,inputminu));
      Serial.print("set done. ");   
      houra=inputhour;
      minu=inputminu;
      sec=0;
      printnow();
      return true;
    }
}
long unsigned int irinput(){
  if (irrecv.decode(&results)){
     long unsigned int n=results.value;//只要一将results.value赋值给int就出错,因为.value是long unsigned int 型！！
        delay(2000);
        irrecv.resume(); // 接收下一个值
        return n;
  }
}

long unsigned int recv_num(long unsigned int v){
  String a=(String)v;//&(v>16700000)&(v<16799999)
  if((a.length()!=8)){
    return -1;
  }else{
    if(v==16712445){
      Serial.println("yes");  
    }
     if(v==16769055){
      Serial.println("delete");  
    }
    if(v==16738455){
      Serial.println("0");  
      return 0;
    }
    if(v==16724175){
      Serial.println("1");
      return 1;  
    }
    if(v==16718055){
      Serial.println("2");  
      return 2;
    }
    if(v==16743045){
      Serial.println("3");  
    return 3;
    }
    if(v==16716015){
      Serial.println("4");  
    return 4;
    }
    if(v==16726215){
      Serial.println("5");  
    return 5;
    }
    if(v==16734885){
      Serial.println("6");  
    return 6;
    }
    if(v==16728765){
      Serial.println("7");  
    return 7;
    }
    if(v==16730805){
      Serial.println("8");  
    return 8;
    }
    if(v==16732845){
      Serial.println("9");  
    return 9;
    }
  }
}
/*
int ascchange(int a){
   int b;
   for(int i=48;i<58;i++){
    if(a==i){
      Serial.println(i-48);//将ASCII码转换为数字
      b=i-48;
      return b;
      }
   }
}*/
void btinsert(){
  while(Serial.available()){
    int a=Serial.read();
    //Serial.println(a);
    if(a==99){  //close的首字母c的ASCII码是99
      Serial.println("please input the time to close the curtain:");
      do{
        do{
          a=Serial.read();
          for(int i=48;i<58;i++){
            if(a==i){
              Serial.println(i-48);//将ASCII码转换为数字
              t[n]=i-48;
              n++; 
            }
          }
        }while(n<4);
        if(n==4){
          n=0;
          setclosehour=t[0]*10+t[1];
          setcloseminu=t[2]*10+t[3];}
      }while(!match(setclosehour,setcloseminu));
      Serial.print("Set done. The time to close the curtain is ");
      for(int j=0;j<2;j++){
        Serial.print(t[j]);
      }
      Serial.print(":");
      for(int j=2;j<4;j++){
        Serial.print(t[j]);
      }
      Serial.println();
      delay(3000);
    }
        if(a==111){     //o的ASCII码
        Serial.println("please input the time to open the curtain:");
        do{
          do{
            a=Serial.read();
            for(int i=48;i<58;i++){
              if(a==i){
                Serial.println(i-48);//将ASCII码转换为数字
                t[n]=i-48;
                n++; 
              }
            }
          }while(n<4);
          if(n==4){
            n=0;
            setopenhour=t[0]*10+t[1];
            setopenminu=t[2]*10+t[3];
          }
        }while(!match(setopenhour,setopenminu));
        Serial.print("Set done. The time to open the curtain is ");
        for(int j=0;j<2;j++){
            Serial.print(t[j]);
        }
        Serial.print(":");
        for(int j=2;j<4;j++){
          Serial.print(t[j]);
        }
        Serial.println();
        delay(3000);
        }  
      }
}  
void runtime(){
  sec++;
  if(sec==60){
    sec=0;
    minu++;
  }
  if(minu==60){
    minu=0;
    houra++;
  }
  if(houra==24){
    houra=0;
  }
  printnow();
}
void printDigits(long unsigned int digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
void printnow(){ 
  Serial.print("now is ");
  printDigits(houra);
  Serial.print(":");
  printDigits(minu);
  Serial.print(":");
  printDigits(sec); 
  Serial.println(); 
}
