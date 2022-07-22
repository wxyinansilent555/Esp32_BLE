#include<FreeRTOS.h>

/* 创建任务一和任务二的句柄，并初始化 */
TaskHandle_t TASK_HandleOne = NULL;
TaskHandle_t TASK_HandleTwo = NULL;
 
void setup() {
  Serial.begin(115200);
  delay(1000);
  int num = 0;
  xTaskCreate(
                    TASK_ONE,          /* 任务函数 */
                    "TaskOne",         /* 任务名 */
                    8*1024,            /* 任务栈大小，根据需要自行设置*/
                    NULL,              /* 参数，入参为空 */
                    1,                 /* 优先级 */
                    &TASK_HandleOne);  /* 任务句柄 */
 
  xTaskCreate(
                    TASK_TWO,          /* 任务函数 */
                    "TaskTwo",         /* 任务名 */
                    8*1024,            /* 任务栈大小*/
                    (void *)&num,      /* 参数为变量，传递入参时,需传递该变量的地址并强转为void *型，否则无法传递，编译报错 */
                    1,                 /* 优先级 */
                    &TASK_HandleTwo);  /* 任务句柄 */
 
}

void loop() {
  delay(1000);
  /* 主函数的循环每秒打印 */
  Serial.println("Hell from Main~");
  
}

/* 任务一的函数体,由于入参即为NULL，因此函数体的入参需为void *参数，否则报错 */
void TASK_ONE(void *param )
{
    /* 打印来自任务1的信息 */
    for( int i = 0;i<10;i++ )
    {
        Serial.println("Hello from TaskOne~");
        delay(1000);
    }
    /* 打印10次后通过任务一的句柄退出任务，并释放资源 */
    Serial.println("Ending TaskOne!");
    vTaskDelete( TASK_HandleOne );
 
}

/* 任务二的函数体,由于入参为void *型变量，因此函数体的入参需为强转为void *参数，否则报错 */
void TASK_TWO(void *param )
{
    /* 定义一个int 型指针用于接收强转回int型指针的参数地址 */
    int *pNum = (int *)param;
    /* 打印入参和来自任务二的信息,每次循环改变入参的值，并进行打印 */
    for( int i = 0;i<10;i++ )
    {
        /* 改变入参指针所指内存中的数据，并输出 */
        *pNum = i;
        Serial.println((*pNum));
        Serial.println("Hello from TaskTwo~~");
        delay(1000);
    }
    /* 打印10次后通过任务二的句柄退出任务，并释放资源 */
    Serial.println("Ending TaskTwo!!");
    vTaskDelete( TASK_HandleTwo );
}


