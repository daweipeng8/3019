#include <Servo.h>  // 引入舵机库

#define MOTOR_PIN PA4   // MOS管控制直流电机
#define SERVO_PIN PA7   // 舵机控制信号
#define SENSOR1_PIN PA6 // 传感器1
#define SENSOR2_PIN PA5 // 传感器2

Servo myServo;  
unsigned long previousMillis = 0;
const long interval = 2000;  // 舵机每 2.5 秒触发一次

unsigned long motorStartTime = 0; // 记录电机开始运行的时间
const long motorTimeout = 30000;  // 30秒超时

bool servoStopped = true; // 记录舵机是否已经停止，防止重复写入0度

void setup() {
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(SENSOR1_PIN, INPUT);
    pinMode(SENSOR2_PIN, INPUT);
    myServo.attach(SERVO_PIN);
    myServo.write(0); // 舵机归零
    digitalWrite(MOTOR_PIN, LOW); // 关闭电机
}

void loop() {
    bool sensor1 = digitalRead(SENSOR1_PIN);  // 读取 PA6
    bool sensor2 = digitalRead(SENSOR2_PIN);  // 读取 PA5
    unsigned long currentMillis = millis();

    if (sensor2 == HIGH || sensor1 == LOW) {  
        // PA5 高 或 PA6 低，所有设备停止
        digitalWrite(MOTOR_PIN, LOW);

        if (!servoStopped) {  // 只有当舵机不是停止状态时才执行
            myServo.write(0); 
            servoStopped = true;  // 标记舵机已经停止
        }

        motorStartTime = 0;  // 重置超时计时
    } 
    else {  
        // PA6 高且 PA5 低，电机运转，舵机运动
        if (motorStartTime == 0) { 
            motorStartTime = currentMillis; // 记录启动时间
        }

        if (currentMillis - motorStartTime >= motorTimeout) {  
            // 如果持续运行超过 30 秒，强制停止
            digitalWrite(MOTOR_PIN, LOW);

            if (!servoStopped) {  // 只在舵机未停止时执行
                myServo.write(0);
                servoStopped = true;
            }
        } else {
            // 电机正常运行
            digitalWrite(MOTOR_PIN, HIGH);
            servoStopped = false;  // 标记舵机正在运行

            // 每 2.5 秒触发一次舵机运动（瞬间 0° → 45° → 0°）
            if (currentMillis - previousMillis >= interval) {
                previousMillis = currentMillis;
                myServo.write(65);  // 先转到 45°
                delay(300);         // 等待 200ms
                myServo.write(0);   // 立刻回到 0°
            }
        }
    }
}
