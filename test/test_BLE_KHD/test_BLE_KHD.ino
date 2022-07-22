//ESP32 BLE客户端模式
#include "BLEDevice.h"//低功耗蓝牙驱动函数库
#include <Wire.h>
#include "SSD1306.h" 
SSD1306  display(0x3c, 23, 22);//SCL 22 SDA 23
#define bleServerName "BLE_time_difference"//定义要连接的BLE服务器的名称
static BLEUUID TIMEServiceUUID("5c094f55-89a3-4f74-b0c7-fa4044d13081");//服务的UUID，我们想要读取的特征和我们想要写入的特性。
static BLEUUID TIMECharacteristicUUID("22d89905-f348-48a5-b3a9-e4347cddfabf");//电压特征的UUID

static boolean doConnect = false;//连接的标识
static boolean connected = false;//启动的标识

static BLEAddress *pServerAddress;//BLE服务器地址，外围设备的地址，地址将在扫描过程中找到...|
static BLERemoteCharacteristic* TIMECharacteristic;//我们想读的特征和我们想写的特征。
const uint8_t notificationOn[] = {0x1, 0x0};//激活打开
const uint8_t notificationOff[] = {0x0, 0x0};//通知关闭

char* TIME;//存储时间差变量
boolean newTIME = false;//检查新的电压读数是否可用的标志

bool connectToServer(BLEAddress pAddress)
{//连接到名称服务和特征一致的BLE服务器
  BLEClient* pClient = BLEDevice::createClient();//BLE创建客户端
  pClient->connect(pAddress);//连接到BLE服务器。
  BLERemoteService* pRemoteService = pClient->getService(TIMEServiceUUID);//在远程BLE服务器中获取对我们所需要的服务的引用
  if(pRemoteService == nullptr)
  {
  return(false);//未能连接到我们的BLE服务器
  }

  TIMECharacteristic = pRemoteService->getCharacteristic(TIMECharacteristicUUID);//获取远程BLE服务器服务中特征属性

  if (TIMECharacteristic ==nullptr)
  {//判断我们找到我们的特征值
    return false;//未能找到
  }
  
  //为特征分配调用函数
  TIMECharacteristic->registerForNotify(TIMENotifyCallback);//特征注册通知
  return true;
}

class MyAdvertisedDeviceCallbacks:public BLEAdvertisedDeviceCallbacks
{//当收到另一个设备的广播时，调用广播
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {//BLE广播函数
    if(advertisedDevice.getName()== bleServerName
    ){//检查广告客户的名称是否匹配
      advertisedDevice.getScan()->stop();//可以停止扫描，我们找到了我们要找的BLE蓝牙服务器
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());//BLE广播的地址是我们需要的
      doConnect = true;//设置指示器，说明我们已准备好连接
      Serial.println("找到设备已连接!");
    }
  }
};
//获取服务器的BLE特征通知
static void TIMENotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData, size_t length, bool isNotify){
  TIME =(char*)pData;//存储时间值
  newTIME = true;//将时间差值设置成"真"
}

void UARTprint()
{//串口信息输出函数，我们主要用串口监视器查看。
  display.clear();
  display.drawString(0,15,String(TIME));
  display.display();
}
void setup()
{
  //Serial.begin(115200);//打开串口波特率设置为115200
  BLEDevice::init("");//初始化BLE客户端设备
  BLEScan* pBLEScan = BLEDevice::getScan();
  //检索扫描器并设置我们想要使用的回调，以便在我们
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());//设置广播并调用“广播类”
  pBLEScan->setActiveScan(true);//BLE设置成主动扫描
  pBLEScan->start(30);//扫描运行30秒。
  display.init();
  
}

void loop(){
  if (doConnect == true)
  { //如果标志"doConnect”为真，那么我们已经扫描并找到了所需的BLE服务器
    if(connectToServer(*pServerAddress))
    {//连接到服务器地址
      //Serial.println("我们现在已连接到BLE服务器");
      display.drawString(0,0,"Connected!");
      TIMECharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);//启用时间特征的通知属性
      connected = true;//一旦我们连接BLED服务器，我们将连接标志设置为“真”（false）
    } 
    else
    {
      //Serial.println("我们无法连接到服务器，重新启动您的设备以再次扫描附近的BLE服务器");
      display.drawString(0,0,"NOconnected");
    }
    doConnect = false;//我们将"doconnect"连接标志设置为“假”(false)。
  }
  if(newTIME)
  {//如果有新的读数可用，串口输出信息
    newTIME = false;//将时间差标识设置为“假”
    UARTprint();//调用串口信息输出函数
  }
  //delay(200);//在循环之间延迟。
  
}
