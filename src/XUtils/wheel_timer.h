#ifndef TIME_WHEEL_TIMER_H
#define TIME_WHEEL_TIMER_H

#include <sys/socket.h>
#include <netinet/in.h>

class wheel_timer;

struct client_wheel_timer
{
    sockaddr_in address;
    int sockfd;
};

class node_wheel_timer
{
public:
    node_wheel_timer(int rot, int st, client_wheel_timer _client, void (*_func)(client_wheel_timer)):rotation(rot), slot_time(st), client(_client), prev(nullptr), next(nullptr),_cb_func(_func) 
    {}
public:
    int rotation;
    int slot_time;
    void (*_cb_func)(client_wheel_timer);
    client_wheel_timer client;
    node_wheel_timer* prev;
    node_wheel_timer* next;

};

class wheel_timer
{
public:
    wheel_timer();
    ~wheel_timer();
    void add_timer(int time, client_wheel_timer _client, void (*_cb_func)(client_wheel_timer));
    void delete_timer(const struct client_wheel_timer _clinet);
    void adjust_timer(int time, const struct client_wheel_timer _client);
    void tick();
private:
    int m_curSlot;
    static const int N = 60;
    static const int SI = 1;
    node_wheel_timer* m_slots[N];
};

#endif
