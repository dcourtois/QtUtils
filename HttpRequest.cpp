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
	//! @param networkManager
	//!		Optional network manager to use. If not specified or nullptr, a global one defined in the
	//!		QtUtils library is used.
	//!
	//! @returns
	//!		The reply or an empty array.
	//!
	QByteArray RequestUrl(const QString & url, QNetworkAccessManager * networkManager)
	{
		// check the manager
		if (networkManager == nullptr)
		{
			networkManager = &s_NetworkManager;
		}

		// to handle http redirections
		QString address = url;
		for (int i = 0; i < 2; ++i)
		{
			// prepare the request
			QNetworkRequest request;
			request.setUrl(QUrl(address));

			// send
			QNetworkReply * reply = networkManager->get(request);

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
						address = reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString();
						reply->deleteLater();
						continue;
					}
				}

				QByteArray result = reply->readAll();
				reply->deleteLater();
				return result;
			}
		}

		return QByteArray();
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
	//! @param networkManager
	//!		Optional network manager to use. If not specified or nullptr, a global one defined in the
	//!		QtUtils library is used.
	//!
	void RequestUrl(const QString & url, const SuccessCallback & success, const FailureCallback & failure, QNetworkAccessManager * networkManager)
	{
		// check the manager
		if (networkManager == nullptr)
		{
			networkManager = &s_NetworkManager;
		}

		// prepare the request
		QNetworkRequest request;
		request.setUrl(QUrl(url));

		// send
		QNetworkReply * reply = networkManager->get(request);
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
