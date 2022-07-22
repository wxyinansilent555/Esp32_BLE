//这是发送端的程序，请下载到板1上
#include <BLEDevice.h>
#include <BLE2902.h>
#include <time.h>

//传感器引脚可以再设置
#define minitor_1 13
#define minitor_2 34
#define minitor_3 35

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

float C_recv,A_recv,B_recv;               //三个接收模块
float recv1,recv2,recv3;
char data_sent[6];
float t;
int flag,clo,shifou=0;
int ss = 123;

clock_t start  = 0 ;
clock_t ending = 0 ;
clock_t time1;

unsigned long previousMillis = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {};
    void onDisconnect(BLEServer* pServer) {
      pServer->startAdvertising(); // 如果客户端断开连接了就重新开启Advertising广播，使客户端可以再次搜索到自己
    }
};

BLECharacteristic* pCharacteristic = NULL;

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

void setup() {
  Serial.begin(115200);
  Serial.println();

  BLEDevice::init("ESP32-BLE");
  BLEServer *pServer = BLEDevice::createServer(); // 创建服务器
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);  // 创建服务
  pCharacteristic = pService->createCharacteristic(
                                                  CHARACTERISTIC_UUID, 
                                                  BLECharacteristic::PROPERTY_READ   |
                                                  BLECharacteristic::PROPERTY_WRITE  |
                                                  BLECharacteristic::PROPERTY_NOTIFY |
                                                  BLECharacteristic::PROPERTY_INDICATE
                                                  );
  pCharacteristic->setValue("Hello World! ");
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start(); 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); // 广播服务的UUID
  BLEDevice::startAdvertising();
}

void loop() 
{    
    //t_work();
    pCharacteristic->setValue(ss);
    pCharacteristic->notify(); // 每隔interval时间主动推送一次数据
    delay(2000);
}

