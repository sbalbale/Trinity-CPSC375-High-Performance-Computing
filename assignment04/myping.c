#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#define PORT 12345
#define PING_PAYLOAD "Ping"
#define TIMEOUT_SEC 1

int sockfd;
struct sockaddr_in server_addr;

// Statistics variables
long sent_packets = 0;
long received_packets = 0;
double min_rtt = 999999.0;
double max_rtt = 0.0;
double total_rtt = 0.0;
char *target_host;

void cleanup(int signum) {
    close(sockfd);
    
    printf("\n--- %s ping statistics ---\n", target_host);
    if (sent_packets > 0) {
        printf("%ld packets transmitted, %ld received, %.0f%% packet loss\n",
            sent_packets, received_packets,
            ((sent_packets - received_packets) / (double)sent_packets) * 100.0);
    }
    
    if (received_packets > 0) {
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
            min_rtt, total_rtt / received_packets, max_rtt);
    }
    
    exit(0);
}

int main(int argc, char *argv[]) {
    struct hostent *server;
    struct sockaddr_in from_addr;
    socklen_t from_len;
    struct timeval start, end, timeout;
    char buffer[1024];
    double rtt;
    ssize_t n;
    int seq = 0;

    if (argc < 2) {
        fprintf(stderr,"usage: %s hostname\n", argv[0]);
        exit(0);
    }

    target_host = argv[1];

    // Resolve hostname
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set socket timeout
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        exit(1);
    }

    // Setup server address structure
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // Handle Ctrl+C
    signal(SIGINT, cleanup);

    printf("PING %s (%s) %lu bytes of data.\n", 
           argv[1], inet_ntoa(server_addr.sin_addr), sizeof(PING_PAYLOAD));

    while (1) {
        seq++;
        gettimeofday(&start, NULL);

        // Send ping
        if (sendto(sockfd, PING_PAYLOAD, strlen(PING_PAYLOAD), 0,
                   (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            perror("sendto");
        }        sent_packets++;
        from_len = sizeof(from_addr);
        if ((n = recvfrom(sockfd, buffer, 1024, 0,
                     (struct sockaddr *) &from_addr, &from_len)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Request timeout for seq %d\n", seq);
            } else {
                perror("recvfrom");
            }
        } 
        else {
            gettimeofday(&end, NULL);
            // Calculate RTT in milliseconds
            long seconds = end.tv_sec - start.tv_sec;
            long microseconds = end.tv_usec - start.tv_usec;
            double rtt = (seconds * 1000.0) + (microseconds / 1000.0);

            // Print output
            printf("from %s (%s): seq=%d time=%.3f ms\n", 
                   argv[1], inet_ntoa(from_addr.sin_addr), seq, rtt);

            // Update stats
            received_packets++;
            total_rtt += rtt;
            if (rtt < min_rtt) min_rtt = rtt;
            if (rtt > max_rtt) max_rtt = rtt;
        }

        sleep(1);
    }

    return 0;
}
