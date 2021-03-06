
#include <EtherShield.h>
#include <GB_Datalink.h>
#include <Genibus.h>


#define GB_MASTER_ADDRESS  0x01

#define BUFFER_SIZE 550
static uint8_t buffer[BUFFER_SIZE + 1];

byte macAddress[] = { 0x90, 0xa2, 0xda, 0x00, 0x69, 0x4b  };

static uint8_t subnet[] = { 255, 255, 255, 0 };
#define LOCAL_PORT  6734

/* TODO: The IP-configuration has to be adjusted to your needs!!! */
static uint8_t myIP[] = { 192, 168, 1, 3 };         // for Linux users: adjust the two first line on the file /etc/hosts:
static uint8_t serverIP[] = { 192, 168, 1, 2 };     // IP address of "localhost" must be the same as "IPAddress serverIP", for example "192.168.1.2  localhost"
//IPAddress gateway = (192,168, 1, 1);  // and, 2nd line, "192.168.1.2   'my computer name'"
int EN = 2;                             // RS485 has a enable/disable pin to transmit or receive data.
                                        // Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive
int LED_PIN = 9;                        /* Pin 13 has an LED connected on most Arduino boards, otherwise this should be changed. */
                                        /* Pin 9: led connected on Arduino Ethernet board*/

//EthernetServer server(LOCAL_PORT);
EtherShield es = EtherShield();
//EthernetClient client;

boolean alreadyConnected = false;

void setup(void)
{
    es.ES_enc28j60SpiInit();
    es.ES_enc28j60Init(macAddress);
    es.ES_init_ip_arp_udp_tcp(macAddress, myIP, LOCAL_PORT);

    delay(1000); // give the Ethernet shield a second to initialize:.

    Serial.begin(9600);

    pinMode(EN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    setTxMode();
}

void frameReceived(uint8 * rcvBuffer, uint8 len)
{
    uint16 dataPointer;
    setTxMode();    // Switch back to TX mode.
    delay(500);
    digitalWrite(LED_PIN, LOW);
    
    dataPointer = es.ES_fill_tcp_data_len(buffer, 0, (const char *)rcvBuffer, len);
    es.ES_www_server_reply(buffer, dataPointer);
}

void errorCallout(Gb_Error error, uint8 * rcvBuffer, uint8 len)  //  Needs latest software version to compile!!!
{
    uint16 dataPointer;
    //Serial.print("CRC-Error\n\r"); // Only a single cause of error right now.
    setTxMode();    // Switch back to TX mode.
    delay(500);
    digitalWrite(LED_PIN, LOW);
    
    dataPointer = es.ES_fill_tcp_data_len(buffer, 0, (const char *)rcvBuffer, len);
    es.ES_www_server_reply(buffer, dataPointer);
}

GB_Datalink link(Serial, frameReceived, errorCallout);

byte header[4];
byte apdus[0xff];

void loop(void)
{
    uint16 dataPointer;
    uint16 length;
    
    length = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buffer);
    dataPointer = es.ES_packetloop_icmp_tcp(buffer, length);

    if (dataPointer > 0) {
        setTxMode();    // We need to be able to write the received telegram later on.
        digitalWrite(LED_PIN, HIGH);
        // TAP#1 - Indicates basic TCP/IP connectivity.
        processRequest(dataPointer, length);
    }
    delay(50);
}


void serialEvent(void)
{
    // receive data
    //digitalWrite(LED_PIN, LOW);
    // TAP#3 - Pump received a correct request and is now answering.
    link.feed();
}


void writeToClient(char const * data, byte len)
{
    uint16_t dataPointer;
    
    dataPointer = es.ES_fill_tcp_data_len(buffer, 0, data, len);
    es.ES_www_server_reply(buffer, dataPointer);
}

void processRequest(uint16_t offset, uint16_t length)
{
    byte totalLength = 0;
    byte remainingBytes;
    char ch;
    uint16_t idx;
    
    for (idx = 0; idx < length; ++idx) {
      ch = buffer[offset+idx];
      link.write(ch);      
            if (totalLength == 0x01) {  // Length byte.
                remainingBytes = (byte)ch + 2;

            } else if (totalLength > 1) {
                remainingBytes -= 1;

                if (remainingBytes == 0) {

                    //Serial.print("TCP frame completed");
                    //Serial.print("\n\r");
                    link.reset();
		    delay(2); 	    // !!!
                    setRxMode();    // TCP frame completely received and written to RS485, now receiving response.
                    return;
                }
            }
            totalLength += 1;      
    }           
}

void setRxMode(void)
{
    digitalWrite(EN, LOW);//Enable RS485 Receiving Data
    digitalWrite(LED_PIN, LOW);
}

void setTxMode(void)
{
    digitalWrite(EN, HIGH);//Enable RS485 Receiving Data
    digitalWrite(LED_PIN, LOW);
}

