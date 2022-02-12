#include "downloadlistmodel.h"

#include <QDebug>

#include "myfunc.h"

const QString DownloadListModel::ITEM_NAME = "DownloadListModel";
const bool DownloadListModel::IS_QML_REG = My::qmlRegisterType<DownloadListModel>(DownloadListModel::ITEM_NAME);

const std::array
	<QString, DownloadListModel::DOWNLOAD_ROLE_COUNT>
		DownloadListModel::DOWNLOAD_ROLE_STR {
	"downloadUrl", "downloadState", "downloadFilename", "downloadBytesTotal", "downloadBytesReceived"
};

DownloadListModel::DownloadListModel(QObject *parent) : QAbstractListModel(parent)
{
	connect(this, &DownloadListModel::rowsInserted, this, &DownloadListModel::checkRowChanged, Qt::QueuedConnection);
	connect(this, &DownloadListModel::rowsRemoved, this, &DownloadListModel::checkRowChanged, Qt::QueuedConnection);
	connect(this, &DownloadListModel::modelReset, this, &DownloadListModel::checkRowChanged, Qt::QueuedConnection);
}

int DownloadListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return _list.size();
}

QVariant DownloadListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.column() != 0
			|| index.row() < 0 || index.row() >= rowCount()) {
		return QVariant();
	}
	switch (role) {
	case Qt::DisplayRole:
	case DownloadUrl:
		return _list.at(index.row()).url;
	case DownloadFilename:
		return QUrl(_list.at(index.row()).url).fileName();
	case DownloadState:
		return _list.at(index.row()).state;
	case DownloadBytesTotal:
		return _list.at(index.row()).bytesTotal;
	case DownloadBytesReceived:
		return _list.at(index.row()).bytesReceived;
	default:
		Q_ASSERT(false);
	}
	return QVariant();
}

bool DownloadListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.row() < 0 || index.row() >= _list.size() || index.column() != 0) {
		return false;
	}
	if (data(index, role) == value) {
		return false;
	}
	auto &rowData = _list[index.row()];
	switch (role) {
	case Qt::DisplayRole:
	case Qt::EditRole:
	case DownloadUrl:
		rowData.url = value.toString();
		break;
	case DownloadState:
		rowData.state = RowState(value.toInt());
		if (rowData.state == StateError) {
			setData(index, 0, DownloadBytesReceived);
		}
		break;
	case DownloadBytesTotal:
		rowData.bytesTotal = value.toInt();
		break;
	case DownloadBytesReceived:
		rowData.bytesReceived = value.toInt();
		break;
	case DownloadFilename: // нельзя изменять!
	default:
		Q_ASSERT(false);
		return false;
	}
	emit dataChanged(index, index, { role });
	return true;
}

QHash<int, QByteArray> DownloadListModel::roleNames() const
{
	QHash<int, QByteArray> r;
	for (int i = DownloadBeginRole; i < DownloadEndRole; ++i) {
		r.insert(i, DOWNLOAD_ROLE_STR.at(i - DownloadBeginRole).toUtf8());
	}
	return r;
}

QVariant DownloadListModel::data(int row, int role) const
{
	return data(index(row), role);
}

bool DownloadListModel::setData(int row, const QVariant &value, int role)
{
	return setData(index(row), value, role);
}

void DownloadListModel::addRow(QString url, RowState state)
{
	if (url.isEmpty()) { return; }
	beginInsertRows(QModelIndex(), _list.size(), _list.size());
	_list.append(RowFields(url, state));
	endInsertRows();
}

bool DownloadListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (count <= 0 || row < 0 || (row + count) > rowCount(parent)) {
		return false;
	}
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	const auto it = _list.begin() + row;
	_list.erase(it, it + count);
	endRemoveRows();
	return true;
}

void DownloadListModel::clear()
{
	beginResetModel();
	_list.clear();
	endResetModel();
}

bool DownloadListModel::setStateAndBytes(int row, RowState state)
{
	if (row < 0 || row >= _list.size()) {
		return false;
	}
	_list[row].state = state;
	_list[row].bytesReceived = 0;
	_list[row].bytesTotal = 1;
	emit dataChanged(index(row), index(row), { DownloadState, DownloadBytesReceived, DownloadBytesTotal });
	return true;
}

void DownloadListModel::allStateToNone()
{
	for (auto &r: _list) {
		r.state = StateNone;
		r.bytesReceived = 0;
		r.bytesTotal = 1;
	}
	emit dataChanged(index(0), index(_list.size() - 1), { DownloadState, DownloadBytesReceived, DownloadBytesTotal });
}

void DownloadListModel::checkRowChanged()
{
	if (_lastRowCount == rowCount()) {
		return;
	}
	_lastRowCount = rowCount();
	emit rowCountChanged();
}
