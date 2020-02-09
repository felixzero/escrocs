#define CHANNEL_MOTOR_ENC1 1
#define CHANNEL_MOTOR_ENC2 2

void initialize_datalog()
{
  Serial.begin(9600);
}

char beginning_of_transmission[] = { 0xFF, 0xFF, 0xFF, 0xFF };

int log_datapoint(char channel, float value)
{
  unsigned long now = millis();
  
  if (Serial.availableForWrite() < 13)
    return -1;

  Serial.write(beginning_of_transmission, 4);
  Serial.write(channel);
  Serial.write((char*)&now, 4);
  Serial.write((char*)&value, 4);

  return 0;
}
