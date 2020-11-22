class LcdDisplayController {
public:
  void init();
  void showScore(int score);
  void displayMessage(char *message);

private:
  void moveToPosition(uint8_t row, uint8_t column);
  void writeString(char *message);
  
  void sendCommand(uint8_t command);
  void sendData(uint8_t data);
  
  void sendRawCommand(uint8_t command, uint8_t flags);
  void writeByteOverI2C(uint8_t byteToSend);
};

extern LcdDisplayController LcdDisplay;
