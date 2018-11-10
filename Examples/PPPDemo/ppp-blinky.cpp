// PPP-Blinky - "The Most Basic Internet Thing"
// A Tiny HTTP Webserver Using Windows XP/7/8/10/Linux Dial-Up Networking Over A Serial Port.
// Receives UDP packets and responds to ping (ICMP Echo requests)
// WebSocket Service - see https://en.wikipedia.org/wiki/WebSocket

// Copyright 2016/2017 Nicolas Nackel aka Nixnax. Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Notes and Instructions
// http://bit.ly/PPP-Blinky-Instructions
// http://bit.ly/win-rasdial-config

// Important - ensure that you are using the latest mbed firmware for your specific mbed target board

// Handy reading material
// https://technet.microsoft.com/en-us/library/cc957992.aspx
// https://en.wikibooks.org/wiki/Serial_Programming/IP_Over_Serial_Connections
// http://atari.kensclassics.org/wcomlog.htm

// Handy tools
// https://ttssh2.osdn.jp/index.html.en - Tera Term, a good terminal program to monitor the debug output from the second serial port with!
// https://www.microsoft.com/en-us/download/details.aspx?id=4865 - Microsoft network monitor - real-time monitoring of PPP packets
// http://pingtester.net/ - nice tool for high rate ping testing
// http://www.sunshine2k.de/coding/javascript/crc/crc_js.html - Correctly calculates the 16-bit FCS (crc) on our frames (Choose CRC16_CCITT_FALSE), then custom relected-in=1, reflected-out=1
// https://technet.microsoft.com/en-us/sysinternals/pstools.aspx - psping for fast testing of ICMP ping function
// https://eternallybored.org/misc/netcat/ - use netcat -u 172.10.10.1 80 to send/receive UDP packets from PPP-Blinky
// Windows Powershell invoke-webrequest command - use it to stress test the webserver like this:  while (1){ invoke-webrequest -uri 172.10.10.1/x }

// Connecting PPP-Blinky to Linux
// PPP-Blinky can be made to talk to Linux - tested on Fedora - the following command, which uses pppd, works:
// pppd /dev/ttyACM0 115200 debug dump local passive noccp novj nodetach nocrtscts 172.10.10.1:172.10.10.2
// in the above command 172.10.10.1 is the adapter IP, and 172.10.10.2 is the IP of PPP-Blinky.
// See also https://en.wikipedia.org/wiki/Point-to-Point_Protocol_daemon

// Special pages when PPP-Blinky is running
// 172.10.10.2  root page
// 172.10.10.2/x  returns the number of ppp frames sent - this is handy for testing
// 172.10.10.2/xb  also returns number of ppp frames sent, but issues a fast refresh meta command. This allows you to use your browser to benchmark page load speed
// 172.10.10.2/ws  a simple WebSocket demo
// http://jsfiddle.net/d26cyuh2/  more complete WebSocket demo in JSFiddle, showing cross-domain access

// Ok, enough talking, time to check out some code!!

#include "ppp-blinky.h"
#include "USBSerial.h"

// The #define below enables/disables a second (OPTIONAL) serial port that prints out interesting diagnostic messages.
// Change to SERIAL_PORT_MONITOR_YES to enable diagnostics messages. You need to wire a second serial port to your mbed hardware to monitor the debug output.
// Using the second serial port will slow down packet response time
// Note - the LPC11U24 does NOT have a second serial port

#define SERIAL_PORT_MONITOR_NO /* change to SERIAL_PORT_MONITOR_YES for debug messages */

// here we define the OPTIONAL, second debug serial port for various mbed target boards
#ifdef SERIAL_PORT_MONITOR_YES
#if defined(TARGET_LPC1768)
RawSerial xx(p9, p10); // Second serial port on LPC1768 - not required to run, if you get compile error here, change #define SERIAL_PORT_MONITOR_YES to #define SERIAL_PORT_MONITOR_NO
#elif defined(TARGET_NUCLEO_F446RE) || defined(TARGET_NUCLEO_L152RE) || defined(TARGET_NUCLEO_L053R8) || defined(TARGET_NUCLEO_L476RG) || defined(TARGET_NUCLEO_F401RE)
RawSerial xx(PC_10, PC_11); // Second serial port on NUCLEO boards - not required to run, if you get compile error here, change #define SERIAL_PORT_MONITOR_YES to #define SERIAL_PORT_MONITOR_NO
#elif defined(TARGET_NUCLEO_L432KC)
RawSerial xx(PA_9, PA_10); // Second serial port on NUCLEO-L432KC - not required to run, if you get compile error here, change #define SERIAL_PORT_MONITOR_YES to #define SERIAL_PORT_MONITOR_NO
#elif defined(TARGET_LPC11U24)
#error The LPC11U24 does not have a second serial port to use for debugging - change SERIAL_PORT_MONITOR_YES back to SERIAL_PORT_MONITOR_NO
#elif defined (TARGET_KL46Z) || (TARGET_KL25Z)
RawSerial xx(PTE0,PTE1); // Second serial port on FRDM-KL46Z board
#elif defined(TARGET_KW41Z)
RawSerial xx(PTC3, PTC2); // Second serial port on FRDM-KW41Z board
#elif defined(YOUR_TARGET_BOARD_NAME_HERE)
// change the next line to YOUR target board's second serial port pin definition if it's not present - and if it works, please send it to me - thanks!!!
RawSerial xx(p9, p10); // change this to YOUR board second serial port pin definition - and please send it to me if it works!!!
#else
#error Add your target board's second serial port here if you want to use debugging - or simply change SERIAL_PORT_MONITOR_YES to SERIAL_PORT_MONITOR_NO
#endif
#define debugPrintf(x...) xx.printf (x) /* if we have a serial port we can print debug messages */
#define debugPutc(x...) xx.putc(x)
#define debugBaudRate(x...) xx.baud(x)
#else
// if we don't have a debug port the debug print functions do nothing
#define debugPrintf(x...) {}
#define debugPutc(x...) {}
#define debugBaudRate(x...) {}
#endif

// verbosity flags used in debug printouts - change to 1 to see increasingly more detailed debug info.
#define v0 1
#define v1 0
#define v2 0
#define IP_HEADER_DUMP_YES /* YES for ip header dump */
#define TCP_HEADER_DUMP_YES /* YES for tcp header dump */

// this is the webpage we serve when we get an HTTP request to root (/)
// keep size under ~900 bytes to fit into a single PPP packet

const static char rootWebPage[] = "\
<!DOCTYPE html>\
<html>\
<head>\
<title>mbed PPP-Blinky</title>\r\n\
<script>\r\n\
window.onload=function(){\
setInterval(function(){function x(){return document.getElementById('w');};\
x().textContent=parseInt(x().textContent)+1;},100);};\r\n\
</script>\r\n\
</head>\
<body style=\"font-family: sans-serif; font-size:20px; text-align:center; color:#807070\">\
<h1>mbed PPP-Blinky Up and Running</h1>\
<h1 id=\"w\">0</h1>\
<h1><a href=\"http://bit.ly/pppBlink2\">Source on mbed</a></h1>\
<h1><a href=\"/ws\">WebSocket Demo</a></h1>\
<h1><a href=\"/x\">Benchmark 1</a></h1>\
<h1><a href=\"/xb\">Benchmark 2</a></h1>\
<h1><a href=\"http://jsfiddle.net/d26cyuh2/\">JSFiddle Demo</a></h1>\
</body>\
</html>\r\n"; // size = 644 bytes plus 1 null byte = 645 bytes

// this is a websocket demo html page we serve when GET /ws is requested
const static char webSocketPage[] = "\
<!DOCTYPE html>\
<html>\
<head>\
<title>mbed PPP-Blinky</title>\
<script>\
window.onload=function(){\
 var url=\"ws://172.10.10.2\";\
 var sts=document.getElementById(\"sts\");\
 var btn=document.getElementById(\"btn\");\
 var ctr=0;\
 function show(text){sts.textContent=text;}\
 btn.onclick=function(){\
  if(btn.textContent==\"Connect\"){\
   x=new WebSocket(url);\
    x.onopen=function(){\
    show(\"Connected to : \"+url);\
    btn.textContent=\"Send \\\"\"+ctr+\"\\\"\";\
   };\
  x.onclose=function(){show(\"closed\");};\
  x.onmessage=function(msg){show(\"PPP-Blinky Sent: \\\"\"+msg.data+\"\\\"\");};\
  } else {\
   x.send(ctr);\
   ctr=ctr+1;\
   btn.textContent=\"Send \\\"\"+ctr+\"\\\"\";\
  }\
 };\
};\
</script>\
<body style=\"font-family: sans-serif; font-size:25px; color:#807070\">\
<h1>PPP-Blinky WebSocket Test</h1>\
<div id=\"sts\">Idle</div>\
<button id=\"btn\" style=\"font-size: 100%; margin-top: 55px; margin-bottom: 55px;\">Connect</button>\
<h4><a href=\"/\">PPP-Blinky home</a></h4>\
</body>\
</html>"; // size = 916 bytes + 1 null byte = 917 bytes

// The serial port on your mbed hardware. Your PC should be configured to view this port as a standard dial-up networking modem.
// On Windows the model type of the modem should be selected as "Communications cable between two computers"
// The modem baud rate should be set to 115200 baud
// See instructions at the top.
// On a typical mbed hardware platform this serial port is a USB virtual com port (VCP) and the USB serial driver is supplied by the board vendor.
//RawSerial pc (USBTX, USBRX); // usb virtual com port for mbed hardware

//RawSerial pc (USBTX, USBRX); // usb virtual com port for mbed hardware
USBSerial pc;

DigitalOut led1(LED1); // this led toggles when a packet is received

// the standard hdlc frame start/end character. It's the tilde character "~"
#define FRAME_7E (0x7e)

pppType ppp; // our global - definitely not thread safe

/// Initialize the ppp structure and clear the receive buffer
void pppInitStruct()
{
    memset( ppp.rx.buf, 0, RXBUFLEN);
    ppp.online=0;
    ppp.rx.tail=0;
    ppp.rx.rtail=0;
    ppp.rx.head=0;
    ppp.rx.buflevel=0;
    ppp.pkt.len=0;
    ppp.ipData.ident=10000; // easy to recognize in ip packet dumps
    ppp.ledState=0;
    ppp.hdlc.frameStartIndex=0;
    ppp.responseCounter=0;
    ppp.firstFrame=1;
    ppp.ppp = (pppHeaderType *)ppp.pkt.buf; // pointer to ppp header
    ppp.ip = (ipHeaderType *)(ppp.pkt.buf+4); // pointer to IP header
}

/// Toggle the LED on every second PPP packet received
void led1Toggle()
{
    led1 = (ppp.ledState >> 1) & 1; // use second bit, in other words toggle LED only every second packet
    ppp.ledState++;
}

/// Returns 1 after a connect message, 0 at startup or after a disconnect message
int connectedPpp()
{
    return ppp.online;
}

/// Previously used to check for characters in receive buffer.
/// Now just a stub.
void checkPc() {};

/// PPP serial port receive interrupt handler.
/// Check for available characters from the PC and read them into our own circular serial receive buffer at ppp.rx.buf.
/// Also, if we are offline and a 0x7e frame start character is seen, we go online immediately
void pppReceiveHandler()
{
    char ch;
    while ( pc.readable() ) {
        int hd = (ppp.rx.head+1)&(RXBUFLEN-1); // increment/wrap head index
        if ( hd == ppp.rx.rtail ) {
            debugPrintf("\nReceive buffer full\n");
            return;
        }
        ch = pc.getc(); // read new character
        ppp.rx.buf[ppp.rx.head] = ch; // insert in our receive buffer
        if ( ppp.online == 0 ) {
            if (ch == 0x7E) {
                ppp.online = 1;
            }
        }
        ppp.rx.head = hd; // update head pointer
        ppp.rx.buflevel++;
    }
}

/// print to debug port while checking for incoming characters
void putcWhileCheckingInput( char outByte )
{
#ifdef SERIAL_PORT_MONITOR_YES
    checkPc();
    debugPutc( outByte );
    checkPc();
#endif
}

/// puts to debug port while checking the PPP input stream
void putsWhileCheckingInput( char * data )
{
#ifdef SERIAL_PORT_MONITOR_YES
    char * nextChar = data;
    while( *nextChar != 0 ) {
        putcWhileCheckingInput( *nextChar ); // write one character to debug port while checking input
        nextChar++;
    }
#endif
}

/// Initialize the PPP FCS (frame check sequence) total
void fcsReset()
{
    ppp.fcs=0xffff;   // crc restart
}

/// update the cumulative PPP FCS (frame check sequence)
void fcsDo(int x)
{
    for (int i=0; i<8; i++) {
        ppp.fcs=((ppp.fcs&1)^(x&1))?(ppp.fcs>>1)^0x8408:ppp.fcs>>1; // crc calculator
        x>>=1;
    }
    checkPc(); // handle input
}

/// calculate the PPP FCS (frame check sequence) on an entire block of memory
int fcsBuf(char * buf, int size) // crc on an entire block of memory
{
    fcsReset();
    for(int i=0; i<size; i++)fcsDo(*buf++);
    return ppp.fcs;
}

/// Get one character from our received PPP buffer
int pc_getBuf()
{
    int x = ppp.rx.buf[ ppp.rx.tail ];
    ppp.rx.tail=(ppp.rx.tail+1)&(RXBUFLEN-1);
    ppp.rx.buflevel--;
    return x;
}

/// Dump a PPP frame to the debug serial port
/// Note - the hex output of dumpPPPFrame() can be imported into WireShark
/// Capture the frame's hex output in your terminal program and save as a text file
/// In WireShark, use "Import Hex File". Import options are: Offset=None, Protocol=PPP.
void dumpPPPFrame()
{
    char pbuf[30];
    for(int i=0; i<ppp.pkt.len; i++) {
        checkPc();
        sprintf(pbuf, "%02x ", ppp.pkt.buf[i]);
        checkPc();
        putsWhileCheckingInput(pbuf);
    }
    checkPc();
    sprintf(pbuf, " CRC=%04x Len=%d\n", ppp.pkt.crc, ppp.pkt.len);
    checkPc();
    putsWhileCheckingInput(pbuf);
}

/// Process a received PPP frame
void processPPPFrame(int start, int end)
{
    led1Toggle(); // change led1 state on every frame we receive
    if(start==end) {
        return; // empty frame
    }
    fcsReset();
    char * dest = ppp.pkt.buf;
    ppp.pkt.len=0;
    int unstuff=0;
    int idx = start;
    while(1) {
        checkPc();
        if (unstuff==0) {
            if (ppp.rx.buf[idx]==0x7d) unstuff=1;
            else {
                *dest = ppp.rx.buf[idx];
                ppp.pkt.len++;
                dest++;
                fcsDo(ppp.rx.buf[idx]);
            }
        } else { // unstuff characters prefixed with 0x7d
            *dest = ppp.rx.buf[idx]^0x20;
            ppp.pkt.len++;
            dest++;
            fcsDo(ppp.rx.buf[idx]^0x20);
            unstuff=0;
        }
        idx = (idx+1) & (RXBUFLEN-1);
        if (idx == end) break;
    }
    ppp.pkt.crc = ppp.fcs & 0xffff;
    if(0) dumpPPPFrame(); // set to 1 to dump ALL ppp frames
    if (ppp.pkt.crc == 0xf0b8) { // check for good CRC
        determinePacketType();
    } else {
#define REPORT_FCS_ERROR_YES
#ifdef REPORT_FCS_ERROR_YES
        char pbuf[50]; // local print buffer
        checkPc();
        sprintf(pbuf, "\nPPP FCS(crc) Error CRC=%x Length = %d\n",ppp.pkt.crc,ppp.pkt.len); // print a debug line
        checkPc();
        putsWhileCheckingInput( pbuf );
        if(0) dumpPPPFrame(); // set to 1 to dump frames with errors in them
#endif
    }
}

/// output a character to the PPP port while checking for incoming characters
void pcPutcWhileCheckingInput(int ch)
{
    checkPc(); // check input
    pc.putc(ch);
    checkPc();
}

/// do PPP HDLC-like handling of special (flag) characters
void hdlcPut(int ch)
{
    if ( (ch<0x20) || (ch==0x7d) || (ch==0x7e) ) {
        pcPutcWhileCheckingInput(0x7d);
        pcPutcWhileCheckingInput(ch^0x20);  // these characters need special handling
    } else {
        pcPutcWhileCheckingInput(ch);
    }
}

/// send a PPP frame in HDLC format
void sendPppFrame()
{
    ppp.responseCounter++; // count the number of ppp frames we send
    int crc = fcsBuf(ppp.pkt.buf, ppp.pkt.len-2); // update crc
    ppp.pkt.buf[ ppp.pkt.len-2 ] = (~crc>>0); // fcs lo (crc)
    ppp.pkt.buf[ ppp.pkt.len-1 ] = (~crc>>8); // fcs hi (crc)
    pcPutcWhileCheckingInput(0x7e); // hdlc start-of-frame "flag"
    for(int i=0; i<ppp.pkt.len; i++) {
        wait_us(86); // wait one character time
        checkPc();
        hdlcPut( ppp.pkt.buf[i] ); // send a character
    }
    pcPutcWhileCheckingInput(0x7e); // hdlc end-of-frame "flag"
}

/// convert a network ip address in the buffer to an integer (IP adresses are big-endian, i.e most significant byte first)
int bufferToIP(char * buffer)
{
    int result=0;
    for(int i=0; i<4; i++) result = (result<<8)|(*buffer++ & 0xff);
    return result;
}

/// convert 4-byte ip address to 32-bit
unsigned int ip( int a, int b, int c, int d)
{
    return a<<24 | b<<16 | c<<8 | d;
}

/// handle IPCP configuration requests
void ipcpConfigRequestHandler()
{
    debugPrintf("Their IPCP Config Req, Our Ack\n");
    if(ppp.ipcp->request[0]==3) {
        ppp.hostIP = bufferToIP(ppp.pkt.buf+10);
        debugPrintf("Host IP = %d.%d.%d.%d (%08x)\n", ppp.ipcp->request[2],ppp.ipcp->request[3],ppp.ipcp->request[4],ppp.ipcp->request[5],ppp.hostIP);
    }

    ppp.ipcp->code=2; // change code to ack
    sendPppFrame(); // acknowledge everything they ask for - assume it's IP addresses

    debugPrintf("Our IPCP Ask (no options)\n");
    ppp.ipcp->code=1; // change code to request
    ppp.ipcp->lengthR = __REV16( 4 ); // 4 is minimum length - no options in this request
    ppp.pkt.len=4+4+2; // no options in this request shortest ipcp packet possible (4 ppp + 4 ipcp + 2 crc)
    sendPppFrame(); // send our request
}

/// handle IPCP acknowledge (do nothing)
void ipcpAckHandler()
{
    debugPrintf("Their IPCP Grant\n");
}

/// Handle IPCP NACK by sending our suggested IP address if there is an IP involved.
/// This is how Linux responds to an IPCP request with no options - Windows assumes any IP address on the submnet is OK.
void ipcpNackHandler()
{
    debugPrintf("Their IPCP Nack\n");
    if (ppp.ipcp->request[0]==3) { // check if the NACK contains an IP address parameter
        ppp.ipcp->code=1; // assume the NACK contains our "suggested" IP address
        sendPppFrame(); // let's request this IP address as ours
        debugPrintf("Our IPCP ACK (received an IP)\n");
    } else { // if it's not an IP nack we ignore it
        debugPrintf("IPCP Nack Ignored\n");
    }
}

/// handle all other IPCP requests (by ignoring them)
void ipcpDefaultHandler()
{
    debugPrintf("Their IPCP Other\n");
}

/// process an incoming IPCP packet
void IPCPframe()
{
    int action = ppp.ipcp->code; // packet type is here
    switch (action) {
        case 1:
            ipcpConfigRequestHandler();
            break;
        case 2:
            ipcpAckHandler();
            break;
        case 3:
            ipcpNackHandler();
            break;
        default:
            ipcpDefaultHandler();
    }
}

/// perform a 16-bit checksum. if the byte count is odd, stuff in an extra zero byte.
unsigned int dataCheckSum(char * ptr, int len, int restart)
{
    unsigned int i,hi,lo;
    unsigned char placeHolder;
    if (restart) ppp.sum=0;
    if (len&1) {
        placeHolder = ptr[len];
        ptr[len]=0;  // if the byte count is odd, insert one extra zero byte is after the last real byte because we sum byte PAIRS
    }
    i=0;
    while ( i<len ) {
        checkPc();
        hi = ptr[i++];
        lo = ptr[i++];
        ppp.sum = ppp.sum + ((hi<<8)|lo);
    }
    if (len&1) {
        ptr[len] = placeHolder;    // restore the extra byte we made zero
    }
    ppp.sum = (ppp.sum & 0xffff) + (ppp.sum>>16);
    ppp.sum = (ppp.sum & 0xffff) + (ppp.sum>>16); // sum one more time to catch any carry from the carry
    return ~ppp.sum;
}

/// perform the checksum on an IP header
void IpHeaderCheckSum()
{
    ppp.ip->checksumR=0; // zero the checsum in the IP header
    int len = 4 * ppp.ip->headerLength; // length of IP header in bytes
    unsigned int sum = dataCheckSum(ppp.ipStart,len,1);
    ppp.ip->checksumR = __REV16( sum ); // insert fresh checksum
}

/// swap the IP source and destination addresses
void swapIpAddresses()
{
    unsigned int tempHold;
    tempHold = ppp.ip->srcAdrR; // tempHold <- source IP
    ppp.ip->srcAdrR = ppp.ip->dstAdrR; // source <- dest
    ppp.ip->dstAdrR = tempHold; // dest <- tempHold*/
}

/// swap the IP source and destination ports
void swapIpPorts()
{
    int headerSizeIP    = 4 * (ppp.ip->headerLength); // calculate size of IP header
    char * ipSrcPort = ppp.ipStart + headerSizeIP + 0; // ip source port location
    char * ipDstPort = ppp.ipStart + headerSizeIP + 2; // ip destin port location
    char tempHold[2];
    memcpy(tempHold, ipSrcPort,2); // tempHold <- source
    memcpy(ipSrcPort,ipDstPort,2); // source <- dest
    memcpy(ipDstPort,tempHold, 2); // dest <- tempHold
}

/// Build the "pseudo header" required for UDP and TCP, then calculate its checksum
void checkSumPseudoHeader( unsigned int packetLength )
{
    // this header  contains the most important parts of the IP header, i.e. source and destination address, protocol number and data length.
    pseudoIpHeaderType pseudoHeader; // create pseudo header
    pseudoHeader.srcAdrR = ppp.ip->srcAdrR; // copy in ip source address
    pseudoHeader.dstAdrR = ppp.ip->dstAdrR; // copy in ip dest address
    pseudoHeader.zero = 0; // zero byte
    pseudoHeader.protocol = ppp.ip->protocol; // protocol number (udp or tcp)
    pseudoHeader.lengthR = __REV16( packetLength ); // size of tcp or udp packet
    dataCheckSum(pseudoHeader.start, 12, 1); // calculate this header's checksum
}

/// initialize an IP packet to send
void initIP (unsigned int srcIp, unsigned int dstIp, unsigned int srcPort, unsigned int dstPort, unsigned int protocol)
{
    ppp.ppp->address = 0xff;
    ppp.ppp->control = 3;
    ppp.ppp->protocolR = __REV16( 0x0021 );
    ppp.ip->version = 4;
    ppp.ip->headerLength = 5; // 5 words = 20 bytes
    ppp.ip->identR = __REV16(ppp.ipData.ident++); // insert our ident
    ppp.ip->dontFragment=1;
    ppp.ip->ttl=128;
    ppp.ip->protocol = protocol; // udp
    ppp.ip->srcAdrR = __REV(srcIp);
    ppp.ip->dstAdrR = __REV(dstIp);
    ppp.udpStart = ppp.ipStart + 20; // calculate start of udp header
    ppp.udp->srcPortR = __REV16(srcPort); // source port
    ppp.udp->dstPortR = __REV16(dstPort); // dest port
}


/// Build a UDP packet from scratch
void sendUdp(unsigned int srcIp, unsigned int dstIp, unsigned int srcPort, unsigned int dstPort, char * message,int msgLen)
{
    struct {
        unsigned int ipAll; // length of entire ip packet
        unsigned int ipHeader; // length of ip header
        unsigned int udpAll; // length of entire udp packet
        unsigned int udpData; // length of udp data segment
    } len;
    len.ipHeader = 20; // ip header length
    len.udpData = msgLen; // udp data size
    len.udpAll = len.udpData+8; // update local udp packet length
    len.ipAll = len.ipHeader + len.udpAll; // update IP Length
    initIP(srcIp, dstIp, srcPort, dstPort, 17); // init a UDP packet
    ppp.ip->lengthR = __REV16(len.ipAll); // update IP length in buffer
    ppp.udpStart = ppp.ipStart + len.ipHeader; // calculate start of udp header
    memcpy( ppp.udp->data, message, len.udpData ); // copy the message to the buffer
    ppp.udp->lengthR = __REV16(len.udpAll); // update UDP length in buffer
    ppp.pkt.len = len.ipAll+2+4; // update ppp packet length
    IpHeaderCheckSum();  // refresh IP header checksum
    checkSumPseudoHeader( len.udpAll ); // get the UDP pseudo-header checksum
    ppp.udp->checksumR = 0; // before TCP checksum calculations the checksum bytes must be set cleared
    unsigned int pseudoHeaderSum=dataCheckSum(ppp.udpStart,len.udpAll, 0); // continue the TCP checksum on the whole TCP packet
    ppp.udp->checksumR = __REV16( pseudoHeaderSum); // tcp checksum done, store it in the TCP header
    sendPppFrame(); // send the UDP message back
}

/// Process an incoming UDP packet.
/// If the packet starts with the string "echo " or "test" we echo back a special packet
void UDPpacket()
{
    struct {
        unsigned int all; // length of entire ip packet
        unsigned int header; // length of ip header
    } ipLength;

    struct {
        unsigned int all; // length of entire udp packet
        unsigned int data; // length of udp data segment
    } udpLength;

    ipLength.header = 4 * ppp.ip->headerLength; // length of ip header
    ppp.udpStart = ppp.ipStart + ipLength.header; // calculate start of udp header
    udpLength.all = __REV16( ppp.udp->lengthR ); // size of udp packet
    udpLength.data = udpLength.all - 8; // size of udp data

#ifdef SERIAL_PORT_MONITOR_YES
    char * srcIP        = ppp.ip->srcAdrPtr; // IP source
    char * dstIP        = ppp.ip->dstAdrPtr; //IP destination

    unsigned int udpSrcPort = __REV16( ppp.udp->srcPortR ); // integer of UDP source port
    unsigned int udpDstPort = __REV16( ppp.udp->dstPortR ); // integer of UDP dest port

    if(v0) {
        debugPrintf("UDP %d.%d.%d.%d:%d ", srcIP[0],srcIP[1],srcIP[2],srcIP[3],udpSrcPort);
        debugPrintf("%d.%d.%d.%d:%d ",     dstIP[0],dstIP[1],dstIP[2],dstIP[3],udpDstPort);
        debugPrintf("Len %03d", udpLength);
    }
    if (v1) {
        int printSize = udpLength.data;
        if (printSize > 20) printSize = 20; // print only first 20 characters
        for (int i=0; i<printSize; i++) {
            char ch = ppp.udp->data[i];
            if (ch>31 && ch<127) {
                debugPrintf("%c", ch);
            } else {
                debugPrintf("_");
            }
        }
    }
    if (v0) debugPrintf("\n");
#endif
    int echoFound = !strncmp(ppp.udp->data,"echo ",5); // true if UDP message starts with "echo "
    int testFound = !strncmp(ppp.udp->data,"test" ,4); // true if UDP message starts with "test"
    if ( (echoFound) || (testFound)) { // if the UDP message starts with "echo " or "test" we answer back
        if (echoFound) {
            swapIpAddresses(); // swap IP source and destination
            swapIpPorts(); // swap IP source and destination ports
            memcpy(ppp.udp->data,"Got{",4); // in the UDP data modify "echo" to "Got:"
            int n=0;
            n=n+sprintf(n+ppp.udp->data+udpLength.data, "} UDP Server: PPP-Blinky\n"); // an appendix
            udpLength.data = udpLength.data + n; // update udp data size with the size of the appendix
            // we may have changed data length, update all the lengths
            udpLength.all    = udpLength.data+8; // update local udp packet length
            ipLength.all     = ipLength.header + udpLength.all; // update IP Length
            ppp.ip->lengthR  = __REV16(ipLength.all); // update IP length in buffer
            ppp.udp->lengthR = __REV16(udpLength.all); // update UDP length in buffer
            ppp.pkt.len      = ipLength.all+2+4; // update ppp packet length
            IpHeaderCheckSum();  // refresh IP header checksum
            checkSumPseudoHeader( udpLength.all ); // get the UDP pseudo-header checksum
            ppp.udp->checksumR = 0; // before TCP checksum calculations the checksum bytes must be set cleared
            unsigned int pseudoHeaderSum=dataCheckSum(ppp.udpStart,udpLength.all, 0); // continue the TCP checksum on the whole TCP packet
            ppp.udp->checksumR = __REV16( pseudoHeaderSum); // tcp checksum done, store it in the TCP header
            sendPppFrame(); // send the UDP message back
        } else if ( testFound ) {
            unsigned int sI = __REV( ppp.ip->srcAdrR );
            unsigned int dI = __REV( ppp.ip->dstAdrR );
            unsigned int sp = __REV16( ppp.udp->srcPortR );
            unsigned int dp = __REV16( ppp.udp->dstPortR );
            int n=sprintf(ppp.pkt.buf+200,"Response Count %d\n", ppp.responseCounter);
            sendUdp(dI,sI,dp,sp,ppp.pkt.buf+200,n); // build a udp packet from the ground up
        }
    }
}

/// UDP demo that sends a udp packet containing a character received from the second debug serial port.
/// Sends a 48 byte IP/UDP header for every 1 byte of data so line-mode would probably be better.
/// If you want ip packets from ppp blinky to be routed to other networks, ensure you have ip routing enabled.
/// See http://www.wikihow.com/Enable-IP-Routing.
/// Also ensure that the firewall on the receiving machine has the receiving UDP port (12345 in this example) enabled.
/// The netcat UDP receive command on the remote host would be: nc -ul 12345
void sendUdpData()
{
#ifdef SERIAL_PORT_MONITOR_YES
    if (ppp.online) {
        if (xx.readable()) {
            char inchar = xx.getc();
            xx.putc( inchar ); // echo the received character on the debug serial port
            sendUdp(ip(172,10,10,2), ip(192,168,0,109), 1, 12345, &inchar, 1); // send a 1 byte UDP message to a remote machine at IP 192.168.0.109:12345
        }
    }
#endif
}

/// handle a PING ICMP (internet control message protocol) packet
void ICMPpacket()   // internet control message protocol
{
    struct {
        unsigned int all; // length of entire ip packet
        unsigned int header; // length of ip header
    } ipLength;
    struct {
        unsigned int all; // length of entire udp packet
        unsigned int data; // length of udp data segment
    } icmpLength;
    ipLength.all = __REV16( ppp.ip->lengthR );  // length of ip packet
    ipLength.header = 4 * ppp.ip->headerLength; // length of ip header
    ppp.icmpStart = ppp.ipStart + ipLength.header; // calculate start of udp header
    icmpLength.all = ipLength.all - ipLength.header; // length of icmp packet
    icmpLength.data = icmpLength.all - 8; // length of icmp data
#define ICMP_TYPE_PING_REQUEST 8
    if ( ppp.icmp->type == ICMP_TYPE_PING_REQUEST ) {
        ppp.ip->ttl--; // decrement time to live (so we have to update header checksum)
#ifdef SERIAL_PORT_MONITOR_YES
        char * srcAdr = ppp.ip->srcAdrPtr;
        char * dstAdr = ppp.ip->dstAdrPtr;
        int icmpIdent = __REV16( ppp.ip->identR ); // byte reversed - big endian
        int icmpSequence = __REV16( ppp.icmp->sequenceR ); // byte reversed - big endian
        if(1) {
            char pbuf[50];
            checkPc();
            sprintf(pbuf, "ICMP PING %d.%d.%d.%d %d.%d.%d.%d ", srcAdr[0],srcAdr[1],srcAdr[2],srcAdr[3],dstAdr[0],dstAdr[1],dstAdr[2],dstAdr[3]);
            putsWhileCheckingInput( pbuf );
            checkPc();
            sprintf(pbuf, "Ident %04x Sequence %04d \n",icmpIdent,icmpSequence);
            checkPc();
            putsWhileCheckingInput( pbuf );
        }
#endif
        swapIpAddresses(); // swap the IP source and destination addresses
        IpHeaderCheckSum();  // new ip header checksum (required because we changed TTL)
#define ICMP_TYPE_ECHO_REPLY 0
        ppp.icmp->type = ICMP_TYPE_ECHO_REPLY; // icmp echo reply
        ppp.icmp->checkSumR = 0; // zero the checksum for recalculation
        unsigned int sum = dataCheckSum(ppp.icmpStart, icmpLength.all, 1); // icmp checksum
        ppp.icmp->checkSumR = __REV16( sum ); // save big-endian icmp checksum

        int printSize = icmpLength.data; // exclude size of icmp header
        if (printSize > 10) printSize = 10; // print up to 20 characters
        if (0) {
            for (int i=0; i<printSize; i++) {
                char ch = ppp.icmp->data[i];
                if (ch>31 && ch<127) {
                    putcWhileCheckingInput(ch);
                } else {
                    putcWhileCheckingInput('_');
                }
            }
            putcWhileCheckingInput('\n');
        }
        sendPppFrame(); // reply to the ping
    } else {
        if (v0) {
            debugPrintf("ICMP type=%x \n", ppp.icmp->type);
        }
    }
}

/// handle an IGMP (internet group managment protocol) packet (by ignoring it)
void IGMPpacket()
{
    if (v0) debugPrintf("IGMP type=%d \n", ppp.pkt.buf[28]);
}

/// dump the header of an IP pakcet on the (optional) debug serial port
void dumpHeaderIP (int outGoing)
{
#if defined(IP_HEADER_DUMP_YES) && defined(SERIAL_PORT_MONITOR_YES)
    checkPc(); // we are expecting the first character of the next packet
    int IPv4Id = __REV16(ppp.ip->identR);
    char pbuf[80]; // local print buffer
    int n=0;
    n=n+sprintf(pbuf+n, outGoing ? "\x1b[34m" : "\x1b[30m" ); // VT100 color code, print black for incoming, blue for outgoing headers
    n=n+sprintf(pbuf+n, "%05d ",IPv4Id); // IPv4Id is a good way to correlate our dumps with net monitor or wireshark traces
#define DUMP_FULL_IP_ADDRESS_YES
#ifdef DUMP_FULL_IP_ADDRESS_YES
    char * srcAdr = ppp.ip->srcAdrPtr;
    char * dstAdr = ppp.ip->dstAdrPtr;
    n=n+sprintf(pbuf+n, " %d.%d.%d.%d %d.%d.%d.%d ",srcAdr[0],srcAdr[1],srcAdr[2],srcAdr[3], dstAdr[0],dstAdr[1],dstAdr[2],dstAdr[3]); // full ip addresses
#endif
    putsWhileCheckingInput( pbuf );
#ifndef TCP_HEADER_DUMP_YES
    putsWhileCheckingInput('\x1b[30m\n'); // if there's no TCP header dump we terminate the line with \n and VT100 code for black
#endif
#endif
}

/// dump a TCP header on the optional debug serial port
void dumpHeaderTCP(int outGoing)
{
#if defined(TCP_HEADER_DUMP_YES) && defined(SERIAL_PORT_MONITOR_YES)
    char flagString[9]; // text string presenting the 8 most important TCP flags
#define PRINT_ALL_TCP_FLAGS_YES
#ifdef PRINT_ALL_TCP_FLAGS_YES
    memset(flagString,'.', 8); // fill string with "........"
    if (ppp.tcp->flag.fin) flagString[7]='F';
    if (ppp.tcp->flag.syn) flagString[6]='S';
    if (ppp.tcp->flag.rst) flagString[5]='R';
    if (ppp.tcp->flag.psh) flagString[4]='P';
    if (ppp.tcp->flag.ack) flagString[3]='A';
    if (ppp.tcp->flag.urg) flagString[2]='U';
    if (ppp.tcp->flag.ece) flagString[1]='E';
    if (ppp.tcp->flag.cwr) flagString[0]='C';
    flagString[8]=0; // null terminate string
#else
    if (ppp.tcp->flag.ack) flagString[0]='A'; // choose only the most important flag to print
    if (ppp.tcp->flag.syn) flagString[0]='S';
    if (ppp.tcp->flag.fin) flagString[0]='F';
    if (ppp.tcp->flag.psh) flagString[0]='P';
    if (ppp.tcp->flag.rst) flagString[0]='R';
    flagString[1]=0; // null terminate string
#endif
    putsWhileCheckingInput( flagString );
#define EVERY_PACKET_ON_A_NEW_LINE_YES
#ifdef EVERY_PACKET_ON_A_NEW_LINE_YES
    putsWhileCheckingInput("\x1b[30m\n"); // write a black color and newline after every packet
#else
    putsWhileCheckingInput("\x1b[30m"); // write a black color after every packet
#endif
    if( outGoing && ppp.tcp->flag.fin ) { // ACK/FIN - if this is an outgoing FIN it's the end of a tcp conversation
        putcWhileCheckingInput('\n'); // insert an extra new line to mark the end (except for final ack) of an HTTP conversation
    }
#endif
}

/// Encode a buffer in base-64
const static char lut [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
void enc64(char * in, char * out, int len)
{
    int i,j,a,b,c;
    i=0;
    j=0;
    while(1) {
        if (i<len) {
            a = in[i++];
            out[j++] = lut[ ( (a >> 2) & 0x3f) ];
        } else break;
        if (i<len) {
            b = in[i++];
            out[j++] = lut[ ( (a << 4) & 0x30) | ( (b >> 4) & 0x0f) ];
            out[j++] = lut[ ( (b << 2) & 0x3c)  ];
        } else out[j++] = '=';
        if (i<len) {
            c = in[i++];
            j--;
            out[j++] = lut[ ( (b << 2) & 0x3c) | ( (c >> 6) & 0x03) ];
            out[j++] = lut[ ( (c >> 0) & 0x3f) ];
        } else out[j++] = '=';
    }
    out[j]=0;
}

/// Handle a request for an http websocket.
/// We end up here if we enter the following javascript in a web browser console: x = new WebSocket("ws://172.10.10.2");
int webSocketHandler(char * dataStart)
{
    int n=0; // byte counter
    char * key = strstr(dataStart, "Sec-WebSocket-Key:"); // search for the key in the payload
    if (key != NULL) {
        key = key + 18; // skip over the key ident string "Sec-WebSocket-Key:"
        if (v0) putsWhileCheckingInput("WebSocket Request\n");
        while ( strchr(lut, *key) == NULL) key++; // skip non-valid base-64 characters (whitespace)
        char challenge [80];
        int i=0;
        char * copyTo = challenge;
        while (strchr(lut, *key) != NULL) { // copy while we see valid base-64 characters
            if (i++ >40) break; // prevent buffer overflow
            *copyTo++ = *key++; // copy next valid base-64 character
        }
        strcpy(copyTo,"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"); // append websocket gui code
        if (0) debugPrintf("Challenge is %s\n", challenge); // the string we hash for the challenge
        char shaOutput [20]; // sha1 output
        sha1( shaOutput, challenge, strlen(challenge)); // hash the challenge
        char encOut[50];
        enc64( shaOutput, encOut, 20); // base-64 encode
        char * versionstring = strstr(dataStart, "Sec-WebSocket-Version:");
        char * version = challenge;
        strncpy(version, versionstring,70); // copy their version string
        *strchr(version,'\r')=0; // null terminate so we can sprintf it
        memset(dataStart,0,500); // blank out old data befor send the websocket response header
        n=n+sprintf(dataStart+n, "HTTP/1.1 101 Switching Protocols\r\n");
        n=n+sprintf(dataStart+n, "Upgrade: websocket\r\n");
        n=n+sprintf(dataStart+n, "Connection: Upgrade\r\n");
        n=n+sprintf(dataStart+n, "Sec-WebSocket-Accept: %s\r\n",encOut);
        n=n+sprintf(dataStart+n, "%s\r\n",version);
        n=n+sprintf(dataStart+n, "mbed-Code:  PPP-Blinky\r\n");
        n=n+sprintf(dataStart+n, "\r\n"); // websocket response header ending
    }
    return n; // this response should satisfy a web browser's websocket protocol request
}

#define TCP_FLAG_ACK (1<<4)
#define TCP_FLAG_SYN (1<<1)
#define TCP_FLAG_PSH (1<<3)
#define TCP_FLAG_RST (1<<2)
#define TCP_FLAG_FIN (1<<0)

/// respond to an HTTP request
int httpResponse(char * dataStart, int * flags)
{
    int n=0; // number of bytes we have printed so far
    n = webSocketHandler( dataStart ); // test for and handle WebSocket upgrade requests
    if (n>0) return n; // if n>0 we already have a response, so return

    int nHeader; // byte size of HTTP header
    int contentLengthStart; // index where HTML starts
    int httpGet5,httpGet6,httpGetx, httpGetRoot; // temporary storage of strncmp results
    *flags = TCP_FLAG_ACK | TCP_FLAG_FIN; // the default case is that we close the connection

    httpGetRoot = strncmp(dataStart, "GET / HTTP/1.", 13);  // found a GET to the root directory
    httpGetx    = strncmp(dataStart, "GET /x", 6);          // found a GET to /x which we will treat special (anything starting with /x, e.g. /x, /xyz, /xABC?pqr=123
    httpGet5    = dataStart[5]; // the first character in the path name, we use it for special functions later on
    httpGet6    = dataStart[6]; // the second character in the path name, we use it for special functions later on
    // for example, you could try this using netcat (nc):    echo "GET /x" | nc 172.10.10.2
    if( (httpGetRoot==0) || (httpGetx==0) ) {
        n=n+sprintf(n+dataStart,"HTTP/1.1 200 OK\r\nServer: mbed-PPP-Blinky-v1\r\n"); // 200 OK header
    } else {
        n=n+sprintf(n+dataStart,"HTTP/1.1 404 Not Found\r\nServer: mbed-PPP-Blinky\r\n"); // 404 header
    }
    n=n+sprintf(n+dataStart,"Content-Length: "); // http header
    contentLengthStart = n; // remember where Content-Length is in buffer
    n=n+sprintf(n+dataStart,"?????\r\n"); // leave five spaces for content length - will be updated later
    n=n+sprintf(n+dataStart,"Connection: close\r\n"); // close connection immediately
    n=n+sprintf(n+dataStart,"Content-Type: text/html; charset=us-ascii\r\n\r\n"); // http header must end with empty line (\r\n)
    nHeader=n; // size of HTTP header

    if( httpGetRoot == 0 ) {
        // this is where we insert our web page into the buffer
        memcpy(n+dataStart,rootWebPage,sizeof(rootWebPage));
        n = n + sizeof(rootWebPage)-1; // one less than sizeof because we don't count the null byte at the end

    } else if ( (httpGet5 == 'w') && (httpGet6 == 's') ) { // "ws" is a special page for websocket demo
        memcpy(n+dataStart,webSocketPage,sizeof(webSocketPage));
        n = n + sizeof(webSocketPage)-1; // one less than size
        *flags = TCP_FLAG_ACK | TCP_FLAG_PSH; // for a websocket page we do NOT close the connection
    } else {
        if (httpGetx == 0) { // the page request started with "GET /x" - here we treat anything starting with /x special:
#define W3C_COMPLIANT_RESPONSE_NO
// change the above to W3C_COMPLIANT_RESPONSE_YES if you want a W3C.org compliant HTTP response
#ifdef W3C_COMPLIANT_RESPONSE_YES
            n=n+sprintf(n+dataStart,"<!DOCTYPE html><title>mbed PPP-Blinky</title>"); // html title (W3C.org required elements)
            n=n+sprintf(n+dataStart,"<body>%d</body>",ppp.responseCounter); // body = the http frame count
#else
            if( httpGet6 == 'b' ) { // if the fetched page is "xb" send a meta command to let the browser continuously reload
                n=n+sprintf(n+dataStart, "<meta http-equiv=\"refresh\" content=\"0\">"); // reload loop - handy for benchmarking
            }
            // /x is a very short page, in fact, it is only a decimal number showing the http Page count
            n=n+sprintf(n+dataStart,"%d ",ppp.responseCounter); // not really valid html but most browsers and curl are ok with it
#endif
        } else {
            // all other requests get 404 Not Found response with a http frame count - nice for debugging
            n=n+sprintf(n+dataStart,"<!DOCTYPE html><title>mbed PPP-Blinky</title>"); // html title (required element)
            n=n+sprintf(n+dataStart,"<body>Not Found</body>"); // not found message
        }
    }
#define CONTENTLENGTHSIZE 5
    char contentLengthString[CONTENTLENGTHSIZE+1];
    snprintf(contentLengthString,CONTENTLENGTHSIZE+1,"%*d",CONTENTLENGTHSIZE,n-nHeader); // print Content-Length with leading spaces and fixed width equal to csize
    memcpy(dataStart+contentLengthStart, contentLengthString, CONTENTLENGTHSIZE); // copy Content-Length to it's place in the send buffer
    return n; // total byte size of our response
}

/// Handle TCP data that is not an HTTP GET.
/// This is handy when for example you want to use netcat (nc.exe) to talk to PPP-Blinky.
/// This could also be a websocket receive event - especially if the first byte is 0x81 (websocket data push)
int tcpResponse(char * dataStart, int len, int * outFlags)
{
    int n=0; // number of bytes we have printed so far
    if (dataStart[0] == 0x81) { // check if this is a websocket push message
        char mask [4];
        memcpy ( mask, dataStart+2, 4); // websocket messages are "masked", so first we obtain the 4-byte mask
        int websocketMessageSize = len - 6;  // 1 byte prefix (0x81), 1 byte, 4 bytes mask = 6 bytes
        if((dataStart[1]&0x80)==0x80) // test if the mask bit is set, which means all data is xor'ed with the mask
            for (int i=0; i<websocketMessageSize; i++) dataStart[i+6]^= mask[i%4]; // unmask each byte with one of the mask bytes
        dataStart[1] = len-2; // add four extra bytes to the message length because we don't use mask bytes for the send
        memcpy(dataStart+2, "Got:",4); // insert our own text into the four mask bytes
        n = len; // our response size remains exactly the same length as what we received
    } else if ( (dataStart[0]==0x88) && (dataStart[1]==0x80) && (len == 6) ) { // test for a websocket close request
        n=2; // our close command is only two bytes long because we don't use the four mask bytes
        dataStart[1]=0; // we don't have mask bytes on
    } else {
        if ( len > 1 ) { // we assume a length of 1 is a keep-alive or push packet
            if (v1) putsWhileCheckingInput("TCP data received\n"); // all other tcp push packets
        }
    }
    return n; // total byte size of our response
}

/// dump the TCP data to the debug serial port
void dumpDataTCP(int outGoing)
{
#ifdef SERIAL_PORT_MONITOR_YES
    if (v2) {
        int packetLengthIp = __REV16(ppp.ip->lengthR ); // size of ip packet
        int headerSizeIp = 4 * ppp.ip->headerLength;  // size of ip header
        ppp.tcpStart = ppp.ipStart + headerSizeIp; // calculate where the TCP header starts
        int headerSizeTcp = 4 * (ppp.tcp->offset); // tcp "offset" for start of data is also the header size
        ppp.tcpData = ppp.tcpStart + headerSizeTcp; // start of tcp data
        int tcpSize = packetLengthIp - headerSizeIp; // tcp size = size of ip payload
        int tcpDataSize = tcpSize - headerSizeTcp; // size of data block after TCP header
        char pbuf[80]; // local print buffer
        int n=0;
        checkPc();
        n=n+sprintf(pbuf+n, outGoing ? "\x1b[34m" : "\x1b[30m" ); // VT100 color code, print black for incoming, blue for outgoing headers
        checkPc();
        n=n+sprintf(pbuf+n, "IP:%d ipHeader:%d tcpHeader:%d tcpData:%d\n", packetLengthIp, headerSizeIp, headerSizeTcp, tcpDataSize);    // 1 for more verbose
        if (n>70) putsWhileCheckingInput("n>pbuf overflow in dumpDataTCP()\n");
        checkPc();
        putsWhileCheckingInput( pbuf );
        if (tcpDataSize > 0) {
            ppp.tcpData[tcpDataSize]=0; // insert a null after the data so debug printf stops printing after the data
            putsWhileCheckingInput( ppp.tcpData );    // print the tcp payload data
            putsWhileCheckingInput("\n");
        }
        putsWhileCheckingInput( "\x1b[30m" ); // VT100 color code, print black
    }
#endif
}

/// handle an incoming TCP packet
/// use the first few bytes to figure out if it's a websocket, an http request or just pure incoming TCP data
void tcpHandler()
{
    int packetLengthIp = __REV16(ppp.ip->lengthR ); // size of ip packet
    int headerSizeIp = 4 * ppp.ip->headerLength;  // size of ip header
    ppp.tcpStart = ppp.ipStart + headerSizeIp; // calculate TCP header start
    int tcpSize = packetLengthIp - headerSizeIp; // tcp size = size of ip payload
    int headerSizeTcp = 4 * (ppp.tcp->offset); // tcp "offset" for start of data is also the header size
    char * tcpDataIn = ppp.tcpStart + headerSizeTcp; // start of data TCP data after TCP header
    int tcpDataSize = tcpSize - headerSizeTcp; // size of data block after TCP header

    unsigned int seq_in = __REV(ppp.tcp->seqTcpR);
    unsigned int ack_in = __REV(ppp.tcp->ackTcpR);
    unsigned int ack_out = seq_in + tcpDataSize;
    unsigned int seq_out = ack_in; // use their version of our current sequence number

    // first we shorten the TCP response header to only 20 bytes. This means we ignore all TCP option requests
    headerSizeIp=20;
    ppp.ip->headerLength = headerSizeIp/4; // ip header is 20 bytes long
    ppp.ip->lengthR = __REV(40); // 20 ip header + 20 tcp header
    //tcpSize = 20; // shorten total TCP packet size to 20 bytes (no data)
    headerSizeTcp = 20; // shorten outgoing TCP header size 20 bytes
    ppp.tcpStart = ppp.ipStart + headerSizeIp; // recalc TCP header start
    ppp.tcp->offset = (headerSizeTcp/4);
    char * tcpDataOut = ppp.tcpStart + headerSizeTcp; // start of outgoing data

    int dataLen = 0; // most of our responses will have zero TCP data, only a header
    int flagsOut = TCP_FLAG_ACK; // the default case is an ACK packet

    ppp.tcp->windowR = __REV16( 1200 ); // set tcp window size to 1200 bytes

    // A sparse TCP flag interpreter that implements stateless TCP connections

    switch ( ppp.tcp->flag.All ) {
        case TCP_FLAG_SYN:
            flagsOut = TCP_FLAG_SYN | TCP_FLAG_ACK; // something wants to connect - acknowledge it
            seq_out = seq_in+0x10000000U; // create a new sequence number using their sequence as a starting point, increase the highest digit
            ack_out++; // for SYN flag we have to increase the sequence by 1
            break;
        case TCP_FLAG_ACK:
        case TCP_FLAG_ACK | TCP_FLAG_PSH:
            if ( (ppp.tcp->flag.All == TCP_FLAG_ACK) && (tcpDataSize == 0)) return; // handle zero-size ack messages by ignoring them
            if ( (strncmp(tcpDataIn, "GET /", 5) == 0) ) { // check for an http GET command
                flagsOut = TCP_FLAG_ACK | TCP_FLAG_PSH; // we have data, set the PSH flag
                dataLen = httpResponse(tcpDataOut, &flagsOut); // send an http response
            } else {
                dataLen = tcpResponse(tcpDataOut,tcpDataSize, &flagsOut); // not an http GET, handle as a tcp connection
                if (dataLen > 0) flagsOut = TCP_FLAG_ACK | TCP_FLAG_PSH; // if we have any data set the PSH flag
            }
            break;
        case TCP_FLAG_FIN:
        case TCP_FLAG_FIN | TCP_FLAG_ACK:
        case TCP_FLAG_FIN | TCP_FLAG_PSH | TCP_FLAG_ACK:
            flagsOut = TCP_FLAG_ACK | TCP_FLAG_FIN; // set outgoing FIN flag to ask them to close from their side
            ack_out++; // for FIN flag we have to increase the sequence by 1
            break;
        default:
            return; // ignore all other packets
    } // switch

    // The TCP flag handling is now done
    // first we swap source and destination TCP addresses and insert the new ack and seq numbers
    swapIpAddresses(); // swap IP source and destination addresses
    swapIpPorts(); // swap IP  source and destination ports

    ppp.tcp->ackTcpR = __REV( ack_out ); // byte reversed - tcp/ip messages are big-endian (high byte first)
    ppp.tcp->seqTcpR = __REV( seq_out ); // byte reversed - tcp/ip messages are big-endian (high byte first)

    ppp.tcp->flag.All = flagsOut; // update the TCP flags

    // recalculate all the header sizes
    tcpSize = headerSizeTcp + dataLen; // tcp packet size
    int newPacketSize = headerSizeIp + tcpSize; // calculate size of the outgoing packet
    ppp.ip->lengthR = __REV16 ( newPacketSize );
    ppp.pkt.len = newPacketSize+4+2; // ip packet length + 4-byte ppp prefix (ff 03 00 21) + 2 fcs (crc) bytes bytes at the end of the packet

    // the header is all set up, now do the IP and TCP checksums
    IpHeaderCheckSum(); // calculate new IP header checksum
    checkSumPseudoHeader( tcpSize ); // get the TCP pseudo-header checksum
    ppp.tcp->checksumR = 0; // before TCP checksum calculations the checksum bytes must be set cleared
    unsigned int pseudoHeaderSum=dataCheckSum(ppp.tcpStart,tcpSize, 0); // continue the TCP checksum on the whole TCP packet
    ppp.tcp->checksumR = __REV16( pseudoHeaderSum); // tcp checksum done, store it in the TCP header

    dumpHeaderIP(1); // dump outgoing IP header before sending the frame
    dumpHeaderTCP(1); // dump outgoing TCP header before sending the frame
    dumpDataTCP(1); // dump outgoing TCP data before sending the frame
    if (0) wait_ms(45); // 45 ms delay before sending frame - a typical internet delay time
    sendPppFrame(); // All preparation complete - send the TCP response
    if(0) dumpPPPFrame(); // set to 1 to dump transmitted ppp frame
    memset(ppp.pkt.buf+44,0,500); // flush out traces of previous data that we may scan for
}

/// handle an incoming TCP packet
void TCPpacket()
{
    dumpHeaderIP(0);     // dump incoming packet IP header
    dumpHeaderTCP(0);   // dump incoming packet TCP header
    dumpDataTCP(0); // dump incoming packet data
    tcpHandler();
}

/// handle the remaining IP protocols by ignoring them
void otherProtocol()
{
    debugPrintf("Other IP protocol");
}

/// process an incoming IP packet
void IPframe()
{
    int protocol = ppp.ip->protocol;
    switch (protocol) {
        case    1:
            ICMPpacket();
            break;
        case    2:
            IGMPpacket();
            break;
        case   17:
            UDPpacket();
            break;
        case    6:
            TCPpacket();
            break;
        default:
            otherProtocol();
    }
}

/// respond to LCP (line configuration protocol) configuration request) by allowing no options
void LCPconfReq()
{
    debugPrintf("LCP Config ");
    if ( ppp.lcp->lengthR != __REV16(4) ) {
        ppp.lcp->code=4; // allow only "no options" which means Maximum Receive Unit (MRU) is default 1500 bytes
        debugPrintf("Reject\n");
        sendPppFrame();
    } else {
        ppp.lcp->code=2; // ack zero conf
        debugPrintf("Ack\n");
        sendPppFrame();
        debugPrintf("LCP Ask\n");
        ppp.lcp->code=1; // request no options
        sendPppFrame();
    }
}

/// handle LCP acknowledge packets by ignoring them
void LCPconfAck()
{
    debugPrintf("LCP Ack\n");
}

/// handle LCP end (disconnect) packets by acknowledging them and by setting ppp.online to false
void LCPend()
{
    ppp.lcp->code=6; // end
    sendPppFrame(); // acknowledge
    ppp.online=0; // start hunting for connect string again
    pppInitStruct(); // flush the receive buffer
    debugPrintf("LCP End (Disconnect from host)\n");
}

/// respond to other LCP requests by ignoring them
void LCPother()
{
    debugPrintf("LCP Other\n");
    dumpPPPFrame();
}

/// process incoming LCP packets
void LCPframe()
{
    int code = ppp.lcp->code;
    switch (code) {
        case 1:
            LCPconfReq();
            break; // config request
        case 2:
            LCPconfAck();
            break; // config ack
        case 5:
            LCPend();
            break; // end connection
        default:
            LCPother();
    }
}

/// discard packets that are not IP, IPCP, or LCP
void discardedFrame()
{
    if (v0) debugPrintf("Frame is not IP, IPCP or LCP: %02x %02x %02x %02x\n", ppp.pkt.buf[0],ppp.pkt.buf[1],ppp.pkt.buf[2],ppp.pkt.buf[3]);
}

/// determine the packet type (IP, IPCP or LCP) of incoming packets
void determinePacketType()
{
    if ( ppp.ppp->address != 0xff ) {
        debugPrintf("Unexpected: PPP address != ff\n");
        return;
    }
    if ( ppp.ppp->control != 3 ) {
        debugPrintf("Unexpected: PPP control !=  3\n");
        return;
    }
    unsigned int protocol = __REV16( ppp.ppp->protocolR );
    switch ( protocol ) {
        case 0xc021:
            LCPframe();
            break;  // link control
        case 0x8021:
            IPCPframe();
            break;  // IP control
        case 0x0021:
            IPframe();
            break;  // IP itself
        default:
            discardedFrame();
    }
}

/// a sniffer tool to assist in figuring out where in the code we are having characters in the input buffer
void sniff()
{
    if ( pc.readable() ) putsWhileCheckingInput( "Sniff - Char available!\n" ); // if this prints anything it means there is a character in the serial receive buffer
}

/// scan the PPP serial input stream for frame start markers
void waitForPppFrame()
{
    while(1) {
        sendUdpData(); // handle received characters from the DEBUG TERMINAL
        if ( ppp.rx.head != ppp.rx.tail ) {
            int oldTail = ppp.rx.tail; // remember where the character is located in the buffer
            int rx = pc_getBuf(); // get the character
            if (rx==FRAME_7E) {
                if (ppp.firstFrame) { // is this the start of the first frame start
                    ppp.rx.rtail = ppp.rx.tail; // update real-time tail with the virtual tail
                    ppp.hdlc.frameStartIndex = ppp.rx.tail; // remember where first frame started
                    ppp.firstFrame=0; // clear first frame flag
                }  else {
                    ppp.hdlc.frameEndIndex=oldTail; // mark the frame end character
                    processPPPFrame(ppp.hdlc.frameStartIndex, ppp.hdlc.frameEndIndex); // process the frame
                    ppp.rx.rtail = ppp.rx.tail; // update real-time tail with the virtual tail
                    ppp.hdlc.frameStartIndex = ppp.rx.tail; // remember where next frame started
                    break;
                }
            }
        }
    }
}

/// Wait for a dial-up modem connect command ("CLIENT") from the host PC, if found, we set ppp.online to true, which will start the IP packet scanner.
// Note: a 0x7E in the input stream (ppp start of frame character) will also set ppp.online to true - see checkPc();
void waitForPcConnectString()
{
    while(ppp.online == 0) {
        checkPc(); // gather received characters
        // search for Windows Dialup Networking "Direct Connection Between Two Computers" expected connect string
        char * found1 = strstr( (char *)ppp.rx.buf, "CLIENT" );
        if (found1 != NULL) {
            // respond with Windows Dialup networking expected "Direct Connection Between Two Computers" response string
            if (v0) debugPrintf("Connected: Found connect string \"CLIENT\", sent \"CLIENTSERVER\"\n");
            pc.puts("CLIENTSERVER");
            ppp.online=1; // we are connected - set flag so we stop looking for the connect string
            checkPc();
        }
    }
}

/// Initialize PPP data structure and set serial port(s) baud rate(s)
void initializePpp()
{
    debugBaudRate(115200); // baud rate for (optional) debug serial port
    debugPrintf("\x1b[2J\x1b[H\x1b[30m");
    wait_ms(200); // a brief wait so a human can see the reset event
    debugPrintf("mbed PPP-Blinky HTTP & WebSocket server ready :)\n"); // VT100 codes for clear_screen, home, black_text - Tera Term is a handy VT100 terminal

    pppInitStruct(); // initialize all the variables/properties/buffers
    //pc.baud(115200); // pc serial port acting as a dial-up modem - for PPP traffic
    //pc.attach(&pppReceiveHandler, RawSerial::RxIrq); // set up serial port receive interrupt handler
    pc.attach(&pppReceiveHandler); // set up serial port receive interrupt handler
}
