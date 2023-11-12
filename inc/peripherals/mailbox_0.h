#ifndef _MAILBOX0_H
#define _MAILBOX0_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define BASE_ADDRESS 0x43800000
#define REGISTER_SIZE 0x10000
#define READ_FIFO_OFFSET 0x0
#define READ_FIFO_STATUS_OFFSET 0x4
#define WRITE_FIFO_OFFSET 0x8
#define WRITE_FIFO_STATUS_OFFSET 0xC

/**
 * @brief a struct for mailbox related pointers
 *
 */
typedef struct
{
    void *mapped_base;
    volatile uint32_t *read_fifo_buffer;
    volatile uint32_t *read_fifo_status;
    volatile uint32_t *write_fifo_buffer;
    volatile uint32_t *write_fifo_status;
} mailbox_t;

/**
 * @brief simple enum for errors
 *
 */
typedef enum
{
    MAILBOX_OK = 0,
    MAILBOX_ERR = -1,
} mailbox_error_t;

/**
 * @brief function that checks if the mailbox_0 is available
 *
 * @return true if available
 * @return false if not available
 */
bool peripherals_mailbox0_is_available();

/**
 * @brief function that queries the write FIFO status register
 *
 * @param mailbox instance of mailbox struct
 * @return true if the flag is set to 1
 * @return false if the flag is set to 0
 */
bool peripherals_mailbox0_is_write_full(mailbox_t *mailbox);

/**
 * @brief function that queries the read FIFO status register
 *
 * @param mailbox instance of mailbox struct
 * @return true if the flag is set to 1
 * @return false if the flag is set to 0
 */
bool peripherals_mailbox0_is_read_empty(mailbox_t *mailbox);

/**
 * @brief function that initializes the memory mapping of the mailbox_0
 *
 * @param mailbox instance of mailbox struct
 * @param mem_fd memory file descriptor
 * @return mailbox_error_t a custom error value
 */
mailbox_error_t peripherals_mailbox0_init(mailbox_t *mailbox, uint32_t mem_fd);

/**
 * @brief function that writes 32-bit integer to the write FIFO register.
 *
 * function makes sure to match the endianness of the given hardware.
 *
 * @param mailbox instance of mailbox struct
 * @param data 32-bit integer
 * @return mailbox_error_t a custom error value
 */
mailbox_error_t peripherals_mailbox0_write_to_register(mailbox_t *mailbox, int32_t data);

/**
 * @brief function that reads a 32-bit integer to a data parameter from the read FIFO register.
 *
 * @param mailbox instance of mailbox struct
 * @param data int32_t variable to store the data
 * @return mailbox_error_t a custom error value
 */
mailbox_error_t peripherals_mailbox0_read_from_register(mailbox_t *mailbox, int32_t *data);

/**
 * @brief function that deinits the mailbox and closes the memory file descriptor
 *
 * @param mailbox instance of mailbox struct
 * @param mem_fd memory file descriptor
 * @return mailbox_error_t a custom error value
 */
mailbox_error_t peripherals_mailbox0_deinit(mailbox_t *mailbox, uint32_t mem_fd);

#endif //_MAILBOX0_H