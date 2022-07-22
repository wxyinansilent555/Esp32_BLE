    #include <esp_now.h>
    #include <time.h>
    #include <Arduino.h>
    #include "BluetoothSerial.h"

    //传感器引脚可以再设置
    #define minitor_1 13
    #define minitor_2 12
    #define minitor_3 14

    #define Master 1    //主从机模式选择 1主机 0从机

    float fi_recv,se_recv,th_recv;
    float recv1,recv2,recv3;
    char data_sent[6];
    int t;
    int flag = 0;

    clock_t start,ending;   

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
    if(flag == 0)//只记录BC的信号时间差
    {
      if( se_recv == 0 & th_recv == 1)
    {
      time_open();
      if( se_recv == 1 & th_recv == 1)
      {
         time_close();
         t = ending - start;
         return t;
      }
     }
    } 
    else//只记录AB的信号时间差
    {
      if( fi_recv == 0 & se_recv == 1)
    {
      time_open();
      if( se_recv == 1 & fi_recv == 1)
      {
         time_close();
         t = ending - start;
         return t;
      }
     }
    }
   }

   void read_sensor()
   {
    //读取传感器数值
    fi_recv = digitalRead(minitor_1);
    se_recv = digitalRead(minitor_2);
    th_recv = digitalRead(minitor_3);
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
      }
      }
      
    
     
    void loop() 
    {
      t_work(flag);
      if(flag <2)
      {
        if(t==0) //小于多少时，falg+1
        {
          flag = flag + 1;

          if(Master ==1  & flag == 1)
          {
             SerialBT.write('S');  //发送S说明需要停止运动          
          }
         delay(5);
        }
      }
      else
      {
        delay(1000000);//超出两次就不再检测
      }
      delayMicroseconds(20);
    }
