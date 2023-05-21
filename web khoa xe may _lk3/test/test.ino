String stringOne, stringTwo;

void setup() {
  // Mở cổng Serial để debug - xem thành quả của chúng ta
  Serial.begin(9600);
  while (!Serial) {
    ; // đợi cho cổng serial đươc bật (chỉ cần thiết với Arduino Leonardo)
  }

  stringOne = String("Cam bien");
  stringTwo = String("Gia tri ");
  //Gửi một đoạn thông báo là đã chạy xong hàm setup()
  Serial.println("\n\nVi du ve noi chuoi:");
  Serial.println();
}

void loop() {
  Serial.println(stringOne);  // in chữ "cam bien"

  // chèn chuỗi thứ 1 vào sau chuỗi thứ 2
  stringTwo += stringOne;
  Serial.println(stringTwo);  // in chữ "Gia tri cam bien"

  // thêm một chuỗi hằng vào
  stringTwo += " cho dau Analog ";
  Serial.println(stringTwo);  // prints "Gia tri cam bien cho dau Analog "

  // Thêm một hằng ký tự vào
  stringTwo += 'A';
  Serial.println(stringTwo);   // prints "Gia tri cam bien cho dau Analog A"

  // Thêm một số nguyên vào
  stringTwo += 0;
  Serial.println(stringTwo);   // prints "Gia tri cam bien cho dau Analog A0"

  
  // thêm giá trị của một chân analog vào
  stringTwo += ": ";
  stringTwo += analogRead(A0);
  Serial.println(stringTwo);   // prints "Gia tri cam bien cho dau Analog A0: 123" hoặc là một giá trị bất kỳ mà analogRead(A0) trả về 

  Serial.println("\n\nDoi gia tri của bien Chuoi");
  stringOne = "Gia tri cua mot bien so nguyen kieu long: ";
  stringTwo = "millis(): ";

  stringOne += 123456789L; 
  Serial.println(stringOne);   //Xuất "Gia tri cua mot bien so nguyen kieu long: 123456789"

  stringTwo.concat(millis());
  Serial.println(stringTwo); //Xuất "millis(): "+thời điểm hiện tại theo mili giây

  // dừng hầm loop
  while(true)
  {
    delay(100);
  }
}
