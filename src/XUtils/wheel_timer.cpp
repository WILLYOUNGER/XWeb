#include "wheel_timer.h"

wheel_timer::wheel_timer()
{
    m_curSlot = 0;
    for(int i = 0; i < N; i++)
    {
        m_slots[i] = nullptr;
    }
}

wheel_timer::~wheel_timer()
{
    for (int i = 0; i < N; i++)
    {
        if (m_slots[i] != nullptr)
        {
            node_wheel_timer *temp = m_slots[i];
            while(temp)
            {
                m_slots[i] = temp->next;
                delete temp;
                temp = m_slots[i];
            }
        }
    }
}

void wheel_timer::add_timer(int time, client_wheel_timer _client, void (*_cb_func)(client_wheel_timer))
{
    if (time < 0)
    {
        return;
    }
    int tick = 0;
    if (time < 1)
    {
        tick = 1;
    }
    else
    {
        tick = time / SI;
    }
    int ro = tick / N;
    int st = (m_curSlot + (tick % N)) % N;
    node_wheel_timer *_time = new node_wheel_timer(ro, st, _client, _cb_func);
    if (m_slots[st] == nullptr)
    {
        m_slots[st] = _time;
    }
    else
    {
        _time->next = m_slots[st];
        m_slots[st]->prev = _time;
        m_slots[st] = _time;
    }
}

void wheel_timer::adjust_timer(int time, const struct client_wheel_timer _client)
{
    if (time < 0)
    {
        return;
    }
    for (int i = 0; i < N; i ++)
    {
        node_wheel_timer *temp = m_slots[i];
        while (temp)
        {
            if (temp->client.sockfd == _client.sockfd)
            {
                int tick = 0;
                if (time < 1)
                {
                    tick = 1;
                }
                else
                {
                    tick = time / SI;
                }
                int ro = tick / N;
                int sl = (m_curSlot + (tick % N)) % N;
                temp->rotation = ro;
                temp->slot_time = sl;
            }
            else
            {
                temp = temp->next;
            }
        }
    }
}

void wheel_timer::delete_timer(const struct client_wheel_timer _client)
{
    for (int i = 0; i < N; i++)
    {
        node_wheel_timer *temp = m_slots[i];
        while (temp)
        {
            if (temp->client.sockfd == _client.sockfd)
            {
                if (temp == m_slots[i])
                {
                    if (temp->next)
                    {
                        temp->next->prev = nullptr;
                        m_slots[i] = temp->next;
                        delete temp;
                        return;
                    }
                    delete temp;
                    m_slots[i] = nullptr;
                    return;
                }
                else
                {
                    if (temp->next)
                    {
                        temp->next->prev = temp->prev;
                        temp->prev->next = temp->next;
                        delete temp;
                        return;
                    }
                    else
                    {
                        temp->prev->next = nullptr;
                        delete temp;
                        return;
                    }
                }
            }
            else
            {
                temp = temp->next;
            }
        }
    }
}

void wheel_timer::tick()
{
    node_wheel_timer* node = m_slots[m_curSlot];
    while (node)
    {
        if (node->rotation > 0)
        {
            node->rotation--;
            node = node->next;
        }
        else
        {
            node->_cb_func(node->client);
            if (node == m_slots[m_curSlot])
            {
                m_slots[m_curSlot] = node->next;
                if (m_slots[m_curSlot])
                {
                    m_slots[m_curSlot]->prev = nullptr;
                }
                delete node;
                node = m_slots[m_curSlot];
            }
            else
            {
                node->prev->next = node->next;
                if (node->next)
                {
                    node->next->prev = node->prev;
                }
                node_wheel_timer* temp = node->next;
                delete node;
                node = temp;
            }
        }
    }
    m_curSlot = ++m_curSlot % N;
}
