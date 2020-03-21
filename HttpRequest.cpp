#include "./HttpRequest.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QUrl>


QT_UTILS_NAMESPACE_BEGIN

	//! The network manager
	QNetworkAccessManager s_NetworkManager;

	//!
	//! Synchronously request a url.
	//!
	//! @param url
	//!		The url to request.
	//!
	//! @param retry
	//!		Do not use. This is used internally to handle URL rediction retries.
	//!
	//! @returns
	//!		The reply or an empty array.
	//!
	QByteArray RequestUrl(const QString & url, int retry)
	{
		if (retry == -1)
		{
			return QByteArray();
		}

		// prepare the request
		QNetworkRequest request;
		request.setUrl(QUrl(url));

		// send
		QNetworkReply * reply = s_NetworkManager.get(request);

		// wait for the reply
		QEventLoop loop;
		while (reply->isFinished() == false)
		{
			loop.processEvents();
		}

		if (reply->error() != QNetworkReply::NoError)
		{
			reply->deleteLater();
			return QByteArray();
		}
		else
		{
			QVariant statusCode = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute);
			if (statusCode.isValid())
			{
				// redirect
				if (statusCode.toInt() == 301)
				{
					QString redirection = reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString();
					reply->deleteLater();
					return RequestUrl(redirection, retry - 1);
				}
			}

			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
	}

	//!
	//! Asynchronously request a url.
	//!
	//! @param networkManager
	//!		The network manager to use for this request
	//!
	//! @param url
	//!		The url to request.
	//!
	//! @param success
	//!		A function that will be called when the request successfully completed.
	//!
	//! @param failure
	//!		A function that will be called if the request failed.
	//!
	void RequestUrl(const QString & url, const SuccessCallback & success, const FailureCallback & failure)
	{
		// prepare the request
		QNetworkRequest request;
		request.setUrl(QUrl(url));

		// send
		QNetworkReply * reply = s_NetworkManager.get(request);
		QObject::connect(reply, &QNetworkReply::finished, [=] (void) {
			QNetworkReply::NetworkError error = reply->error();
			QVariant statusCode = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute);
			if (statusCode.isValid())
			{
				// redirect
				if (statusCode.toInt() == 301)
				{
					QString redirection = reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString();
					reply->deleteLater();
					RequestUrl(redirection, success, failure);
					return;
				}
			}
			if (error != QNetworkReply::NoError)
			{
				QString errorString = reply->errorString();
				reply->deleteLater();
				failure(error, errorString);
			}
			else
			{
				QByteArray result = reply->readAll();
				reply->deleteLater();
				success(result);
			}
		});
	}

QT_UTILS_NAMESPACE_END
