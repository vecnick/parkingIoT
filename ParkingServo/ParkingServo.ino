#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position


#define BARCODE_SERIAL    Serial2
int count = 0;
String number = "";


void setup() {
      // attaches the servo on pin 9 to the servo object
    Serial.begin(115200);     

   Serial.println("Serial init OK");
  // открываем Serial-соединение с Barcode-модулем на скорости 9600 бод
  BARCODE_SERIAL.begin(9600);
  // печатаем строку
  Serial.println("Barcode init is OK");

  
    // Start the software serial for communication with the ESP8266
    Serial1.begin(115200);  //this assumes default baud rate is used by the module
 
    Serial.println("");
    Serial.println("Ready");   

     
    Serial.println("Setting WiFI mode…");
    Serial1.write("AT+CWMODE=3\r\n");
    delay(1000);
    while ( Serial1.available() )   {  Serial.write( Serial1.read() );  }
    Serial.println("Connecting to WiFi");
    Serial1.write("AT+CWJAP=\"MyWIFI\",\"123445678\"\r\n"); //insert your own SSID and password here
    delay(1000);
    Serial.println("Establishing connection type…");
    Serial1.write("AT+CIPMUX=0\r\n");
    
    
    while ( Serial1.available() )   {  Serial.write( Serial1.read() );  }
    delay(10000);
}

void loop() {
  
  // если приходят данные из Barcode-модуля - отправим их в порт компьютера
  if (BARCODE_SERIAL.available()) {
    count++;
    char buf = BARCODE_SERIAL.read();
    if (buf >= 48 && buf <=57){
      number+=buf;  
    }
    Serial.write(buf);
    if (count==11) {
      Serial.println("");
      Serial.println(number);

    String tmp="";
    int counter =0;
    Serial.println("Establishing TCP connection");
    Serial1.write("AT+CIPSTART=\"TCP\",\"192.168.217.185\",8070\r\n");
    delay(1000);
    while ( Serial1.available() )   {  Serial.write( Serial1.read() );  }
    Serial.println("Sending out data…");
    Serial1.write("AT+CIPSEND=58\r\n");
    while ( Serial1.available() )   {  Serial.write( Serial1.read() );  }
    delay(1000);
    String sending = "GET /?test="+number+" HTTP/1.1";  
    
//    int count = sending.length();  // lenth() is 3
//    char buff[count];
//    sending.toCharArray(buff, count);
    Serial.println(number.length());
    Serial.println(number);
    Serial1.println(sending);
    
    number=""; 
    //Serial1.write("\r\n");
    Serial1.write("Host: 192.168.217.185\r\n\r\n"); 

    bool flag = false;
    while ( true )   {  
      if (Serial1.available()){
        //Serial.write(Serial1.read());
        char a = Serial1.read();
        if (a == '{'){
          flag = true;
          continue;
        }
        if (a == '}'){
          flag = false;
          Serial.println(tmp);
          if (tmp == "True"){
            myservo.attach(9);
            for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
              myservo.write(pos);              // tell servo to go to position in variable 'pos'
              delay(50);                       // waits 15 ms for the servo to reach the position
            }
            myservo.detach();
          }
          break;
        }
       if (flag){
        tmp += a;
       }
      }
    } 
//        while (Serial.available()) {
//    BARCODE_SERIAL.write(Serial.read());
//  }
      count = -1;
      number = "";
      BARCODE_SERIAL.flush();
    }
  }

}
