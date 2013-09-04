
#define SERIAL_PREAMBLE "U9"
#define SERIAL_PREAMBLE_LENGTH 2
#define SERIAL_PAYLOAD_SIZE 16

struct SerialPacket {
  unsigned char sequence_number;
  unsigned char message_type;
  unsigned char data[SERIAL_PAYLOAD_SIZE];
  unsigned char checksum;
};


Struct PWMCommand {
  unsigned char led_pwm;
  unsigned char motor_pwm;
};

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
    while(Serial.available() < sizeof(SerialPacket)
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


#define MESSAGE_SET_PWM 0
#define MESSAGE_STATUS 1
#define MESSAGE_PING 2
#define MESSAGE_PONG 3

#define RESULT_SUCCESS 0
#define RESULT_UNKNOWN_COMMAND 1
#define RESULT_OTHER_ERROR 2
#define RESULT_CHECKSUM_ERROR 3

void handle_ping(unsigned char *recv_buf, unsigned char *send_buf) {
  unsigned char byte_i;
  
  send_buf[0] = MESSAGE_PONG;
  for(byte_i = 1; byte_i < SERIAL_PAYLOAD_SIZE; byte_i++) {
    send_buf[byte_i] = recv_buf[byte_i];
  }
}

void handle_command_set_pwm(unsigned char *recv_buf, unsigned char *send_buf) {
    OCR2A = rec_buf[1];
    OCR2B = rec_buf[2];
    send_buf[1] = REC_SUCCESS;
    status_buf[2] = last_result;
    status_buf[3] = OCR2A;
    status_buf[4] = OCR2B;
}

void handle_packet(unsigned char *recv_buf, unsigned char *send_buf) {
  unsigned char command;
  
  command = buf[0];
  switch(command) {
    case MESSAGE_SET_PWM:
      return handle_command_set_pwm(recv_buf, send_buf);
      break;
    case MESSAGE_PING:
      return handle_command_ping(recv_buf, send_buf);
      break;
    default:
      return RESULT_UNKNOWN_COMMAND;
    }
}
  
void setup() {
  Serial.begin(38400); 
 
  pinMode(SCOPE_PIN, INPUT);
  
  pinMode(3, OUTPUT); // this is output "B"
  pinMode(11, OUTPUT); // this is output "A"
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2A = 20; // pin 11
  OCR2B = 29; // pin 3

}

void loop() {
  SerialPacket recv_pkt, send_pkt;
  unsigned char result;
  
  if(receive_packet(serial_buf) < 0) {
    result = RESULT_CHECKSUM_ERROR;
  }
  else {
    handle_packet(recv_buf, send_buf);
  }
  
  send_packet(send_buf);
}
