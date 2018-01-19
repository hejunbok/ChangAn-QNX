#include "variant.h"

#include <QRegExp>
#include <QDebug>
#include <QJsonDocument>

using namespace QPps;

static QByteArray fromQJsonDocument(const QJsonDocument &value)
{
#if QT_VERSION >= 0x050100
    return value.toJson(QJsonDocument::Compact);
#else
    QByteArray data = value.toJson();

    QString strData = QString::fromLocal8Bit(data);
    strData.replace(QRegExp(QString::fromLocal8Bit("(\\r\\n|\\n|\\r)")), QString::fromLocal8Bit(""));
    strData = strData.replace(QRegExp(QString::fromLocal8Bit("\\{\\s+")), QString::fromLocal8Bit("{"));
    strData = strData.replace(QRegExp(QString::fromLocal8Bit(",\\s+")), QString::fromLocal8Bit(","));
    strData = strData.replace(QRegExp(QString::fromLocal8Bit(":\\s+")), QString::fromLocal8Bit(":"));

    return strData.toLocal8Bit();
#endif
}


Variant::Variant()
    : m_isValid(false)
{
}

Variant::Variant(const QByteArray &value, const QByteArray &encoding)
    : m_value(value)
    , m_encoding(encoding)
    , m_isValid(true)
{
}

Variant::Variant(const QString &stringValue)
    : m_value(stringValue.toLocal8Bit())
    , m_encoding()
    , m_isValid(true)
{
}

Variant::Variant(const QByteArray &value)
    : m_value(value.toBase64())
    , m_encoding(QByteArrayLiteral("b64"))
    , m_isValid(true)
{
}

Variant::Variant(bool value)
    : m_value(value ? QByteArrayLiteral("true") : QByteArrayLiteral("false"))
    , m_encoding("b")
    , m_isValid(true)
{
}

Variant::Variant(int value)
    : m_value(QByteArray::number(value))
    , m_encoding(QByteArrayLiteral("n"))
    , m_isValid(true)
{
}

Variant::Variant(double value)
    : m_value(QByteArray::number(value))
    , m_encoding(QByteArrayLiteral("n"))
    , m_isValid(true)
{
}

Variant::Variant(const QJsonObject& value)
    : m_value(fromQJsonDocument(QJsonDocument(value)))
    , m_encoding(QByteArrayLiteral("json"))
    , m_isValid(true)
{
}

Variant::Variant(const QJsonDocument &value)
    : m_value(fromQJsonDocument(value))
    , m_encoding(QByteArrayLiteral("json"))
    , m_isValid(true)
{
}

bool Variant::isValid() const
{
    return m_isValid;
}

QByteArray Variant::value() const
{
    return m_value;
}

QByteArray Variant::encoding() const
{
    return m_encoding;
}

QString Variant::toString() const
{
    if (!m_encoding.isEmpty() && m_encoding != QByteArrayLiteral("s"))
        qWarning("Converting to string but encoding is not \"s\" or empty: encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    if (!m_isValid || m_value.isNull())
        return QString();
    else
        return QString::fromLocal8Bit(m_value);
}

QByteArray Variant::toByteArray() const
{
    if (m_encoding != QByteArrayLiteral("b64"))
        qWarning("Converting from base64-encoded bytearray but encoding is not \"b64\": encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    return QByteArray::fromBase64(m_value);
}

bool Variant::toBool() const
{
    if (m_encoding != QByteArrayLiteral("b"))
        qWarning("Converting to bool but encoding is not \"b\": encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    return (m_value == QByteArrayLiteral("true"));
}

int Variant::toInt(bool *ok) const
{
    if (m_encoding != QByteArrayLiteral("n"))
        qWarning("Converting to int but encoding is not \"n\": encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    return m_value.toInt(ok);
}

double Variant::toDouble(bool *ok) const
{
    if (m_encoding != QByteArrayLiteral("n"))
        qWarning("Converting to double but encoding is not \"n\": encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    return m_value.toDouble(ok);
}

QJsonDocument Variant::toJson(QJsonParseError *error) const
{
    if (m_encoding != QByteArrayLiteral("json"))
        qWarning("Converting to QJsonDocument but encoding is not \"json\": encoding: \"%s\" value: \"%s\"", m_encoding.constData(), m_value.constData());

    return QJsonDocument::fromJson(m_value, error);
}

bool Variant::operator==(const Variant &other) const
{
    return m_isValid == other.m_isValid && m_value == other.m_value && m_encoding == other.m_encoding;
}

bool Variant::operator!=(const Variant &other) const
{
    return !operator==(other);
}

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug dbg, const Variant& attribute)
{
    return dbg << attribute.value();
}
#endif
