#define BUFFER_SIZE 1000
unsigned char output_byte;
unsigned char buffer[BUFFER_SIZE];
#define SCOPE_PIN 8

#define SERIAL_PREAMBLE "U9"
#define SERIAL_PREAMBLE_LENGTH 2
#define SERIAL_PAYLOAD_SIZE 16

void send_packet(unsigned char *buf) {
  Serial.write(SERIAL_PREAMBLE);
  
  unsigned char checksum = 0;
  for(unsigned char byte_i = 0; byte_i < SERIAL_PAYLOAD_SIZE; byte_i++) {
    Serial.write(buf[byte_i]);
    checksum ^= buf[byte_i];
  }
  Serial.write(checksum);
}

unsigned char receive_packet(unsigned char *buf) {
  int input_byte;
  
  // look for a preamble
  unsigned char preamble_i = 0;
  while(true) {
    while(Serial.available() < 1)
      /* wait */;
    
    input_byte = Serial.read();
    
    if(input_byte == SERIAL_PREAMBLE[preamble_i] || true) {
      preamble_i++;
    }
    else {
      preamble_i = 0;
    }
      
    if(preamble_i == SERIAL_PREAMBLE_LENGTH) {
      break;
    }
  }
      
  // read the payload
  unsigned char buf_i = 0;
  unsigned char checksum = 0;
  while(buf_i < SERIAL_PAYLOAD_SIZE) {
    while(Serial.available() < 1)
      /* wait */;
    input_byte = Serial.read();

    buf[buf_i] = (unsigned char) input_byte;
    checksum ^= input_byte;
    buf_i++;
  }
  
  // read the checksum
  int expected_checksum;
  while(Serial.available() < 1)
      /* wait */;
  expected_checksum = (unsigned char) Serial.read();
  
  if(checksum != expected_checksum)
    return -1;
  
  return 0;
}

#define COMMAND_SET_PWM 0

void handle_command_set_pwm(unsigned char *buf) {
  
}

void handle_packet(unsigned char *buf) {
  unsigned char command;
  
  command = buf[0];
  switch(command) {
    case COMMAND_SET_PWM:
      handle_command_set_pwm(&(buf[1]));
  }
    
}
  
void setup() {
  Serial.begin(38400); 
  /*
  pinMode(SCOPE_PIN, INPUT);
  
  pinMode(3, OUTPUT); // this is output "B"
  pinMode(11, OUTPUT); // this is output "A"
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2A = 20; // pin 11
  OCR2B = 29; // pin 3
  */
}

void loop() {
  unsigned char serial_buf[SERIAL_PAYLOAD_SIZE];
  
  if(receive_packet(serial_buf) < 0)
    send_packet((unsigned char *) "incorrect checksum!!!!!");
  else
    send_packet(serial_buf);
 
}
