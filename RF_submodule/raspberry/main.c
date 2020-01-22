#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "radio_emitter.h"

static int continue_tx = 1;

void sig_handler(int signo)
{
    if (signo == SIGINT)
        continue_tx = 0;
}


int main()
{
    radio_payload_t payload;
    payload.flags = 0x00;
    
    printf("Starting radio tranceiver test routine...\n");
    if (radio_emitter_initialize() != 0) {
        fprintf(stderr, "Failed to initialize radio.\n");
    }
    
    signal(SIGINT, sig_handler);
    
    do {
        payload.flags++;
        radio_emitter_send_packet(&payload);
        while (!radio_is_packet_sent())
            usleep(1000);
    } while(continue_tx);
    
    radio_emitter_terminate();
    return 0;
}


