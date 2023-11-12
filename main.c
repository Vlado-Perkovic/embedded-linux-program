#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BASE_ADDRESS 0x43800000
#define REGISTER_SIZE 0x10000
#define READ_FIFO_OFFSET 0x0
#define READ_FIFO_STATUS_OFFSET 0x4
#define WRITE_FIFO_OFFSET 0x8
#define WRITE_FIFO_STATUS_OFFSET 0xC

#define IS_NOT_EMPTY 0
#define IS_NOT_FULL 0

#define SLEEP_US 5000

bool is_hardware_available(const char *node_label)
{

    char path[100];

    snprintf(path, sizeof(path), "/proc/device-tree/%s/compatible", node_label);

    FILE *file = fopen(path, "r");

    if (NULL == file)
    {
        printf("Device %s does not exist in the device tree.\n", node_label);
        return false;
    }

    char compatible_string[100];
    fscanf(file, "%s", compatible_string);
    printf("The device %s exists with compatible value: %s\n", node_label, compatible_string);
    fclose(file);
    return true;
}

int main(void)
{

    if (!is_hardware_available("mailbox_0"))
    {
        fprintf(stderr, "Hardware isn't available.\n");
        exit(1);
    }

    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1)
    {
        fprintf(stderr, "Can't open /dev/mem.\n");
        return 1;
    }

    void *mapped_base = mmap(0, REGISTER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, BASE_ADDRESS);

    if (mapped_base == MAP_FAILED)
    {
        fprintf(stderr, "Mapping memory failed.\n");
        close(mem_fd);
        return 1;
    }

    volatile int *read_fifo = (volatile int *)((char *)mapped_base + READ_FIFO_OFFSET);
    volatile int *read_fifo_status = (volatile int *)((char *)mapped_base + READ_FIFO_STATUS_OFFSET);

    volatile int *write_fifo = (volatile int *)((char *)mapped_base + WRITE_FIFO_OFFSET);
    volatile int *write_fifo_status = (volatile int *)((char *)mapped_base + WRITE_FIFO_STATUS_OFFSET);

    FILE *input_file = fopen("input_data.txt", "r");
    FILE *output_file = fopen("output_data.txt", "a");

    if (!input_file || !output_file)
    {
        fprintf(stderr, "File open failed.\n");
        return 1;
    }

    int values_to_read = 0;
    bool is_input_finished = false;

    while ((is_input_finished == false) && (values_to_read == 0))
    {
        if (*read_fifo_status == IS_NOT_EMPTY)
        {
            fprintf(output_file, "%d\n", *read_fifo);
            values_to_read--;
        }

        if ((*write_fifo_status == IS_NOT_FULL) && (is_input_finished == false))
        {
            int data;
            if (fscanf(input_file, "%d", &data) != EOF)
            {

                *write_fifo = data;
                values_to_read++;
            }
            else
            {
                is_input_finished = true;
            }
        }
        usleep(SLEEP_US);
    }

    fclose(input_file);
    fclose(output_file);

    // Unmap memory and close file descriptors
    munmap(mapped_base, REGISTER_SIZE);
    close(mem_fd);

    return 0;
}