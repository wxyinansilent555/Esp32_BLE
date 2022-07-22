    #include <Arduino.h>
    #include <WiFi.h>
    #include <time.h>
    #include "BluetoothSerial.h"

    //引脚根据实际情况修改
    #define left_motor_1     33                             //IN1
    #define left_motor_2     25                             //IN2
    #define right_motor_1    26                             //IN3
    #define right_motor_2    27                             //IN4
    #define ENA              32                             //ENA
    #define ENB              14                             //ENB
    
    int flag = 0;
    int receive_message;

    #define Master 0    //主从机模式选择 1主机 0从机

    void Bluetooth_Event(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    uint8_t address[6]={0x7C,0x9E,0xBD,0x47,0x69,0x22}; 

    BluetoothSerial SerialBT;

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
        while(SerialBT.available())
        {
            Serial.write(SerialBT.read());
            receive_message = SerialBT.read();
        }
        Serial.write("  receive complete! \r\n");
    }
    else if(event == ESP_SPP_WRITE_EVT)     //数据发送标志
    {
        Serial.write("  send complete! \r\n");
    }
}

    void car_stop(int t)
    {
      //停止函数
    }

    void car_left()
    {
      //左转函数
    }

    void car_right()
    {
      //右转函数
    }
     
    void setup() 
    {
      Serial.begin(115200);

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
      if(receive_message == 'S')
      {
        flag ++;
        if(flag = 1)
        {
          car_stop(5);
          car_left();
        }
        else
        {
          car_stop(10000);
        }
        
      }
    }
