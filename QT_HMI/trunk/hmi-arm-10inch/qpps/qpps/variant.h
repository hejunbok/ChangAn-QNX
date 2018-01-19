#ifndef QPPS_VARIANT_H
#define QPPS_VARIANT_H

#include "qpps_export.h"

#include <QByteArray>

class QJsonDocument;
class QJsonObject;
struct QJsonParseError;
class QString;

namespace QPps {

/**
 * \brief The value of a PPS attribute with type information
 *
 * PPS attribute values consist of the raw value and an encoding, both strings.
 * The encoding indicates what the attribute value's "real" type is, and how to translate between
 * the string representation and the real type.
 *
 * PPS does not standardize any encodings, but a few are common in practice:
 * \a b for bool, \a n for numbers (floating point or integer), \a b64 for binary data
 * (corresponding to QByteArray), \a json for JSON format (corresponding to QJsonDocument),
 * \a s for string.
 *
 * This class handles the listed encodings.
 */
class QPPS_EXPORT Variant
{
public:
    /// Constructs an invalid attribute (isValid() returns false).
    Variant();

    /// Constructs a Variant containing an arbitrary type that you must encode to QByteArray
    /// \p value before passing it in, together with the \p encoding used.
    /// \attention \p encoding and \p value may \em not contain null or '\n' byte values; additionally,
    /// \p encoding may not contain ':'.
    Variant(const QByteArray &value, const QByteArray &encoding);

    /// Constructs a Variant containing a string (empty encoding).
    Variant(const QString &stringValue);

    /// Constructs a Variant containing a QByteArray ("b64" encoding).
    Variant(const QByteArray &value);

    /// Constructs a Variant containing a bool ("b" encoding).
    Variant(bool value);

    /// Constructs a Variant containing an int ("n" encoding).
    Variant(int value);

    /// Constructs a Variant containing a double ("n" encoding).
    Variant(double value);

    /// Constructs a Variant containing a QJsonObject ("json" encoding).
    Variant(const QJsonObject &value);

    /// Constructs a Variant containing a QJsonDocument ("json" encoding).
    Variant(const QJsonDocument &value);

    /// Returns true if the variant is valid, false otherwise.
    /// So far only the default constructor can create invalid variant.
    bool isValid() const;

    /// Returns the raw value.
    QByteArray value() const;

    /// Returns the encoding, indicating how to transform the raw string value to a different type.
    /// If the encoding is empty, the raw value is probably meant to be used as is.
    QByteArray encoding() const;

    /// Like value(), but checks that the encoding is either empty or "s".
    QString toString() const;

    /// Converts to a QByteArray (encoding "b64").
    QByteArray toByteArray() const;

    /// Converts to bool (encoding "b").
    bool toBool() const;

    /**
     * Converts to int (encoding "n").
     *
     * \param ok If not null: If a conversion error occurs, *ok is set to false, otherwise *ok is set to true
     */
    int toInt(bool *ok=0) const;

    /**
     * Converts to double (encoding "n").
     * \param ok If not null: If a conversion error occurs, *ok is set to false, otherwise *ok is set to true
     */
    double toDouble(bool *ok=0) const;

    /**
     * Converts to a JSON document (encoding "json").
     * \param error If not null: If a parsing error occurs, *error contains the details
     */
    QJsonDocument toJson(QJsonParseError *error=0) const;

    /// Tests whether \p other is equal to this variant (validity, encoding and value)
    bool operator==(const Variant &other) const;

    /// Tests whether \p other is unequal to this variant (validity, encoding and value)
    bool operator!=(const Variant &other) const;

private:
    QByteArray m_value;
    QByteArray m_encoding;
    bool m_isValid;
};

}

#if !defined(QT_NO_DEBUG_STREAM)
QPPS_EXPORT QDebug operator<<(QDebug dbg, const QPps::Variant& variant);
#endif

#endif
