#include "SearchModel.h"

#include <QDebug>

#include "TypeConverter.h"

namespace QnxCar {
namespace MediaPlayer {

class SearchModel::Private : public QObject
{
public:
    explicit Private(SearchModel *qq);

    /**
     * @brief Executes the search using the QPlayer library.
     * @param mediaSourceId The media source ID to search.
     * @param searchTerm The search term.
     * @param filter The search result filter.
     */
    void search(const int mediaSourceId, const QString searchTerm, const QString filter);

    /**
     * Clears the search results and resets the #lastSearchIsValid property to @c false.
     */
    void resetSearch();

    SearchModel *const q;

    int mediaSourceId() const;
    QString searchTerm() const;
    QString filter() const;
    bool searchInProgress() const;
    bool searchIsValid() const;

    void setSearchInProgress(bool inProgress);
    void setSearchIsValid(bool isValid);

    void doLoadPage(int page) const;

private:
    void setMediaSourceId(const int mediaSourceId);
    void setSearchTerm(const QString searchTerm);
    void setFilter(const QString filter);

    int m_mediaSourceId;
    QString m_searchTerm;
    QString m_filter;
    bool m_searchInProgress;
    bool m_searchIsValid;

    MediaPlayer::MediaNode m_searchResultNode;

private slots:
    /**
     * @brief Search success handler. Browses the search result node.
     * @param command A pointer to the SearchCommand instance used to invoke the search method.
     */
    void onSearchResult(QPlayer::SearchCommand *command);

    /**
     * @brief Search error handler.
     * @param error The error message.
     */
    void onSearchError(QPlayer::SearchCommand *command);

    /**
     * @brief Appends the browse results to the model media node storage.
     * @param command A pointer to the BrowseCommand instance used to invoke the browse method.
     */
    void onBrowseResult(QPlayer::BrowseCommand *command);

    /**
     * @brief Browse error handler.
     * @param error The error message.
     */
    void onBrowseError(QPlayer::BrowseCommand *command);
};

SearchModel::Private::Private(SearchModel *qq)
    : q(qq)
    , m_mediaSourceId(-1)
    , m_searchTerm("")
    , m_filter("")
    , m_searchInProgress(false)
    , m_searchIsValid(false)
{
}

void SearchModel::Private::search(const int mediaSourceId, QString searchTerm, QString filter)
{
    // Check if we have a valid search term and that we're not attempting to search for the same results
    if(searchTerm.trimmed() != ""
            && (mediaSourceId != m_mediaSourceId || searchTerm != m_searchTerm || filter != m_filter)) {
        // Reset the search results
        resetSearch();

        // Set the search parameters
        setMediaSourceId(mediaSourceId);
        setSearchTerm(searchTerm);
        setFilter(filter);

        // Perform the search
        QPlayer::SearchCommand *command = new QPlayer::SearchCommand(m_mediaSourceId, m_searchTerm, m_filter);
        connect(command, &QPlayer::SearchCommand::complete, this, &SearchModel::Private::onSearchResult);
        connect(command, &QPlayer::SearchCommand::error, this, &SearchModel::Private::onSearchError);
        q->m_qPlayer->search(command);
    }
}

void SearchModel::Private::resetSearch()
{
    setMediaSourceId(-1);
    setSearchTerm("");
    setFilter("");
    setSearchInProgress(false);
    setSearchIsValid(false);

    MediaPlayer::MediaNode emptyNode;
    m_searchResultNode = emptyNode;

    // Clear existing search results
    q->clear();
}

int SearchModel::Private::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString SearchModel::Private::searchTerm() const
{
    return m_searchTerm;
}

QString SearchModel::Private::filter() const
{
    return m_filter;
}

bool SearchModel::Private::searchInProgress() const
{
    return m_searchInProgress;
}

bool SearchModel::Private::searchIsValid() const
{
    return m_searchIsValid;
}

void SearchModel::Private::setSearchInProgress(bool inProgress)
{
    if(m_searchInProgress != inProgress) {
        m_searchInProgress = inProgress;
        emit q->searchInProgressChanged(m_searchInProgress);
    }
}

void SearchModel::Private::setSearchIsValid(bool isValid)
{
    if(m_searchIsValid != isValid) {
        m_searchIsValid = isValid;
        emit q->searchIsValidChanged(m_searchIsValid);
    }
}

void SearchModel::Private::setMediaSourceId(const int mediaSourceId)
{
    if(m_mediaSourceId != mediaSourceId) {
        m_mediaSourceId = mediaSourceId;
        emit q->mediaSourceIdChanged(m_mediaSourceId);
    }
}

void SearchModel::Private::setSearchTerm(const QString searchTerm)
{
    if(m_searchTerm != searchTerm) {
        m_searchTerm = searchTerm;
        emit q->searchTermChanged(m_searchTerm);
    }
}

void SearchModel::Private::setFilter(const QString filter)
{
    if(m_filter != filter) {
        m_filter = filter;
        emit q->filterChanged(m_filter);
    }
}

void SearchModel::Private::doLoadPage(int page) const
{
    QPlayer::BrowseCommand *command = new QPlayer::BrowseCommand(m_searchResultNode.mediaSourceId, m_searchResultNode.id, q->pageSize(), q->pageSize() * (page - 1));
    connect(command, &QPlayer::BrowseCommand::complete, this, &SearchModel::Private::onBrowseResult);
    connect(command, &QPlayer::BrowseCommand::error, this, &SearchModel::Private::onBrowseError);
    q->m_qPlayer->browse(command);
}

void SearchModel::Private::onSearchResult(QPlayer::SearchCommand *command)
{
    // Verify that we received a search result node
    if(command->result().length() == 1) {
        // Cache the search result node in case we need it
        m_searchResultNode = MediaPlayer::TypeConverter::convertMediaNode(command->result()[0]);

        // Browse the search result node
        q->loadPage(1);
    } else {
        // The search returned no results
        setSearchInProgress(false);
        setSearchIsValid(true);
    }
}

void SearchModel::Private::onSearchError(QPlayer::SearchCommand *command)
{
    Q_UNUSED(command);

    // We treat this as the search returning no results.
    setSearchInProgress(false);
    setSearchIsValid(true);
}

void SearchModel::Private::onBrowseResult(QPlayer::BrowseCommand *command)
{
    QList<MediaNode> mediaNodes;

    for(int i = 0; i < command->result().length(); i++) {
        mediaNodes.append(TypeConverter::convertMediaNode(command->result()[i]));
    }

    q->insert(command->offset(), mediaNodes);

    setSearchInProgress(false);
    setSearchIsValid(true);
}

void SearchModel::Private::onBrowseError(QPlayer::BrowseCommand *command)
{
    Q_UNUSED(command);

    // We treat this as the search returning no results.
    setSearchInProgress(false);
    setSearchIsValid(true);
}

SearchModel::SearchModel(QPlayer::QPlayer *qPlayer, QObject *parent)
    : PagedMediaNodeModel(qPlayer, parent)
    , d(new Private(this))
{
}

void SearchModel::search(int mediaSourceId, QString searchTerm, QString filter)
{
    d->search(mediaSourceId, searchTerm, filter);
}

int SearchModel::mediaSourceId() const
{
    return d->mediaSourceId();
}

QString SearchModel::searchTerm() const
{
    return d->searchTerm();
}

QString SearchModel::filter() const
{
    return d->filter();
}

bool SearchModel::searchInProgress() const
{
    return d->searchInProgress();
}

bool SearchModel::searchIsValid() const
{
    return d->searchIsValid();
}

void SearchModel::resetSearch()
{
    d->resetSearch();
}

void SearchModel::doLoadPage(int page) const
{
    d->doLoadPage(page);
}

}
}
