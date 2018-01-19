#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include "qtqnxcar2_export.h"

#include "PagedMediaNodeModel.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * Model for searching and displaying media search results.
 */
class QTQNXCAR2_EXPORT SearchModel : public PagedMediaNodeModel
{
    Q_OBJECT

    /**
     * The ID of the media source being searched.
     *
     * @accessors mediaSourceId()
     */
    Q_PROPERTY(int mediaSourceId READ mediaSourceId NOTIFY mediaSourceIdChanged)

    /**
     * The search term used to perform the search.
     *
     * @accessors searchTerm()
     */
    Q_PROPERTY(QString searchTerm READ searchTerm NOTIFY searchTermChanged)

    /**
     * The search filter to apply to the search.
     *
     * @accessors filter()
     */
    Q_PROPERTY(QString filter READ filter NOTIFY filterChanged)

    /**
     * Whether a search is in progress.
     *
     * @c true while a search query is running, otherwise @c false
     *
     * @accessors searchInProgress()
     * @sa #search
     */
    Q_PROPERTY(bool searchInProgress READ searchInProgress NOTIFY searchInProgressChanged)

    /**
     * Whether this model contains valid search results, i.e. to differentiate an empty
     * model due to no results, or due to a search being cleared.
     *
     * @accessors searchIsValid()
     * @sa #search, resetSearch
     */
    Q_PROPERTY(bool searchIsValid READ searchIsValid NOTIFY searchIsValidChanged)

public:
    /**
     * Creates an empty search model.
     * @param qPlayer A pointer to a qPlayer instance.
     * @param parent The QObject parent.
     */
    explicit SearchModel(QPlayer::QPlayer *qPlayer, QObject *parent = 0);

    /**
     * Returns the ID of the media source last searched.
     *
     * @sa #mediaSourceId
     */
    int mediaSourceId() const;

    /**
     * Returns the search term used to execute the last search.
     *
     * @sa #searchTerm
     */
    QString searchTerm() const;

    /**
     * Returns the filter used for the last search.
     *
     * @sa #filter
     */
    QString filter() const;

    /**
     * Returns whether a search is running
     *
     * @sa #searchInProgress
     */
    bool searchInProgress() const;

    /**
     * Returns whether there is a valid search result
     * of a previous search.
     *
     * @sa #lastSearchIsValid
     */
    bool searchIsValid() const;

Q_SIGNALS:
    void mediaSourceIdChanged(const int mediaSourceId);
    void searchTermChanged(const QString &searchTerm);
    void filterChanged(const QString &filter);
    void searchInProgressChanged(bool);
    void searchIsValidChanged(bool);

public Q_SLOTS:
    /**
     * @brief Executes a search against the specified media source and changes the #searchInProgress
     * property to @c true. When the search completes, the #searchInProgress property will be changed
     * to @c false.
     * @param mediaSourceId The media source ID.
     * @param searchTerm The search term.
     * @param filter The search result filter.
     */
    Q_INVOKABLE void search(int mediaSourceId, QString searchTerm, QString filter);

    /**
     * Clears the search results and resets the #searchIsValid property to @c false
     */
    void resetSearch();

protected:
    /**
     * @brief Concrete doLoadPage implementation for the SearchModel. Executes a browse command for the page specified
     * using the search result node media source and media node IDs, and appends the results to the model node storage.
     * @param page The number of the page of nodes to load.
     */
    void doLoadPage(int page) const Q_DECL_OVERRIDE;

private:
    class Private;
    Private *d;
};

}
}

#endif // SEARCHMODEL_H
