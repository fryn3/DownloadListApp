#include "mycore.h"
#include "myfunc.h"

#include <QDir>

const QString MyCore::ITEM_NAME = "Core";
const bool MyCore::IS_QML_REG = My::qmlRegisterType<MyCore>(MyCore::ITEM_NAME);
QString MyCore::FORMAT_SUBDIR = "yyyy_MM_dd";

MyCore::MyCore(QObject *parent)
	: QObject(parent), _model(new DownloadListModel(this)), _d(new Downloader(this))
{
	connect(_d, &Downloader::parallelReplysChanged, this, [this] {
		qDebug() << "Downloader::parallelReplysChanged" << _d->parallelReplies();
	});
	connect(this, &MyCore::sourcePathChanged, this, &MyCore::checkAllStates, Qt::QueuedConnection);
	connect(this, &MyCore::destinationPathChanged, this, &MyCore::checkAllStates, Qt::QueuedConnection);

	connect(_d, &Downloader::finished, this, &MyCore::startDownloadNextFile, Qt::QueuedConnection);

	connect(_d, &Downloader::started, this, [this] (int num) {
		_model->setData(_model->index(num),
						DownloadListModel::StateProcess,
						DownloadListModel::DownloadState);
	});
	connect(_d, &Downloader::finished, this, [this] (int num, QNetworkReply::NetworkError error) {
		_model->setData(_model->index(num),
						error == QNetworkReply::NoError
							? DownloadListModel::StateDone
							: DownloadListModel::StateError,
						DownloadListModel::DownloadState);
	});
	connect(_d, &Downloader::updateDownloadProgress, this, [this] (int num, qint64 bytesReceived, qint64 bytesTotal) {
		_model->setData(_model->index(num),
						bytesTotal,
						DownloadListModel::DownloadBytesTotal);
		_model->setData(_model->index(num),
						bytesReceived,
						DownloadListModel::DownloadBytesReceived);
	});
}

DownloadListModel *MyCore::model() const
{
	return _model;
}

void MyCore::setModel(DownloadListModel *newModel)
{
	if (_model == newModel)
		return;
	_model = newModel;
	emit modelChanged();
}

const QString &MyCore::login() const
{
	return _login;
}

void MyCore::setLogin(const QString &newLogin)
{
	if (_login == newLogin)
		return;
	_login = newLogin;
	emit loginChanged();
}

const QString &MyCore::pass() const
{
	return _pass;
}

void MyCore::setPass(const QString &newPass)
{
	if (_pass == newPass)
		return;
	_pass = newPass;
	emit passChanged();
}

bool MyCore::savePass() const
{
	return _savePass;
}

void MyCore::setSavePass(bool newSavePass)
{
	if (_savePass == newSavePass)
		return;
	_savePass = newSavePass;
	emit savePassChanged();
}

const QString &MyCore::sourcePath() const
{
	return _sourcePath;
}

void MyCore::setSourcePath(const QString &newSourcePath)
{
	if (_sourcePath == newSourcePath)
		return;
	_sourcePath = newSourcePath;
	parseFile();
	emit sourcePathChanged();
}

const QString &MyCore::destinationPath() const
{
	return _destinationPath;
}

void MyCore::setDestinationPath(const QString &newDestinationPath)
{
	if (_destinationPath == newDestinationPath)
		return;
	_destinationPath = newDestinationPath;
	emit destinationPathChanged();
}

int MyCore::nextTryDownload() const
{
	return _nextTryDownload;
}

void MyCore::setNextTryDownload(int newNextTryDownload)
{
	if (_nextTryDownload == newNextTryDownload)
		return;
	_nextTryDownload = newNextTryDownload;
	emit nextTryDownloadChanged();
}

void MyCore::checkState(int row)
{
	if (_model->rowCount() <= row) {
		return;
	}
	if (_destinationPath.isEmpty()) {
		_model->setStateAndBytes(row, DownloadListModel::StateNone);
		return;
	}
	auto dir = QDir(_destinationPath);
	if (!dir.exists()) {
		_model->setStateAndBytes(row, DownloadListModel::StateNone);
		return;
	}
	auto dateStr = QDate::currentDate().toString(FORMAT_SUBDIR);
	if (!dir.cd(dateStr)) {
		Q_ASSERT(false && "wtf can't open dir");
		_model->setStateAndBytes(row, DownloadListModel::StateNone);
		return;
	}
	auto fileName = _model->data(row, DownloadListModel::DownloadFilename).toString();

	if (fileName.isEmpty()) {
		_model->setStateAndBytes(row, DownloadListModel::StateError);
		return;
	}
	auto fExist = QFile::exists(dir.filePath(fileName));
	_model->setStateAndBytes(row, fExist
							 ? DownloadListModel::StateDone
							 : DownloadListModel::StateNone);
}

void MyCore::startDownload()
{
	Q_ASSERT(_d->parallelReplies() == 0);
	createDir();
	Q_ASSERT(!_pathToSaveDir.isEmpty());
	_lastCommand = CommandDownloading;
	emit startedDownload();
	for (setNextTryDownload(0);
			nextTryDownload() < _model->rowCount() && nextTryDownload() < _maxParallelReplies;
			incrNextTryDownload()) {
		_d->get(_pathToSaveDir,
				_model->data(_model->index(nextTryDownload()),
							 DownloadListModel::DownloadUrl).toString(),
				nextTryDownload());
	}
}

void MyCore::stopDownload()
{
	_lastCommand = CommandStop;
	_d->cancelDownload();
	setNextTryDownload(0);
}

void MyCore::startDownloadNextFile()
{
	Q_ASSERT(_maxParallelReplies >= _d->parallelReplies());
	if (_lastCommand == CommandStop) {
		return;
	}
	if (nextTryDownload() < _model->rowCount()) {
		_d->get(_pathToSaveDir,
				_model->data(_model->index(nextTryDownload()),
							 DownloadListModel::DownloadUrl).toString(),
				nextTryDownload());
		incrNextTryDownload();
		return;
	}
	if (_d->parallelReplies() == 0 && _lastCommand == CommandDownloading) {
		_lastCommand = CommandDownloaded;
		incrNextTryDownload();	// это нужно для прогрессбара
		emit finishedDownload();
		return;
	}
}

void MyCore::checkAllStates()
{
	auto dir = QDir(_destinationPath);
	if (!dir.exists()) {
		_model->allStateToNone();
		return;
	}
	auto dateStr = QDate::currentDate().toString(FORMAT_SUBDIR);
	if (!dir.cd(dateStr)) {
		_model->allStateToNone();
		return;
	}
	for (auto r = 0; r < _model->rowCount(); ++r) {
		checkState(r);
	}
}

void MyCore::createDir()
{
	QDir dir(destinationPath());
	if (!dir.exists()) {
		_pathToSaveDir.clear();
		return;
	}
	auto dateStr = QDate::currentDate().toString(FORMAT_SUBDIR);
	if (!dir.cd(dateStr)) {
		if (!dir.mkdir(dateStr)) {
			qInfo() << __PRETTY_FUNCTION__ << "can't creat dir";
			Q_ASSERT(false);
		}
		if (!dir.cd(dateStr)) {
			qInfo() << __PRETTY_FUNCTION__ << "can't creat dir";
			Q_ASSERT(false);
		}
	}
	_pathToSaveDir = dir.path();
}

void MyCore::incrNextTryDownload()
{
	setNextTryDownload(nextTryDownload() + 1);
}

void MyCore::parseFile()
{
	_model->clear();
	QFile f(sourcePath());
	if (!f.exists() || !f.open(QIODevice::ReadOnly| QIODevice::Text)) {
		qDebug() << __PRETTY_FUNCTION__ << "can't open file";
		return;
	}
	QTextStream in(&f);
	while (!in.atEnd()) {
		_model->addRow(in.readLine());
	}
}








