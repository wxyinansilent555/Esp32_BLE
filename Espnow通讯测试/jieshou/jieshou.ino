  //接收端  
    #include <esp_now.h>
    #include <WiFi.h>
    #define TX 10
    typedef struct struct_message 
    {
        char a[32];
        int b;
    } struct_message;

    struct_message myData;

    void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)//接收数据时执行的回调函数
    {
      memcpy(&myData, incomingData, sizeof(myData));//我们将接收数据，都存储在结构体的变量中
      Serial.print("接收的字节: ");
      Serial.println(len);
      Serial.print("字符串: ");
      Serial.println(myData.a);
      Serial.print("整数: ");
      Serial.println(myData.b);
      Serial.println();
    }
     
    void setup() 
    {
      Serial.begin(9600);
      digitalWrite(TX,LOW);
      WiFi.mode(WIFI_STA);

      if (esp_now_init() != ESP_OK)
      {
        Serial.println("初始化ESPNOW出错！");
        return;
      }
      
      esp_now_register_recv_cb(OnDataRecv);
    }
     
    void loop()
    {
        Serial.println(myData.a);
        delay(2000);
    }
