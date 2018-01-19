#ifndef QTQNXCAR2_UTIL_H
#define QTQNXCAR2_UTIL_H

#include <QDebug>

#define PROPERTY_WITH_NOTIFY(Type,name,setName,changedSignal) \
    public: \
        Type name() const { return m_##name; } \
    public Q_SLOTS: \
        void setName(const Type &value) { \
            if (m_##name == value) \
                return; \
            m_##name = value; \
            emit changedSignal(value); \
        } \
    Q_SIGNALS: \
        void changedSignal(const Type &value); \
    private: \
        Type m_##name;

/// qDebug() convenience macros
#define car2_debug() qDebug() << Q_FUNC_INFO

#endif
