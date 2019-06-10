#include "./File.h"
#include <QFile>


//!
//! Read an entier file and return its content as a string.
//!
QString File::read(const QString & filename)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		return QString();
	}

	return file.readAll();
}

//!
//! Writes a string into a file. This will overwrites any previous content, if
//! @p filename already exists. It returns true if the operation succeeded.
//!
bool File::write(const QString & filename, const QString & content)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		return false;
	}

	file.write(content.toLocal8Bit());
	return true;
}
