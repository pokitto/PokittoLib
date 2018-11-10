/// ppp-blinky.h

#include "mbed.h"
#include "sha1.h"

void initializePpp();
int connectedPpp();
void waitForPcConnectString();
void waitForPppFrame();
void determinePacketType();
void sendUdpData();

/// PPP header
typedef struct { // [ff 03 00 21]
    unsigned int address : 8;  // always 0xff
    unsigned int control : 8;  // always 03
    unsigned int protocolR : 16; // byte reversed, 0x0021 for ip
} pppHeaderType;

/// LCP and IPCP header
typedef struct {
    // ppp part
    unsigned int address : 8;  // always 0xff
    unsigned int control : 8;  // always 03
    unsigned int protocolR : 16; // byte reversed, 0x0021 for ip

    // ipcp and lcp part
    unsigned int code : 8; // IPCP and LCP contain a code field which identifies the requested action or response
    unsigned int identifier : 8 ;
    unsigned int lengthR : 16;
    char request [0];
} ipcpHeaderType;

/// IP header
typedef struct {
    unsigned int headerLength   :  4;  // ip headerlength / 4
    unsigned int version        :  4;  // ip version number
    unsigned int ect            :  1;  // ecn capable transport
    unsigned int ce             :  1;  // ecn-ce
    unsigned int dscp           :  6;  // differentiated services
    unsigned int lengthR        : 16;  // ip packet length (byte-reversed)

    unsigned int identR         : 16;  // ident, byte reversed
    unsigned int fragmentOffsHi :  5;
    unsigned int lastFragment   :  1;
    unsigned int dontFragment   :  1;
    unsigned int reservedIP     :  1;
    unsigned int fragmentOffsLo :  8;

    unsigned int ttl            :  8;
    unsigned int protocol       :  8;  // next protocol
    unsigned int checksumR      : 16;  // ip checksum, byte reversed
    union {
        unsigned int srcAdrR; // source IP address
        char srcAdrPtr [0]; // so we also have a char * to srcAdrR
    };
    union {
        unsigned int dstAdrR; // destination IP address
        char dstAdrPtr [0];  // so we also have a char * to dstAdrR
    };
} ipHeaderType;

/// IP pseudoheader. Used in TCP/UDP checksum calculations.
typedef struct {
    union {
        char start [0]; // a char * to avoid type conversions
        unsigned int srcAdrR; // source IP address
    };
    unsigned int dstAdrR; // destination IP address
    unsigned int zero     :  8;
    unsigned int protocol :  8;
    unsigned int lengthR  : 16; // byte reversed
} pseudoIpHeaderType;

/// TCP header
typedef struct {
    unsigned int    srcPortR : 16; // byte reversed
    unsigned int    dstPortR : 16; // byte reversed
    unsigned int    seqTcpR;      // byte reversed
    unsigned int    ackTcpR;      // byte reversed
    unsigned int resvd1  : 4;  // reserved
    unsigned int offset  : 4; // tcp header length [5..15]
    union {
        unsigned char All;  // all 8 flag bits
        struct {            // individual flag bits
            unsigned char fin:  1, // fin
                     syn    :  1, // syn
                     rst    :  1, // rst
                     psh    :  1, // psh
                     ack    :  1, // ack
                     urg    :  1, // urg
                     ece    :  1, // ece
                     cwr    :  1; // cwr
        };
    } flag;
    unsigned int windowR : 16; // byte reversed
    unsigned int    checksumR : 16; // byte reversed
    unsigned int    urgentPointerR : 16; // byte reversed;
    unsigned int    tcpOptions[10]; // up to 10 words of options possible
} tcpHeaderType;

/// UDP header.
typedef struct {
    unsigned int srcPortR  : 16; // byte reversed
    unsigned int dstPortR  : 16; // byte reversed
    unsigned int lengthR   : 16; // byte reversed
    unsigned int checksumR : 16; // byte reversed
    char data [0]; // data area
} udpHeaderType;

/// ICMP header.
typedef struct {
    unsigned int type : 8;
    unsigned int code : 8;
    unsigned int checkSumR : 16; // byte reversed
    unsigned int idR : 16; // byte reversed
    unsigned int sequenceR : 16; // byte reversed
    char data [0]; // data area
} icmpHeaderType;

/// Structure to manage all ppp variables.
typedef struct pppType {
    union {
        pppHeaderType * ppp; // pointer to ppp structure
        ipcpHeaderType * ipcp; // pointer to ipcp structure
        ipcpHeaderType * lcp; // pointer to lcp structure (same as ipcp)
    };
    union {
        ipHeaderType * ip; // pointer to ip header struct
        char * ipStart; // char pointer to ip header struct (need a char pointer for byte offset calculations)
    };
    union { // a union for the packet type contained in the IP packet
        tcpHeaderType  * tcp;  // pointer to tcp header struct
        udpHeaderType  * udp;  // pointer to udp header struct
        icmpHeaderType * icmp; // pointer to udp header struct
        char * tcpStart;  // char pointer to tcp header struct  (need a char pointer for byte offset calculations)
        char * udpStart;  // char pointer to udp header struct  (need a char pointer for byte offset calculations)
        char * icmpStart; // char pointer to icmp header struct (need a char pointer for byte offset calculations)
    };
    char * tcpData; // char pointer to where tcp data starts
    int online; // we hunt for a PPP connection if this is zero
    int hostIP; // ip address of host
    int fcs; // PPP "frame check sequence" - a 16-bit HDLC-like checksum used in all PPP frames
    int ledState; // state of LED1
    int responseCounter;
    int firstFrame; // cleared after first frame
    unsigned int sum; // a checksum used in headers
    struct {
#define RXBUFLEN (1<<11)
        // the serial port receive buffer and packet buffer, size is RXBUFLEN (currently 2048 bytes)
        char buf[RXBUFLEN]; // RXBUFLEN MUST be a power of two because we use & operator for fast wrap-around in ring buffer
        volatile int head; // declared volatile so user code knows this variable changes in the interrupt handler
        int tail;
        int rtail;
        int buflevel;
    } rx; // serial port objects
    struct {
        int len; // number of bytes in buffer
        int crc; // PPP CRC (frame check)
#define PPP_max_size 1600
        // we are assuming 100 bytes more than MTU size of 1500
        char buf[PPP_max_size]; // send and receive buffer large enough for largest IP packet
    } pkt; // ppp buffer objects
    struct {
        int frameStartIndex; // frame start marker
        int frameEndIndex; // frame end marker
    } hdlc; // hdlc frame objects
    struct {
        unsigned int ident; // our IP ident value (outgoing frame count)
    } ipData; // ip related object
} pppVariables;

