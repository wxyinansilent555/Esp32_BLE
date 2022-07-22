    #include <Arduino.h>
    #include <WiFi.h>
    #include <time.h>
    #include "BluetoothSerial.h"
    #include <String.h>
    #include<FreeRTOS.h>        //多线程库
    /* 创建任务一和任务二的句柄，并初始化 */
    TaskHandle_t TASK_HandleOne = NULL;
    //引脚根据实际情况修改
    #define sound             5                             //喇叭
    #define LED               2                             //LED
    #define left_motor_1      33                            //IN1
    #define left_motor_2      25                            //IN2
    #define right_motor_1     26                            //IN3
    #define right_motor_2     27                            //IN4
    #define ENA               32                            //ENA
    #define ENB               14                            //ENB
    String comdata = "";                                    //转化串口数值
    int numdata[6] = {0};
    int mark = 0;
    

    int t1=20;                                              //方便调参
    int t2=10;
    float pi=0.05;
    #define Master 0    //主从机模式选择 1主机 0从机

    void Bluetooth_Event(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    uint8_t address[6]={0x7C,0x9E,0xBD,0x47,0x69,0x22}; 

    BluetoothSerial SerialBT;
    /*
void function(char cho)
{
  switch(cho)
  {
    case 'S':
      ledcWrite(0,0);
      ledcWrite(1,0);
      for(int i=0;i<=7;i++)
      {
        digitalWrite(LED,HIGH);            //停车且伴有明显的声光指示
        ledcWriteTone(6,1800);
        delay(500);
        digitalWrite(LED,LOW);
                    
        delay(500);
      }
      ledcWriteTone(6,0);
      break;
    case 'L':                              //左转
      digitalWrite(left_motor_1,HIGH);
      digitalWrite(left_motor_2,LOW);
      ledcWrite(0,1750);
      ledcWrite(1,1750);
      delay(470);
      digitalWrite(left_motor_1,LOW);
      digitalWrite(left_motor_2,HIGH);
      break;
    case 'A':                              //掉头
      digitalWrite(left_motor_1,HIGH);
      digitalWrite(left_motor_2,LOW);
      ledcWrite(0,1750);
      ledcWrite(1,1750);
      delay(880);
      digitalWrite(left_motor_1,LOW);
      digitalWrite(left_motor_2,HIGH);
      break;
    case 'B':
      digitalWrite(left_motor_1,HIGH);
      digitalWrite(left_motor_2,LOW);
      digitalWrite(right_motor_1,HIGH);
      digitalWrite(right_motor_2,LOW);
      ledcWrite(0,1700);
      ledcWrite(1,1700);
      delay(80);
      digitalWrite(left_motor_1,LOW);
      digitalWrite(left_motor_2,HIGH);
      digitalWrite(right_motor_1,LOW);
      digitalWrite(right_motor_2,HIGH);
      ledcWrite(0,0);
      ledcWrite(1,0);
      break;
    
  }
  
}

    void judge()                     //numdata[0]==t;  numdata[1]==clo;  numdata[2]==flag;  numdata[3]==shifou 
    {
      if(numdata[3]==0)
      {
        if(numdata[1]==0 && numdata[2]==0)
        {
          do{
            delay(3);
          }while(numdata[0]>=t1);
        
        }
        else if(numdata[1]==1 && numdata[2]==0)
        {
          do
          {
            function('B');
          }while(numdata[0]>=t2);
        }          
      }
      else if(numdata[3]==1)
      {
        function('S');
        function('L');
        if(numdata[1]==0 && numdata[2]==1)
        {
          ledcWrite(0,1700);
          ledcWrite(1,1700);
        }
        else if(numdata[1]==1 && numdata[2]==1)
        {
          do
          {
            function('B');
          }while(numdata[0]>=t2);
        }  
        else if(numdata[2]==0)
        {
          if(numdata[1]==0&&numdata[0]>=t2)
          {
            ledcWrite(1,1700+numdata[0]*pi);
            delay(60);
            ledcWrite(1,1700);
          }
          else if(numdata[1]==1&&numdata[0]>=t2)
          {
            ledcWrite(0,1700+numdata[0]*pi);
            delay(60);
            ledcWrite(0,1700);
          }
        }
        if(numdata[0]<=t1 && numdata[2]==1)
        {
          function('S');
          function('S');
          function('S');
        }
      }
    }
    */

void Bluetooth_Event(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)  //蓝牙事件回调函数
{
    if(event == ESP_SPP_OPEN_EVT || event == ESP_SPP_SRV_OPEN_EVT) //蓝牙连接成功标志 
    {                                                              //蓝牙主机和从机模式对应的标志不同，前面的是主机模式的，后面是从机模式
        Serial.write("connection successful!\r\n");
    }
    else if(event == ESP_SPP_CLOSE_EVT)     //蓝牙断开连接标志
    {
        Serial.write("disconnect successful!\r\n");
    }
    else if(event == ESP_SPP_DATA_IND_EVT)  //数据接收标志
    {
       //j是分拆之后数字数组的位置记数
       int j = 0;
     
       //不断循环检测串口缓存，一个个读入字符串，
       while (Serial.available() > 0)
       {
          //读入之后将字符串，串接到comdata上面。
          comdata += char(Serial.read());
          delay(2);
          mark = 1;
        }
     
       if(mark == 1)  
       {
          Serial.print("comdata:");
          Serial.println(comdata);
          Serial.print("comdata.length():");
          Serial.println(comdata.length());
     
         for(int i = 0; i < comdata.length() ; i++)
         {
            if(comdata[i] == ',')
           {
             j++;
           }
           else
           {
              numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
           }
          }
          Serial.println("收到数据为:");
          Serial.println(numdata[0]);
          Serial.println(numdata[1]);
          Serial.println(numdata[2]);
          Serial.println(numdata[3]);
          
          comdata = String("");    
          delay(300);
        }
        mark = 0;    
        Serial.write("  receive complete! \r\n");
    }
    else if(event == ESP_SPP_WRITE_EVT)     //数据发送标志
    {
        Serial.write("  send complete! \r\n");
    }
}


     
    void setup() 
    {
      Serial.begin(115200);
      xTaskCreate(
                    TASK_ONE,          /* 任务函数 */
                    "TaskOne",         /* 任务名 */
                    8*1024,            /* 任务栈大小，根据需要自行设置*/
                    NULL,              /* 参数，入参为空 */
                    1,                 /* 优先级 */
                    &TASK_HandleOne);  /* 任务句柄 */
      SerialBT.register_callback(Bluetooth_Event); 

      if(Master)
      {
          SerialBT.begin("ESP32_MASTER",true); //开启蓝牙 名称为:"ESP32_MASTER" 主机
          Serial.printf("Init Successful - Master\r\n");
          SerialBT.connect(address);
          Serial.printf("Connect Successful\r\n");
      }
      else
      {
          SerialBT.begin("ESP32_SLAVE");       //开启蓝牙 名称为:"ESP32_SLAVE" 从机
          Serial.printf("Init Successful - Slave\r\n");
      }
      
      // 初始化串口
      
      ledcSetup(0, 5000, 10);               //通道0， 5KHz，10位解析度
      ledcSetup(1, 5000, 10);               //通道1， 5KHz，10位解析度
      pinMode(sound,OUTPUT);  
      ledcAttachPin(sound,6);
      ledcSetup(6,1200,8);                  //喇叭定义
      pinMode(LED,OUTPUT);                  //LED定义      
      pinMode(left_motor_1,OUTPUT);
      pinMode(right_motor_1,OUTPUT);
      pinMode(left_motor_2,OUTPUT);
      pinMode(right_motor_2,OUTPUT);
      pinMode(ENA,OUTPUT);
      pinMode(ENB,OUTPUT);
      digitalWrite(left_motor_1,LOW);
      digitalWrite(left_motor_2,HIGH);
      digitalWrite(right_motor_1,LOW);
      digitalWrite(right_motor_2,HIGH);  
      ledcAttachPin(ENA, 0);
      ledcAttachPin(ENB, 1);
      
    }
     
    void loop() 
    {
      /*ledcWrite(0,1700);
      ledcWrite(0,1700);
      judge();*/
      
      
      
      
      

    }
/* 任务一的函数体,由于入参即为NULL，因此函数体的入参需为void *参数，否则报错 */
void TASK_ONE(void *param )
{
    /*// 打印来自任务1的信息 
    for( int i = 0;i<10;i++ )
    {
        Serial.println("Hello from TaskOne~");
        delay(1000);
    }
    // 打印10次后通过任务一的句柄退出任务，并释放资源 
    Serial.println("Ending TaskOne!");
    vTaskDelete( TASK_HandleOne );           //退出任务*/
    /*ledcWriteTone(6,1800);                 //喇叭间断响
    delay(15);
    ledcWriteTone(6,0);
    delay(15);*/
 
}
