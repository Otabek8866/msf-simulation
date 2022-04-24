#include <contiki.h>
#include <contiki-net.h>

#include <net/mac/tsch/sixtop/sixtop.h>
#include <services/msf/msf.h>

#include <lib/sensors.h>

#include "net/ipv6/simple-udp.h"
#include "lib/random.h"
#include "sys/node-id.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/********** Global variables ***********/
#define UDP_PORT 8765
#define SEND_INTERVAL (PACKET_SENDING_INTERVAL * CLOCK_SECOND)

// data to send to the server
unsigned char custom_payload[UDP_PLAYLOAD_SIZE];


PROCESS(msf_node_process, "MSF node");
AUTOSTART_PROCESSES(&msf_node_process);

// function to populate the payload
void create_payload()
{
  for (uint16_t i = 4; i < UDP_PLAYLOAD_SIZE; i++)
  {
    custom_payload[i] = i % 26 + 'a';
  }
  custom_payload[2] = 0xFF;
  custom_payload[3] = 0xFF;
}

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


PROCESS_THREAD(msf_node_process, ev, data)
{
  // static struct etimer et;
  // static struct udp_socket s;
  // static const uint8_t app_data[] = "data";
  // uip_ipaddr_t root_ipaddr;

  static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  static uint16_t seqnum;
  uip_ipaddr_t dst;

  PROCESS_BEGIN();

  create_payload();

  sixtop_add_sf(&msf);
  
  // printf("APP_SEND_INTERVAL: %u\n", APP_SEND_INTERVAL);
  // etimer_set(&et, APP_SEND_INTERVAL);

  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  
  NETSTACK_MAC.on();

  // if this is a simple node, start sending upd packets
  LOG_INFO("Started UDP communication\n");

  // start the timer for periodic udp packet sending
  etimer_set(&periodic_timer, SEND_INTERVAL);

  // if(udp_socket_register(&s, NULL, NULL) < 0 ||
  //    udp_socket_bind(&s, APP_UDP_PORT) < 0) {
  //   printf("CRITICAL ERROR: socket initialization failed\n");
  // } else {
  //   while(1) {
  //     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  //     etimer_reset(&et);
  //     if(NETSTACK_ROUTING.node_is_reachable() &&
  //        NETSTACK_ROUTING.get_root_ipaddr(&root_ipaddr) &&
  //        msf_is_negotiated_tx_scheduled() &&
  //        udp_socket_sendto(&s, app_data, sizeof(app_data),
  //                          &root_ipaddr, APP_UDP_PORT) > 0) {
  //       // printf("send app data\n");
  //       LOG_INFO("Sent a packet to the root\n");
  //     }
  //   }
  // }

  /* Main UDP comm Loop */
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      if (NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dst))
      {
        /* Send the packet number to the root and extra data */
        custom_payload[0] = seqnum & 0xFF;
        custom_payload[1] = (seqnum >> 8) & 0xFF;
        // LOG_INFO_6ADDR(&dst);
        simple_udp_sendto(&udp_conn, &custom_payload, UDP_PLAYLOAD_SIZE, &dst);
        LOG_INFO("Sent_to %d packet_number: %d\n", dst.u8[15], seqnum);
        seqnum++;
      }
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END();
}
