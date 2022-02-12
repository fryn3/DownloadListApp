#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QNetworkReply>

class Downloader : public QObject
{
	Q_OBJECT
public:
	explicit Downloader(QObject* parent = nullptr);

	///
	/// \brief Метод для запуска скачиввания
	/// \param targetFolder - папка загрузки
	/// \param url - ссылка скачивания файла
	/// \param number - номер загрузки (нужен для параллельной загрузки)
	/// \return код ошибки(NetworkError или -1, -2, -3) или 0 если все хорошо.
	///
	void get(const QString& targetFolder, const QUrl& url, const int &number = 0);


	///
	/// \brief Количество активных скачиваний.
	/// \return Число активных скачиваний.
	///
	int parallelReplies() const;

public slots:
	// Метод для отмены всех загрузок
	void cancelDownload();

signals:
	void started(int number);
	void finished(int number, QNetworkReply::NetworkError error);
	// Сигнал передающий информацию о прогрессе загрузки
	void updateDownloadProgress(int number, qint64 bytesReceived, qint64 bytesTotal);
	void parallelReplysChanged();

private slots:
	// Слот для постепенного считывания загружаемых данных
	void onReadyRead();
	// Слот для обработки завершения запроса
	void onReply(QNetworkReply* reply);

private:
	struct ValueStruct {
		ValueStruct(int n = 0, std::shared_ptr<QFile> f = nullptr) : number(n), file(f) {}
		int number;
		std::shared_ptr<QFile> file;
	};
	void incrParallelReplys();
	void decrParallelReplys();

	QNetworkAccessManager _manager;				// Сетевой менеджер для загрузки файлов
	QMap<QNetworkReply*, ValueStruct> _replysMap;
	int _parallelReplys = 0;
};
