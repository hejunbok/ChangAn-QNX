#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <QDebug>

template <typename T>
struct Optional {

    explicit Optional() : set(false), v() {}
    Optional(const T& v_) : set(true), v(v_) {}

    Optional& operator=(const T &v_)
    {
        v = v_;
        set = true;
        return *this;
    }

    bool operator==(const Optional<T> &other) const
    {
        return (set == false && other.set == false) || (set && other.set && v == other.v);
    }

    bool operator!=(const Optional<T> &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Optional<T> &other) const
    {
        if (set != other.set)
            return !set;
        else
            return v < other.v;
    }

    bool operator>(const Optional<T> &other) const
    {
        if (set != other.set)
            return set;
        else
            return v > other.v;
    }

    bool operator>=(const Optional<T> &other) const
    {
        if (set != other.set)
            return set;
        else
            return v >= other.v;
    }

    bool isSet() const
    {
        return set;
    }

    void unSet()
    {
        v = T();
        set = false;
    }

    const T &value() const
    {
        Q_ASSERT(set);
        return v;
    }

    const T &value(const T &defaultValue) const
    {
        if (set)
            return v;
        else
            return defaultValue;
    }

    bool set : 1;
    T v;
};

template<typename T>
QDebug operator<<(QDebug dbg, const Optional<T> &o)
{
    if (!o.isSet())
        return dbg << "<unset>";

    return dbg << o.value();
}

#endif
