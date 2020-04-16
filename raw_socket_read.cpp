#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>

#include <string.h>
#include <vector>
#include <iostream>

int main() {
    std::vector<char> buffer;
    buffer.reserve(65536);

    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (socket_fd == -1) {
        std::cout << "Failed to create socket";
    }
    while(true) {
        struct sockaddr sock_addr;
        socklen_t len = sizeof(sock_addr);
        int size = recvfrom(socket_fd, buffer.data(), 65536, 0, &sock_addr, &len);

        struct sockaddr_in source_socket_address, dest_socket_address;
        memset(&source_socket_address, 0, sizeof(source_socket_address));
        memset(&dest_socket_address, 0, sizeof(dest_socket_address));

        struct iphdr *ip_packet = (struct iphdr *)(buffer.data() + sizeof(struct ethhdr));
        source_socket_address.sin_addr.s_addr = ip_packet->saddr;
        dest_socket_address.sin_addr.s_addr = ip_packet->daddr;

        std::cout << "--------------------\nIncoming packet:\n";
        std::cout << "Size: " << ntohs(ip_packet->tot_len) << " bytes\n";
        std::cout << "Source: " << (char *)inet_ntoa(source_socket_address.sin_addr) << "\n";
        std::cout << "Destination: " << (char *)inet_ntoa(dest_socket_address.sin_addr) << "\n";
        std::cout << "Identification: " << ntohs(ip_packet->id) << "\n";

        struct ethhdr *eth = (struct ethhdr *)(buffer.data());
        printf("\nEthernet Header\n");
        printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
        printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
        printf("\t|-Protocol : %d\n",eth->h_proto);

        std::string msg{buffer.data() + sizeof(struct iphdr) + sizeof(struct ethhdr), buffer.data() + size};
        std::cout << msg << "\n";
    }
}
