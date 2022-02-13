#pragma once

#include <QObject>
#include "downloadlistmodel.h"
#include "downloader.h"
//#include "settings.h"

class MyCore : public QObject
{
	Q_OBJECT
	Q_PROPERTY(DownloadListModel * model READ model WRITE setModel NOTIFY modelChanged)
	Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
	Q_PROPERTY(QString pass READ pass WRITE setPass NOTIFY passChanged)
	Q_PROPERTY(bool savePass READ savePass WRITE setSavePass NOTIFY savePassChanged)
	Q_PROPERTY(QString sourcePath READ sourcePath WRITE setSourcePath NOTIFY sourcePathChanged)
	Q_PROPERTY(QString destinationPath READ destinationPath WRITE setDestinationPath NOTIFY destinationPathChanged)
	Q_PROPERTY(int nextTryDownload READ nextTryDownload WRITE setNextTryDownload NOTIFY nextTryDownloadChanged)

public:
	static const QString ITEM_NAME;     ///< Core
	static const bool IS_QML_REG;
	static QString FORMAT_SUBDIR;

	MyCore(QObject* parent = nullptr);

	DownloadListModel *model() const;
	void setModel(DownloadListModel *newModel);
	const QString &login() const;
	void setLogin(const QString &newLogin);
	const QString &pass() const;
	void setPass(const QString &newPass);
	bool savePass() const;
	void setSavePass(bool newSavePass);
	const QString &sourcePath() const;
	void setSourcePath(const QString &newSourcePath);
	const QString &destinationPath() const;
	void setDestinationPath(const QString &newDestinationPath);
	int nextTryDownload() const;
	void setNextTryDownload(int newNextTryDownload);

	void checkState(int row);

public slots:
	void startDownload();
	void stopDownload();

private slots:
	void startDownloadNextFile();

	///
	/// \brief Проверяет наличие файлов.
	///
	void checkAllStates();

signals:
	void modelChanged();
	void loginChanged();
	void passChanged();
	void savePassChanged();
	void sourcePathChanged();
	void destinationPathChanged();
	void nextTryDownloadChanged();

	void startedDownload();
	void finishedDownload();

private:
	enum CommandState {
		CommandNone,
		CommandDownloading,
		CommandDownloaded,
		CommandStop,

		CommandCount
	};

	///
	/// \brief Создает папку для сохранения (destinationPath + FORMAT_SUBDIR).
	///
	void createDir();
	void incrNextTryDownload();
	void parseFile();

	DownloadListModel *_model;
	Downloader *_d;
	QString _login;
	QString _pass;
	bool _savePass {};
	QString _sourcePath;
	QString _destinationPath;

	///
	/// \brief Индекс проверки следующего файла.
	///
	int _nextTryDownload {};

	CommandState _lastCommand {};

	///
	/// \brief Максимальное количество параллельных запросов (скачиваний).
	///
	int _maxParallelReplies { 5 };

	///
	/// \brief Путь папки сохранения.
	///
	QString _pathToSaveDir;

//	Settings _s;

};

