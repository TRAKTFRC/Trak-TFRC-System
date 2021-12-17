/**
 * @file command_layer.h
 * @brief This file has class declaration for command layer work
 */

#ifndef __COMMAND_LAYER_H
#define __COMMAND_LAYER_H

// Command exec
#define PACKET_BUFF_SIZE            20

#define PKT_LEN_TIME                15

class CmdProcessPods
{
public:
    void init ();
    char buff [PACKET_BUFF_SIZE];
    int buff_count;
    void detect ();
    void start_storing (char rec_char);
    bool incrBuffCount ();
    bool store (char rec_char);

private:
    bool _readCountManage (int count);
};

#endif