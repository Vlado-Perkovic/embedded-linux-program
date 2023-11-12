#include <peripherals/mailbox_0.h>
#include <utils.h>
#include <sys/mman.h>

bool peripherals_mailbox0_is_available()
{
    char *path = "/proc/device-tree/mailbox_0/compatible";

    FILE *file = fopen(path, "r");

    if (NULL == file)
    {
        printf("Device mailbox_0 does not exist in the device tree.\n");
        return false;
    }

    char compatible_string[100];
    fscanf(file, "%s", compatible_string);
    printf("The device mailbox_0 exists with compatible value: %s\n", compatible_string);
    fclose(file);
    return true;
}

bool peripherals_mailbox0_is_write_full(mailbox_t *mailbox)
{
    return utils_to_big_endian(*mailbox->write_fifo_status) == 1;
}

bool peripherals_mailbox0_is_read_empty(mailbox_t *mailbox)
{
    return utils_to_big_endian(*mailbox->read_fifo_status) == 1;
}

mailbox_error_t peripherals_mailbox0_init(mailbox_t *mailbox, uint32_t mem_fd)
{
    mailbox->mapped_base = mmap(0, REGISTER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, BASE_ADDRESS);

    if (mailbox->mapped_base == MAP_FAILED)
    {
        fprintf(stderr, "Mapping memory failed.\n");
        if (close(mem_fd) != 0)
        {
            fprintf(stderr, "The closing of memory file descriptor failed.\n");
        }
        return MAILBOX_ERR;
    }

    mailbox->read_fifo_buffer = (volatile int *)((char *)mailbox->mapped_base + READ_FIFO_OFFSET);
    mailbox->read_fifo_status = (volatile int *)((char *)mailbox->mapped_base + READ_FIFO_STATUS_OFFSET);

    mailbox->write_fifo_buffer = (volatile int *)((char *)mailbox->mapped_base + WRITE_FIFO_OFFSET);
    mailbox->write_fifo_status = (volatile int *)((char *)mailbox->mapped_base + WRITE_FIFO_STATUS_OFFSET);

    return MAILBOX_OK;
}

mailbox_error_t peripherals_mailbox0_write_to_register(mailbox_t *mailbox, int32_t data)
{
    if (mailbox == NULL)
    {
        fprintf(stderr, "Mailbox is a NULL pointer.\n");
        return MAILBOX_ERR;
    }
    *mailbox->write_fifo_buffer = utils_to_big_endian(data);
    return MAILBOX_OK;
}

mailbox_error_t peripherals_mailbox0_read_from_register(mailbox_t *mailbox, int32_t *data)
{
    if (mailbox == NULL || data == NULL)
    {
        fprintf(stderr, "Mailbox or data is a NULL pointer.\n");
        return MAILBOX_ERR;
    }
    *data = utils_to_big_endian(*mailbox->read_fifo_buffer);
    return MAILBOX_OK;
}

mailbox_error_t peripherals_mailbox0_deinit(mailbox_t *mailbox, uint32_t mem_fd)
{
    if (close(mem_fd) != 0)
    {
        fprintf(stderr, "The closing of memory file descriptor failed.\n");
        return MAILBOX_ERR;
    }

    if (mailbox == NULL)
    {
        fprintf(stderr, "Mailbox is na NULL pointer.\n");
        return MAILBOX_ERR;
    }

    if (munmap(mailbox->mapped_base, REGISTER_SIZE) != 0)
    {
        fprintf(stderr, "Unmapping of mailbox failed.\n");
        return MAILBOX_ERR;
    }
    return MAILBOX_OK;
}