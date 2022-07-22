    #include <esp_now.h>
    #include <time.h>
    #include <Arduino.h>
    #include "BluetoothSerial.h"

    //传感器引脚可以再设置
    #define minitor_1 13
    #define minitor_2 34
    #define minitor_3 35

    #define Master 1    //主从机模式选择 1主机 0从机


    float C_recv,A_recv,B_recv;               //三个接收模块
    float recv1,recv2,recv3;
    char data_sent[6];
    float t;
    int flag = 0;
    int clo;
    int shifou=0;
    clock_t start=0;   
    clock_t ending=0;   
    clock_t time1;
    BluetoothSerial SerialBT;

    // 2号MAC接收地址
    //数据读取
    void Bluetooth_Event(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    uint8_t address[6]={0x7C,0x9E,0xBD,0x47,0x69,0x22}; 
    
    //蓝牙事件回调函数
    void Bluetooth_Event(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)  
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
            while(SerialBT.available())
            {
               Serial.write(SerialBT.read());
            }
            Serial.write("  receive complete! \r\n");
        }
        else if(event == ESP_SPP_WRITE_EVT)     //数据发送标志
        {
            Serial.write("  send complete! \r\n");
         }
}

    void time_open()
    {
      start = clock();
    }

    void time_close()
    {
      ending = clock();
    }
    

  int t_work(int flag)
  {
    read_sensor();
                                                         //可能存在问题，上次喇叭的声音或杂音有干扰，需要在ABC三个接收模块都清零后进行下一步
    if(flag == 0)//只记录BC的信号时间差
    {
      if( A_recv == 0 && B_recv == 1)
    {
      time_open();
q:    read_sensor();      
      if( A_recv == 1 && B_recv == 1)
      {
         time_close();
         t = ending - start;
         clo = 1;                     //接近B端
         return t;
      }
      delay(2);
      goto q;
     }
      if( A_recv == 1 && B_recv == 0)
    {
      time_open();
w:    read_sensor();
      if( A_recv == 1 && B_recv == 1)
      {
         time_close();
         t = ending - start;
         clo = 0;                     //接近A端
         return t;
      }
      delay(2);
      goto w;
     }     
    } 
    else//只记录AB的信号时间差
    {
      if( C_recv == 0 && A_recv == 1)
    {
      time_open();
e:    read_sensor();
      if( A_recv == 1 && C_recv == 1)
      {
         time_close();
         t = ending - start;
         clo = 0;                       //接近A端
         return t;
      }
      delay(2);
      goto e;
     }
      if( C_recv == 1 && A_recv == 0)
    {
      time_open();
r:    read_sensor();
      if( A_recv == 1 && C_recv == 1)
      {
         time_close();
         t = ending - start;
         clo = 1;                       //接近C端
         return t;
      }
      delay(2);
      goto r;
     }     
    }
  }

   void read_sensor()
   {
    //读取传感器数值
    C_recv = digitalRead(minitor_1);
    A_recv = digitalRead(minitor_2);
    B_recv = digitalRead(minitor_3);
   }
     
    void setup() 
    {
      Serial.begin(115200);

      pinMode(minitor_1,INPUT);
      pinMode(minitor_2,INPUT);
      pinMode(minitor_3,INPUT);
      
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
          test();
      }
      }
      
    
     void loop()
     {
      
     }
    void test() 
    {
      /*float ttt;                                                                    //用作测试 
      C_recv = digitalRead(minitor_1);
      A_recv = digitalRead(minitor_2);
      B_recv = digitalRead(minitor_3);      
      if(( A_recv == 1 && B_recv == 0)||( A_recv == 0 && B_recv == 1))
      {
        ttt=t_work(0);
      }
      
      Serial.print(A_recv);
      Serial.print(" ");
      Serial.print(B_recv);
      Serial.print(" ");
      Serial.print(start);
      Serial.print(" ");      
      Serial.print(ending);
      Serial.print(" ");
      Serial.println(ttt);
      delay(100);*/

      SerialBT.write(',');
      SerialBT.write(123);
      SerialBT.write(',');
      SerialBT.write(444);
      SerialBT.write(',');
      SerialBT.write(3);
      SerialBT.write(',');
      SerialBT.write(6);
      delay(100);
      /*t_work(flag);
      SerialBT.write(t);
      SerialBT.write(',');
      SerialBT.write(clo);
      SerialBT.write(',');
      SerialBT.write(flag);
      SerialBT.write(',');
      SerialBT.write(shifou);
      SerialBT.write(',');
      if(shifou==0)
      {
        if(t<=20)
        {
            flag++ ; 
            shifou++;
        }
      }
      else if(shifou==1)
      {
        for(int i=0;i<=5;i++)
        {
          if(i<=4)
          {
            flag=1;
          }
          else if(i==5)
          {
            flag=0;
          }
        }
      }
      delayMicroseconds(20);*/
    }
