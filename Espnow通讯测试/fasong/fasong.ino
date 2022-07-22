    #include <esp_now.h>
    #include <WiFi.h>

    //发送端MAC地址
    uint8_t broadcastAddress[] = {0x7C,0x9E,0xBD,0x48,0xEC,0xC4};//7C:9E:BD:48:EC:C6

    //存储的结构体变量
    typedef struct struct_message
    {
      char a[32];//字符数组变量
      int b;//时间数组变量
    } struct_message;

    struct_message myData;//创建一个结构体的名称

    void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
      //数据发送时回调
      Serial.print("\r\n最后一个是数据包发送状态：\t");
      Serial.println(status == ESP_NOW_SEND_SUCCESS ? "传送成功" : "传送失败");
    }
     
    void setup()
    {
      Serial.begin(9600);
     //串口波特率115200
      WiFi.mode(WIFI_STA);
     //WiFi站模式
      if (esp_now_init() != ESP_OK)
      {
        //初始化ESPNOW
        Serial.println("初始化ESP-NOW出错");
        return;
        //返回错误检查
      }

      esp_now_register_send_cb(OnDataSent);//ESPNOW成功初始化，我们将注册发送包，获取已经发送的数据包的状态
      
      esp_now_peer_info_t peerInfo;//注册对等点
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);//对端点设备地址，我们将其设置为广播地址
      peerInfo.channel = 0;  //对端点用于发送wifi数据通道我们设置为0
      peerInfo.encrypt = false;//不加密
      if (esp_now_add_peer(&peerInfo) != ESP_OK){
        //添加对端点到列表中
        Serial.println("添加到对端点失败");
        return;//返回一个esp_err_t类型的值，我们将使用该值进行错误检查
        }
    }
     
    void loop() {
      strcpy(myData.a, "我是发送端的板子");
      //设置要发送的值
      myData.b = random(1,20);
      //返回一个esp_err_t的值，存储在一个变量里进行错误检查
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
       //sizeof返回占用的字节数
      if (result == ESP_OK) {
        Serial.println("发送成功");
      }
      else {
        Serial.println("发送失败");
      }
      delay(2000);//延时2s
    }
