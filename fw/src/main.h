#ifndef _MAIN
#define _MAIN

void main_signals_handler(int sig);
void seek_event_from_vec();
char get_end_of_operation_pkt(int* client_socket_ptr);
char decode_end_of_op_pkt(char* pkt, int* client_socket_ptr);
void send_end_of_op_ack(char pkt_type, char control_byte, int* client_socket_ptr);
int GetNumOfChannels(int argc, char** argv);
#endif