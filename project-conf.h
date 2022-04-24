#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#define SIXTOP_CONF_MAX_TRANSACTIONS 2

/* Set to enable TSCH security */
#ifndef WITH_SECURITY
#define WITH_SECURITY 0
#endif /* WITH_SECURITY */

#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 101
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_16_16
#define TSCH_CONF_WITH_SIXTOP 1
#define TSCH_CONF_BURST_MAX_LEN 0

// you can use this funtion to finish initialization
#define TSCH_CONF_AUTOSTART 0

#define LOG_CONF_LEVEL_MAC LOG_LEVEL_WARN
#define LOG_CONF_LEVEL_6TOP LOG_LEVEL_WARN
#define LOG_CONF_LEVEL_RPL LOG_LEVEL_WARN

// UDP packet sending interval in seconds
#define PACKET_SENDING_INTERVAL 30

// UDP packet payload size
#define UDP_PLAYLOAD_SIZE 50

// MAX number of re-transmissions
#define TSCH_CONF_MAX_FRAME_RETRIES 3

#endif /* PROJECT_CONF_H_ */
