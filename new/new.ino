#include <Servo.h>  // 引入舵机库

#define MOTOR_PIN PA4   // MOS管控制直流电机
#define SERVO_PIN PA7   // 舵机控制信号
#define SENSOR1_PIN PA6 // 传感器1
#define SENSOR2_PIN PA5 // 传感器2

Servo myServo;
unsigned long previousMillis = 0;
const long interval = 2000;  // 舵机每 2 秒触发一次

unsigned long motorStartTime = 0; // 记录电机开始运行的时间
const long motorTimeout = 30000;  // 30秒超时

bool servoStopped = true; // 记录舵机是否已经停止，防止重复写入0度
bool firstActivation = true; // 记录是否是第一次激活系统

// 串口打印相关
unsigned long lastSerialTime = 0;
const long serialInterval = 1000; // 每秒打印一次传感器数据

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(65);
  digitalWrite(MOTOR_PIN, LOW); // 关闭电机

  // 初始化串口通信，波特率为9600
  Serial.begin(9600);
  //  Serial.println("系统启动，开始监控传感器数据...");
  //  Serial.println("PA6(传感器1), PA5(传感器2), 系统状态");
}

void loop() {
  bool sensor1 = digitalRead(SENSOR1_PIN);  // 读取 PA6
  bool sensor2 = digitalRead(SENSOR2_PIN);  // 读取 PA5
  unsigned long currentMillis = millis();

  // 定期通过串口打印传感器数据
  if (currentMillis - lastSerialTime >= serialInterval) {
    lastSerialTime = currentMillis;
    //    Serial.print("传感器1(PA6): ");
    //    Serial.print(sensor1 ? "HIGH" : "LOW");
    //    Serial.print(", 传感器2(PA5): ");
    //    Serial.print(sensor2 ? "HIGH" : "LOW");
    //    Serial.print(", 状态: ");

    //    if (sensor1 == LOW && sensor2 == LOW) {
    //      Serial.println("系统工作中");
    //    } else if (sensor1 == LOW && sensor2 == HIGH) {
    //      Serial.println("重置状态");
    //    } else {
    //      Serial.println("系统停止");
    //    }
  }

  if (sensor1 == LOW && sensor2 == LOW) {
    // PA6 低且 PA5 低，电机运转，舵机运动
    if (firstActivation) {
      // 第一次进入工作模式时，延迟3.5秒
      //      Serial.println("首次激活，延迟3.5秒后启动...");
      delay(3500);
      firstActivation = false; // 标记已经不是第一次了
      //      Serial.println("系统启动完成，开始工作");
    }

    if (motorStartTime == 0) {
      motorStartTime = currentMillis; // 记录启动时间
      //      Serial.println("开始记录电机运行时间");
    }

    if (currentMillis - motorStartTime >= motorTimeout) {
      // 如果持续运行超过 30 秒，强制停止
      digitalWrite(MOTOR_PIN, LOW);
      //      Serial.println("超时保护：电机运行超过30秒，强制停止");

      if (!servoStopped) {  // 只在舵机未停止时执行
        myServo.write(65);
        servoStopped = true;
      }
    } else {
      // 电机正常运行
      digitalWrite(MOTOR_PIN, HIGH);
      servoStopped = false;  // 标记舵机正在运行

      // 每 2 秒触发一次舵机运动（瞬间 0° → 65° → 0°）
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        myServo.write(0);
        //        Serial.println("舵机动作：65° → 0°");
        delay(200);
        myServo.write(65);
        //        Serial.println("舵机动作：0° → 65°");
      }
    }
  }
  else {
    // 任何其他传感器状态组合，停止设备
    digitalWrite(MOTOR_PIN, LOW);

    if (!servoStopped) {  // 只有当舵机不是停止状态时才执行
      myServo.write(65);
      servoStopped = true;  // 标记舵机已经停止
      //      Serial.println("停止状态：舵机回到65°位置");
    }

    motorStartTime = 0;  // 重置超时计时

    //        // 只有当PA6低且PA5高时，才重置firstActivation标志
    //        if (sensor1 == LOW && sensor2 == HIGH) {
    //            if (!firstActivation) {
    //                Serial.println("系统标志重置：sensor1 == LOW && sensor2 == HIGH下次启动将有3.5秒延迟");
    //                firstActivation = true;
    //            }
    //        }
    //
    //        if (sensor1 == HIGH) {
    //            if (!firstActivation) {
    //                Serial.println("系统标志重置：sensor1 == HIGH下次启动将有3.5秒延迟");
    //                firstActivation = true;
    //            }
    //        }

    if (!firstActivation) {
      //      Serial.println("系统标志重置下次启动将有3.5秒延迟");
      firstActivation = true;
    }
  }
}
