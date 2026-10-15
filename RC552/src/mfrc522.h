#ifndef MFRC522_H
#define MFRC522_H

#include <avr/io.h>

#define SPI_DDR       DDRB
#define SPI_PORT      PORTB
#define SPI_SS        PB0   
#define SPI_SCK       PB1   
#define SPI_MOSI      PB2   
#define SPI_MISO      PB3   
#define MFRC522_RST   PB4   

#define CommandReg    0x01
#define ComIEnReg     0x02
#define ComIrqReg     0x04
#define DivIrqReg     0x05
#define ErrorReg      0x06
#define Status2Reg    0x08
#define FIFODataReg   0x09
#define FIFOLevelReg  0x0A
#define WaterLevelReg 0x0B
#define ControlReg    0x0C
#define BitFramingReg 0x0D
#define CollReg       0x0E
#define ModeReg       0x11
#define TxModeReg     0x12
#define RxModeReg     0x13
#define TxControlReg  0x14
#define TxASKReg      0x15
#define TModeReg      0x2A
#define TPrescalerReg 0x2B
#define TReloadRegH   0x2C
#define TReloadRegL   0x2D

#define PCD_IDLE       0x00
#define PCD_TRANSMIT   0x04
#define PCD_RECEIVE    0x08
#define PCD_TRANSCEIVE 0x0C
#define PCD_RESETPHASE 0x0F

#define PICC_REQIDL    0x26
#define PICC_ANTICOLL  0x93

#define MI_OK          0
#define MI_NOTAGERR    1
#define MI_ERR         2

void mfrc522_init(void);
void mfrc522_write(uint8_t reg, uint8_t value);
uint8_t mfrc522_read(uint8_t reg);
void mfrc522_antenna_on(void);
uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_len, uint8_t *back_data, uint16_t *back_len);
uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type);
uint8_t mfrc522_get_card_id(uint8_t *card_id);

#endif