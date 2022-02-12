#include "downloader.h"

#include <QDir>
#include <QNetworkReply>
#include <QDebug>
#include <QAuthenticator>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
	connect(&_manager, &QNetworkAccessManager::finished, this, &Downloader::onReply);
	connect(&_manager, &QNetworkAccessManager::authenticationRequired, this, [] (QNetworkReply *reply, QAuthenticator *authenticator) {
		qDebug() << __PRETTY_FUNCTION__;
	});
}

void Downloader::get(const QString &targetFolder, const QUrl &url, const int &number)
{
	emit started(number);
	if (url.isEmpty()) {
		emit finished(number, QNetworkReply::UnknownContentError);
		return;
	}
	auto f = std::make_shared<QFile>(targetFolder + QDir::separator() + url.fileName(), this);
	if (f->exists()) {
		emit finished(number, QNetworkReply::NoError);
		return;
	}
	if (!f->open(QIODevice::WriteOnly)) {
		Q_ASSERT(false && "Can't open file");
		emit finished(number, QNetworkReply::UnknownContentError);
		return;
	}
	incrParallelReplys();
	// Создаём запрос
	QNetworkRequest request(url);
	// Обязательно разрешаем переходить по редиректам
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	// Запускаем скачивание
	auto currentReply = _manager.get(request);
	_replysMap.insert(currentReply, ValueStruct(number, f));

	// После чего сразу подключаемся к сигналам о готовности данных к чтению и обновлению прогресса скачивания
	connect(currentReply, &QNetworkReply::readyRead, this, &Downloader::onReadyRead);
	connect(currentReply, &QNetworkReply::downloadProgress, this,
			[number, this] (qint64 bytesReceived, qint64 bytesTotal) {
		emit updateDownloadProgress(number, bytesReceived, bytesTotal);
	});
}

void Downloader::cancelDownload()
{
	for (auto &r: _replysMap.keys()) {
		r->abort();
	}
}

void Downloader::onReadyRead()
{
	auto r = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT(_replysMap.contains(r));
	Q_ASSERT(_replysMap[r].file->isOpen());
	_replysMap[r].file->write(r->readAll());
}

void Downloader::onReply(QNetworkReply *reply)
{
	Q_ASSERT(_replysMap.contains(reply));

	// По завершению запроса
	if (reply->error() == QNetworkReply::NoError) {
		// сохраням файл
		_replysMap[reply].file->flush();
		_replysMap[reply].file->close();
	} else {
		// Или удаляем его в случае ошибки
		_replysMap[reply].file->remove();
	}
	auto num = _replysMap[reply].number;
	auto err = reply->error();
	_replysMap.remove(reply);
	reply->deleteLater();
	decrParallelReplys();
	emit finished(num, err);
}

int Downloader::parallelReplies() const
{
	return _parallelReplys;
}

void Downloader::incrParallelReplys()
{
	++_parallelReplys;
	emit parallelReplysChanged();
}

void Downloader::decrParallelReplys()
{
	--_parallelReplys;
	emit parallelReplysChanged();
}
