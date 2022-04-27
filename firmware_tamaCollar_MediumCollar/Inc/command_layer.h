/**
 * @file command_layer.h
 * @brief This file has class declaration for command layer work
 */

#ifndef __COMMAND_LAYER_H
#define __COMMAND_LAYER_H

// Command exec
#define PACKET_BUFF_SIZE            50

#define PKT_LEN_TIME                17

class CmdProcess
{
public:
    void init ();
    char buff [PACKET_BUFF_SIZE];
    int buff_count;
    void detect (char source);
    void start_storing (char rec_char);
    bool incrBuffCount ();
    bool store (char rec_char);

private:
    int _read_counter = 0;
    bool _readCountManage (int count);
};

#endif