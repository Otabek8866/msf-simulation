#include <contiki.h>
#include <contiki-net.h>

#include "net/routing/routing.h"
#include "net/mac/tsch/sixtop/sixtop.h"
#include "services/msf/msf.h"

#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT 8765


// function to receive udp packets
static void rx_packet(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
                      uint16_t sender_port, const uip_ipaddr_t *receiver_addr,
                      uint16_t receiver_port, const uint8_t *data, uint16_t datalen)
{
  char received_data[UDP_PLAYLOAD_SIZE];
  memcpy(received_data, data, datalen);

  uint16_t packet_num;
  packet_num = received_data[1] & 0xFF;
  packet_num = (packet_num << 8) + (received_data[0] & 0xFF);
  LOG_INFO("Received_from %d packet_number: %d\n", sender_addr->u8[15], packet_num);
}


PROCESS(msf_root_process, "MSF root");
AUTOSTART_PROCESSES(&msf_root_process);

PROCESS_THREAD(msf_root_process, ev, data)
{
  // static struct udp_socket s;
  static struct simple_udp_connection udp_conn;
  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);

  NETSTACK_ROUTING.root_start();
  NETSTACK_MAC.on();
  sixtop_add_sf(&msf);

  // if(udp_socket_register(&s, NULL, NULL) < 0 ||
  //    udp_socket_bind(&s, APP_UDP_PORT) < 0) {
  //   printf("CRITICAL ERROR: socket initialization failed\n");
  //   /*
  //    * we don't need to process received packets, but we need to have
  //    * a socket so as to prevent sending back ICMP error packets
  //    */
  // } else {
  //   NETSTACK_ROUTING.root_start();
  //   sixtop_add_sf(&msf);
  // }

  PROCESS_END();
}
