#pragma once

#include <QAbstractListModel>

class DownloadListModel : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
	static const QString ITEM_NAME;     ///< DownloadListModel
	static const bool IS_QML_REG;

	enum DownloadRole {
		DownloadBeginRole = Qt::UserRole + 1,

		DownloadUrl = DownloadBeginRole,
		DownloadState,
		DownloadFilename,
		DownloadBytesTotal,
		DownloadBytesReceived,

		DownloadEndRole
	};
	Q_ENUM(DownloadRole)
	static const int DOWNLOAD_ROLE_COUNT = DownloadEndRole - DownloadBeginRole;
	static const std::array<QString, DOWNLOAD_ROLE_COUNT> DOWNLOAD_ROLE_STR;

	enum RowState {
		StateNone,
		StateProcess,
		StateDone,
		StateError,

		StateCount
	};
	Q_ENUM(RowState)

	DownloadListModel(QObject * parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	QHash<int, QByteArray> roleNames() const override;

	QVariant data(int row, int role = Qt::DisplayRole) const;
	bool setData(int row, const QVariant &value, int role = Qt::EditRole);
	void addRow(QString url, RowState state = StateNone);
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	void clear();

	bool setStateAndBytes(int row, RowState state = StateNone);
	void allStateToNone();
private slots:
	void checkRowChanged();

signals:
	void rowCountChanged();

private:
	struct RowFields {
		RowFields(QString u, RowState s, int bTotal = 1, int bReceived = 0)
			: url(u), state(s), bytesTotal(bTotal), bytesReceived(bReceived) {}
		QString url;
		RowState state;
		int bytesTotal;
		int bytesReceived;
	};

	QVector<RowFields> _list;
	int _lastRowCount {};
};

