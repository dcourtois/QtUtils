#ifndef QT_UTILS_FILE_H
#define QT_UTILS_FILE_H

#include "./Setup.h"

#include <QObject>


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Simple utility to read and write file from / to the disk.
	//!
	class File
		: public QObject
	{

		Q_OBJECT

	public:

		// QML API
		Q_INVOKABLE QString		read(const QString & filename);
		Q_INVOKABLE bool		write(const QString & filename, const QString & content);

	};

QT_UTILS_NAMESPACE_END


#endif
