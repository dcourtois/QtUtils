#ifndef QT_UTILS_HTTP_REQUEST_H
#define QT_UTILS_HTTP_REQUEST_H

#include "./Setup.h"

#include <QString>
#include <QNetworkReply>


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Defines the signature of a function like object that will be called when
	//! a request has been successfully retrieved.
	//!
	typedef std::function< void (QByteArray reply) > SuccessCallback;

	//!
	//! Defines the signature of a function like object that will be called when
	//! a request couldn't be completed.
	//!
	typedef std::function< void (QNetworkReply::NetworkError error, QString errorString) > FailureCallback;

	// helpers
	QByteArray	RequestUrl(const QString & url, QNetworkAccessManager * networkManager = nullptr);
	void		RequestUrl(const QString & url, const SuccessCallback & success, const FailureCallback & failure, QNetworkAccessManager * networkManager = nullptr);

QT_UTILS_NAMESPACE_END


#endif
