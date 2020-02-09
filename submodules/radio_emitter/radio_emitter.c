#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "radio_emitter.h"

#define SPIDEV "/dev/spidev0.0"
#define SPI_DELAY 0
#define SPI_SPEED 500000
#define RST_GPIO 25

static int spi_fd;

/* Read register from SPI */
static uint8_t read_register(uint8_t reg)
{
    char input_buffer[2], output_buffer[2];
    struct spi_ioc_transfer spi_transfer = {
            .tx_buf = (unsigned long)input_buffer,
            .rx_buf = (unsigned long)output_buffer,
            .len = 2,
            .bits_per_word = 8,
            .delay_usecs = 0,
            .speed_hz = 500000
    };


    input_buffer[0] = reg & 0x7F;
    input_buffer[1] = 0;

    /* Send system call for full duplex communication */
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 1)
        return 0xFF;
    
    return output_buffer[1];
}


/* Set a SPI register */
static void write_register(uint8_t reg, uint8_t value)
{
    int ret;
    uint8_t tx_buffer[2];
    
    tx_buffer[0] = reg | 0x80;
    tx_buffer[1] = value;
    
    struct spi_ioc_transfer tr =
    {
        .tx_buf = (long)tx_buffer,
        .rx_buf = 0,
        .len = 2,
        .delay_usecs = SPI_DELAY,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
    };

    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        fprintf(stderr, "Unable to write register %x.\n", reg);
}


/* Public interface to initialize radio */
int radio_emitter_initialize(void)
{
    int i;
    
    /* Reset the chip */
    send_reset_gpio();
    
    /* Open SPI device */
    if ((spi_fd = open(SPIDEV, O_RDWR)) < 0)
        return -1;
    
    /* Configure RF device */
	for (i = 0; i < ARRAY_SIZE(COMMON_RF_PARAMETERS); i+=2)
        write_register(COMMON_RF_PARAMETERS[i], COMMON_RF_PARAMETERS[i + 1]);

    return 0;
}


/* Public interface to free resources */
void radio_emitter_terminate(void)
{
    close(spi_fd);
    send_reset_gpio();
}


/* Query RegIrqFlags2 register and check PacketSent bit */
int radio_is_packet_sent(void)
{
    return (read_register(0x28) & 0x08);
}


/* Send one packet */
void radio_emitter_send_packet(radio_payload_t *packet)
{
    int i;
    uint8_t *raw_frame = (uint8_t*)packet;

    /* Stop transmission */
    write_register(0x01, 0x04);
    
    /* Fill up FIFO */
    for (i = 0; i < PAYLOAD_LENGTH; ++i) {
        write_register(0x00, raw_frame[i]);
    }
    
    /* Start transmission */
    write_register(0x01, 0x0C);
}


/* Reinitialize the radio by pulsing the reset pin */
void send_reset_gpio(void)
{
    int mem_fd;
    volatile unsigned int *gpio;
    
    /* Map GPIO access to gpio pointer */
    mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (mem_fd < 0) {
        fprintf(stderr, "Failed to open memory device.\n");
    }
    
    gpio = (volatile unsigned int*)mmap(NULL, 4*1024, PROT_READ|PROT_WRITE,
                                        MAP_SHARED, mem_fd, 0x3F000000 + 0x200000);
    if (gpio == MAP_FAILED) {
        fprintf(stderr, "Failed to map memory device.\n");
    }
    
    close(mem_fd);
    
    /* Send reset pulse on GPIO */
    *(gpio + RST_GPIO / 10) &= ~(7 << ((RST_GPIO % 10) * 3)); /* Set GPIO as input (required) */
    *(gpio + RST_GPIO / 10) |=  (1 << ((RST_GPIO % 10) * 3)); /* Set GPIO as output */
    *(gpio + 7) = 1 << RST_GPIO; /* Set RST_GPIO as HIGH */
    usleep(100000);
    *(gpio + 10) = 1 << RST_GPIO; /* Set RST_GPIO as LOW */
    usleep(100000);
}

