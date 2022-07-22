#include <BLEDevice.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

boolean doSacn = true;
boolean doConnect = false;
boolean connected = false;

BLEAdvertisedDevice* pServer;
BLERemoteCharacteristic* pRemoteCharacteristic;

// 搜索到设备时回调功能
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b"))) {
      if (advertisedDevice.haveName() && (advertisedDevice.getName()=="ESP32-BLE")) {
        advertisedDevice.getScan()->stop(); // 停止当前扫描
        pServer = new BLEAdvertisedDevice(advertisedDevice); // 暂存设备
        doSacn = false;
        doConnect = true;
        Serial.println("发现想要连接的设备");
      }
    }
};

// 客户端与服务器连接与断开回调功能
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {}
  void onDisconnect(BLEClient* pclient) {
    doSacn = true;
    connected = false;
    Serial.println("失去与设备的连接");
  }
};

// 收到服务推送的数据时的回调函数
void NotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    char buf[length + 1];
    for (size_t i = 0; i < length; i++) {
      buf[i] = pData[i];
    }
    buf[length] = 0;
    Serial.printf("该消息长度为: %d; 内容为: %s\r\n", length, buf);
    
}

// 用来连接设备获取其中的服务与特征
bool ConnectToServer(void) {
    BLEClient* pClient  = BLEDevice::createClient(); // 创建客户端
    pClient->setClientCallbacks(new MyClientCallback()); // 添加客户端与服务器连接与断开回调功能
    if (!pClient->connect(pServer)) { // 尝试连接设备
      return false;
    }
    Serial.println("连接设备成功");

    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID); // 尝试获取设备中的服务
    if (pRemoteService == nullptr) {
      Serial.println("获取服务失败");
      pClient->disconnect();
      return false;
    }
    Serial.println("获取服务成功");

    pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID); // 尝试获取服务中的特征
    if (pRemoteCharacteristic == nullptr) {
      Serial.println("获取特性失败");
      pClient->disconnect();
      return false;
    }
    Serial.println("获取特征成功");

    if(pRemoteCharacteristic->canRead()) { // 如果特征值可以读取则读取数据
      Serial.printf("该特征值可以读取并且当前值为: %s\r\n", pRemoteCharacteristic->readValue().c_str());
    }
    if(pRemoteCharacteristic->canNotify()) { // 如果特征值启用了推送则添加推送接收处理
      pRemoteCharacteristic->registerForNotify(NotifyCallback);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); 
  pBLEScan->setActiveScan(true); 
  pBLEScan->setInterval(100); 
  pBLEScan->setWindow(80); 
}

void loop() {
  // 如果需要扫描则进行扫描
  if (doSacn) {
    Serial.println("开始搜索设备");
    BLEDevice::getScan()->clearResults();
    BLEDevice::getScan()->start(0); // 持续搜索设备
  }
  // 如果找到设备就尝试连接设备
  if (doConnect) {
    if (ConnectToServer()) { 
      connected = true;
    }
    else {
      doSacn = true;
    }
    doConnect = false;
  }
}

