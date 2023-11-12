#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include <peripherals/mailbox_0.h>

#define SLEEP_US 5000

int main(void)
{
    /* check if the hardware is available */
    if (!peripherals_mailbox0_is_available())
    {
        fprintf(stderr, "Hardware isn't available.\n");
        exit(1);
    }

    /* acquire a memory file descriptor with read and write */
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1)
    {
        fprintf(stderr, "Can't open /dev/mem.\n");
        exit(1);
    }
    /* create a mailbox instance and initialize it */
    mailbox_t mailbox;
    if (peripherals_mailbox0_init(&mailbox, mem_fd) != MAILBOX_OK)
    {
        fprintf(stderr, "The init of mailbox peripheral failed.\n");
        exit(1);
    }

    /* open the input and output file for testing feature */
    FILE *input_file = fopen("input_data.txt", "r");
    FILE *output_file = fopen("output_data.txt", "a");

    if (!input_file || !output_file)
    {
        fprintf(stderr, "File open failed.\n");
        return 1;
    }

    /* main testing feature logic */
    uint8_t values_to_read = 0;
    bool is_input_finished = false;

    /* Run until every integer from input file has been written to hardware, processed and read.
     * The loop polls read and write status registers with a 5 ms delay between each iteration.
     *
     *   -> is_input_finished checks if all of the integers have been read from the input file
     *   -> values_to_read keeps track of how many integers have been written to hardware
     */
    while ((is_input_finished == false) && (values_to_read == 0))
    {
        if (peripherals_mailbox0_is_read_empty(&mailbox) == false)
        {
            int32_t data;
            if (peripherals_mailbox0_read_from_register(&mailbox, &data) != MAILBOX_OK)
            {
                fprintf(stderr, "Read from read FIFO failed.\n");
                exit(1);
            }
            fprintf(output_file, "%d\n", data);
            values_to_read--;
        }

        if ((peripherals_mailbox0_is_write_full(&mailbox) == false) && (is_input_finished == false))
        {
            int32_t data;

            uint8_t fscanf_status = fscanf(input_file, "%d", &data);

            if (fscanf_status == 1)
            {
                if (peripherals_mailbox0_write_to_register(&mailbox, data) != MAILBOX_OK)
                {
                    fprintf(stderr, "Write to write FIFO failed.\n");
                    exit(1);
                }
                values_to_read++;
            }
            else if (fscanf_status == EOF)
            {
                is_input_finished = true;
            }
            else
            {
                fprintf(stderr, "Function fscanf couldn't read one integer. Moving on...\n");
            }
        }
        usleep(SLEEP_US);
    }

    /* close all open files */
    fclose(input_file);
    fclose(output_file);

    /* deinit mailbox and memory file descriptor */
    if (peripherals_mailbox0_deinit(&mailbox, mem_fd) != MAILBOX_OK)
    {
        fprintf(stderr, "Deinit of mailbox failed.\n");
        exit(1);
    }

    return 0;
}