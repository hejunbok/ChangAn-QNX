#ifndef PALETTE_H
#define PALETTE_H

#include "qtqnxcar_ui_export.h"

#include "util/Util.h"

#include <QColor>
#include <QHash>

namespace QnxCarUi {

class QTQNXCAR_UI_EXPORT PaletteData
{
public:
    QColor color(int role) const;
    void setColor(int role, const QColor &color);
    bool unsetColor(int role);

    bool operator==(const PaletteData &other) const;
    bool operator!=(const PaletteData &other) const;

    bool isEmpty() const;

private:
    friend QDebug operator<<(QDebug dbg, const PaletteData &o);

    QHash<int, QColor> m_colorMap;
};

QDebug operator<<(QDebug dbg, const PaletteData &o);

class QTQNXCAR_UI_EXPORT PaletteParser : public QObject
{
    Q_OBJECT

public:
    explicit PaletteParser(const QVariant &data, QObject *parent = 0);

    PaletteData parse() const;

private:
    QVariant m_data;
};

/**
 * Note: Ideally PaletteData should use ColorRole as well,
 * however we cannot share the enums (e.g. put it into a completely separate
 * QObject subclass) because of:
 *
 * https://bugreports.qt-project.org/browse/QTBUG-20639
 */
class QTQNXCAR_UI_EXPORT PaletteItem : public QObject
{
    Q_OBJECT

    Q_ENUMS(ColorRole)

    Q_PROPERTY(QnxCarUi::PaletteData palette READ palette WRITE setPalette NOTIFY paletteChanged)
    PROPERTY_WITH_NOTIFY(QnxCarUi::PaletteData, palette, setPalette, paletteChanged)

public:
    enum ColorRole {
        /// Used by applauncher module
        AppBarItem,
        AppBarItemSelected,

        /// Default text color role
        Text,
        TextPressed,
        /// Text color for disabled UI elements
        DisabledText,
        ///Theme quirk, prevents the wrong colour for disbaled button text on virtual mechanic
        ButtonTextDisabled,
        /// Background color in menu sections
        SectionBackground,
        /// Foreground (Text) color in menu sections
        SectionForeground,
        MenuHighlight,
        MenuHighlightedText,
        DropDown,
        DropDownPressed,
        //Need this for theme inconsistancy in titanium
        DropDownPersonalization,

        /// Background for menus such as the sheet menu / overlay panes
        PaneBackground,

        PushButton,
        PushButtonPressed,
        PushButtonSelected,

        /// Used for gradiant on special keyboard keys
        SpecialKeyTop,
        SpecialKeyMid,
        SpecialKeyBot,
        SpecialKeyText,
        /// Used for gradiant on letter keyboard keys
        LetterKeyTop,
        LetterKeyBot,
        LetterKeyText,
        /// Used for gradiant on service keyboard keys
        ServiceKeyTop,
        ServiceKeyBot,
        ServiceKeyText,
        /// Used for gradiant on number keyboard keys
        NumberKeyTop,
        NumberKeyMid,
        NumberKeyBot,
        NumberKeyText,
        /// Used for gradiant on keyboard
        KeyboardBackgroundTop,
        KeyboardBackgroundMid,
        KeyboardBackgroundBot,

        /// Used by navigator grid item titles, menu bar items
        MenuTitle,
        ///Used for the search field background (media and navigation)
        SearchFieldBackground,
        /// Used for the dialog header text, we need this value for the bw theme since
        ///there are white headers with otherwise white text
        DialogTitleText,
        ///Used for a gradient in the Communications call dialog
        DialogGradientStart,
        DialogGradientEnd,
        ///Used for a quirk in the themes, this prevents and white text on white background issue
        HeaderTitle,
        ///Theme quirk, prevents the wrong colours on the app home buttons
        AppGridButtonText,
        AppGridButtonTextActive,
        ///Call Dialog border color
        CallBorderColor,
        NoRole
    };

    Q_INVOKABLE QColor color(const QnxCarUi::PaletteItem::ColorRole &role) const;

    explicit PaletteItem(QObject* parent = 0);

    static ColorRole fromString(const QString &str);
};

}

Q_DECLARE_METATYPE(QnxCarUi::PaletteData)

#endif
