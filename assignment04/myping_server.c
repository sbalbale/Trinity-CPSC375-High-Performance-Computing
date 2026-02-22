#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    ssize_t received_bytes;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Fill server information
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        addr_len = sizeof(client_addr); // Try to receive some data
        
        received_bytes = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 
                    0, (struct sockaddr *) &client_addr,
                    &addr_len);
        
        if (received_bytes < 0) {
            perror("recvfrom failed");
            continue;
        }

        // Echo the message back to client
        if (sendto(sockfd, (const char *)buffer, received_bytes, 
                   0, (const struct sockaddr *) &client_addr,
                   addr_len) < 0) {
            perror("sendto failed");
        } else {
            // Optional: Print what was received/sent for debugging
            // buffer[received_bytes] = '\0';
            // printf(" unexpected packet from %s: %s\n", inet_ntoa(client_addr.sin_addr), buffer);
        }
    }
    
    close(sockfd);
    return 0;
}
