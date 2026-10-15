#define F_CPU 16000000UL 
#include <util/delay.h>
#include "mfrc522.h"

static uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF))); 
    return SPDR;
}

void mfrc522_write(uint8_t reg, uint8_t value) {
    SPI_PORT &= ~(1 << SPI_SS);               
    spi_transfer((reg << 1) & 0x7E);          
    spi_transfer(value);
    SPI_PORT |= (1 << SPI_SS);                
}

uint8_t mfrc522_read(uint8_t reg) {
    uint8_t value;
    SPI_PORT &= ~(1 << SPI_SS);               
    spi_transfer(((reg << 1) & 0x7E) | 0x80); 
    value = spi_transfer(0x00);               
    SPI_PORT |= (1 << SPI_SS);                
    return value;
}

void mfrc522_antenna_on(void) {
    uint8_t temp = mfrc522_read(TxControlReg);
    if (!(temp & 0x03)) {
        mfrc522_write(TxControlReg, temp | 0x03);
    }
}

void mfrc522_init(void) {
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS) | (1 << MFRC522_RST);
    SPI_DDR &= ~(1 << SPI_MISO);
    
    SPI_PORT |= (1 << MFRC522_RST);
    _delay_ms(1);
    SPI_PORT &= ~(1 << MFRC522_RST);
    _delay_ms(10);
    SPI_PORT |= (1 << MFRC522_RST);
    _delay_ms(10);

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPSR &= ~(1 << SPI2X);

    mfrc522_write(CommandReg, PCD_RESETPHASE);
    _delay_ms(1);

    mfrc522_write(TModeReg, 0x8D);
    mfrc522_write(TPrescalerReg, 0x3E);
    mfrc522_write(TReloadRegL, 30);
    mfrc522_write(TReloadRegH, 0);
    
    mfrc522_write(TxASKReg, 0x40);
    mfrc522_write(ModeReg, 0x3D);
    
    mfrc522_antenna_on();
}

uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_len, uint8_t *back_data, uint16_t *back_len) {
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    if (cmd == PCD_TRANSCEIVE) {
        irqEn = 0x77;
        waitIRq = 0x30; 
    }

    mfrc522_write(ComIEnReg, irqEn | 0x80);
    mfrc522_write(ComIrqReg, 0x7F); 
    mfrc522_write(CommandReg, PCD_IDLE);
    mfrc522_write(FIFOLevelReg, 0x80); 

    for (i = 0; i < send_len; i++) {
        mfrc522_write(FIFODataReg, send_data[i]);
    }

    mfrc522_write(CommandReg, cmd);
    if (cmd == PCD_TRANSCEIVE) {
        mfrc522_write(BitFramingReg, mfrc522_read(BitFramingReg) | 0x80); 
    }

    i = 2000;
    do {
        n = mfrc522_read(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    mfrc522_write(BitFramingReg, mfrc522_read(BitFramingReg) & 0x7F); 

    if (i != 0) {
        if (!(mfrc522_read(ErrorReg) & 0x1B)) { 
            status = MI_OK;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if (cmd == PCD_TRANSCEIVE) {
                n = mfrc522_read(FIFOLevelReg);
                lastBits = mfrc522_read(ControlReg) & 0x07;
                if (lastBits) {
                    *back_len = (n - 1) * 8 + lastBits;
                } else {
                    *back_len = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > 16) {
                    n = 16;
                }
                for (i = 0; i < n; i++) {
                    back_data[i] = mfrc522_read(FIFODataReg);
                }
            }
        }
    }
    return status;
}

uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type) {
    uint8_t status;
    uint16_t backBits;

    mfrc522_write(BitFramingReg, 0x07); 
    tag_type[0] = req_mode;
    status = mfrc522_to_card(PCD_TRANSCEIVE, tag_type, 1, tag_type, &backBits);

    if ((status != MI_OK) || (backBits != 16)) {
        status = MI_ERR;
    }
    return status;
}

uint8_t mfrc522_get_card_id(uint8_t *card_id) {
    uint8_t status;
    uint16_t unLen;

    mfrc522_write(BitFramingReg, 0x00); 
    card_id[0] = PICC_ANTICOLL;
    card_id[1] = 0x20; 
    status = mfrc522_to_card(PCD_TRANSCEIVE, card_id, 2, card_id, &unLen);

    return status;
}