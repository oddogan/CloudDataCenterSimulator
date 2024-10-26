#ifndef RESERVABLE_PRIO_Q_H
#define RESERVABLE_PRIO_Q_H

#include <queue>

template <typename T>
class ReservablePrioQ : public std::priority_queue<T, std::vector<T>>
{
    public:
        void reserve(size_t size)
        {
            this->c.reserve(size);
        }
};

#endif
