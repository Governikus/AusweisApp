/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkManager.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "NetworkReplyError.h"
#include "SecureStorage.h"
#include "TlsChecker.h"
#include "VersionInfo.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkProxyFactory>
#include <QStringBuilder>
#include <http_parser.h>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)

bool NetworkManager::mLockProxy = false;

NetworkManager::NetworkManager()
	: QObject()
	, mNetAccessManager()
	, mApplicationExitInProgress(false)
	, mOpenConnectionCount(0)
	, mUpdaterSessions()
{
	mNetAccessManager.setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);
	connect(&mNetAccessManager, &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireProxyChanged, this, &NetworkManager::onProxyChanged);
}


int NetworkManager::getOpenConnectionCount() const
{
	return mOpenConnectionCount;
}


void NetworkManager::clearConnections()
{
	mNetAccessManager.clearConnectionCache();
	mUpdaterSessions.clear();
}


QSharedPointer<QNetworkReply> NetworkManager::paos(QNetworkRequest& pRequest,
		const QByteArray& pNamespace,
		const QByteArray& pData,
		bool pUsePsk,
		const QByteArray& pSslSession)
{
	return processRequest(pRequest, [pData, pNamespace, pUsePsk, pSslSession, this] (QNetworkRequest& request){
			request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/vnd.paos+xml; charset=UTF-8"));
			request.setRawHeader("Connection", "keep-alive");
			request.setRawHeader("Accept", "text/html; application/vnd.paos+xml");
			request.setRawHeader("PAOS", QByteArray("ver=\"%1\"").replace(QByteArray("%1"), pNamespace));

			SecureStorage::TlsSuite tlsSuite = pUsePsk ? SecureStorage::TlsSuite::PSK : SecureStorage::TlsSuite::DEFAULT;
			auto cfg = Env::getSingleton<SecureStorage>()->getTlsConfig(tlsSuite).getConfiguration();
			cfg.setSessionTicket(pSslSession);
			request.setSslConfiguration(cfg);
			return post(request, pData);
		});
}


QSharedPointer<QNetworkReply> NetworkManager::get(QNetworkRequest& pRequest)
{
	return processRequest(pRequest, [this] (const QNetworkRequest& request){
			return trackConnection(mNetAccessManager.get(request));
		});
}


QSharedPointer<QNetworkReply> NetworkManager::post(QNetworkRequest& pRequest,
		const QByteArray& pData)
{
	pRequest.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(pData.size()));

	return processRequest(pRequest, [pData, this] (const QNetworkRequest& request){
			return trackConnection(mNetAccessManager.post(request, pData));
		});
}


QSharedPointer<QNetworkReply> NetworkManager::deleteResource(QNetworkRequest& pRequest)
{
	return processRequest(pRequest, [this] (const QNetworkRequest& request){
			return trackConnection(mNetAccessManager.deleteResource(request));
		});
}


QSharedPointer<QNetworkReply> NetworkManager::getAsUpdater(QNetworkRequest& pRequest)
{
	return processUpdaterRequest(pRequest, [this] (QNetworkRequest& request){
			return get(request);
		});
}


QSharedPointer<QNetworkReply> NetworkManager::postAsUpdater(QNetworkRequest& pRequest, const QByteArray& pData)
{
	return processUpdaterRequest(pRequest, [pData, this] (QNetworkRequest& request){
			return post(request, pData);
		});
}


QSharedPointer<QNetworkReply> NetworkManager::processRequest(QNetworkRequest& pRequest,
		const std::function<QSharedPointer<QNetworkReply>(QNetworkRequest&)>& pInvoke)
{
	if (!prepareConnection(pRequest))
	{
		return QSharedPointer<QNetworkReply>(new NetworkReplyError(pRequest), &QObject::deleteLater);
	}

	return pInvoke(pRequest);
}


QSharedPointer<QNetworkReply> NetworkManager::processUpdaterRequest(QNetworkRequest& pRequest,
		const std::function<QSharedPointer<QNetworkReply>(QNetworkRequest&)>& pInvoke)
{
	QSharedPointer<QNetworkReply> response = processRequest(pRequest, [pInvoke] (QNetworkRequest& request){
			auto cfg = request.sslConfiguration();
			cfg.setCaCertificates(Env::getSingleton<SecureStorage>()->getUpdateCertificates().toList());
			request.setSslConfiguration(cfg);
			return pInvoke(request);
		});

	connect(response.data(), &QNetworkReply::sslErrors, this, [response, this](const QList<QSslError>& pErrors){
			QList<QSslError> ignoredErrors;

			for (const auto& error : pErrors)
			{
				if (error.error() == QSslError::OcspNoResponseFound && mUpdaterSessions.contains(response->sslConfiguration().sessionTicket()))
				{
					ignoredErrors << error; // QTBUG-99241
					continue;
				}

				qCCritical(network) << "Fatal SSL error:" << error;
				if (!error.certificate().isNull())
				{
					qCCritical(network) << error.certificate();
				}
			}

			if (ignoredErrors.isEmpty())
			{
				response->abort();
			}
			else
			{
				response->ignoreSslErrors(ignoredErrors);
			}
		});

	connect(response.data(), &QNetworkReply::encrypted, this, [response, this]{
			const auto& cfg = response->sslConfiguration();
			TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

			const auto& trustedCertificates = Env::getSingleton<SecureStorage>()->getUpdateCertificates();
			const auto& cert = cfg.peerCertificate();
			if (cert.isNull() || !trustedCertificates.contains(cert))
			{
				const QString& textForLog = response->request().url().fileName();
				qCCritical(network).nospace() << "Untrusted certificate found [" << textForLog << "]: " << cert;
				response->abort();
				return;
			}

			const auto& ticket = cfg.sessionTicket();
			if (!ticket.isEmpty())
			{
				mUpdaterSessions << ticket;
			}
		});

	return response;
}


QString NetworkManager::getUserAgentServerHeader()
{
	// According to TR-03124-1, chapter 2.2.2.1, the Server-header shall have the following form:
	const auto& info = VersionInfo::getInstance();
	const QString spec = QStringLiteral(" (%1/%2)").arg(info.getSpecificationTitle(), info.getSpecificationVersion());
	return info.getImplementationTitle() % QLatin1Char('/') % info.getImplementationVersion() % spec;
}


void NetworkManager::onShutdown()
{
	mApplicationExitInProgress = true;
	mNetAccessManager.clearAccessCache();
	clearConnections();
	Q_EMIT fireShutdown();
}


void NetworkManager::onProxyChanged() const
{
	setApplicationProxyFactory();
}


NetworkManager::NetworkError NetworkManager::toNetworkError(const QSharedPointer<const QNetworkReply>& pNetworkReply)
{
	qCDebug(network) << "Select error message for:" << pNetworkReply->error();
	switch (pNetworkReply->error()) // See qtbase/src/network/access/qhttpthreaddelegate.cpp for details
	{
		case QNetworkReply::TimeoutError:
			return NetworkError::TimeOut;

		case QNetworkReply::ServiceUnavailableError: // 503
			return NetworkError::ServiceUnavailable;

		case QNetworkReply::InternalServerError: // 500
		case QNetworkReply::OperationNotImplementedError: // 501
		case QNetworkReply::UnknownServerError: // > 500
			return NetworkError::ServerError;

		case QNetworkReply::ProtocolInvalidOperationError: // 400
		case QNetworkReply::AuthenticationRequiredError: // 401
		case QNetworkReply::ContentAccessDenied: // 403
		case QNetworkReply::ContentNotFoundError: // 404
		case QNetworkReply::ContentOperationNotPermittedError: // 405
		case QNetworkReply::ContentConflictError: // 409
		case QNetworkReply::ContentGoneError: // 410
		case QNetworkReply::UnknownContentError: // >= 400
			return NetworkError::ClientError;

		case QNetworkReply::ProxyAuthenticationRequiredError: // 407
		case QNetworkReply::ProxyConnectionClosedError:
		case QNetworkReply::ProxyConnectionRefusedError:
		case QNetworkReply::ProxyNotFoundError:
		case QNetworkReply::ProxyTimeoutError:
		case QNetworkReply::UnknownProxyError:
			return NetworkError::ProxyError;

		case QNetworkReply::SslHandshakeFailedError:
			return NetworkError::SecurityError;

		default:
			qCCritical(network) << "Network error opening URL" << pNetworkReply->request().url().toString();
			qCCritical(network) << "Network error description" << pNetworkReply->errorString();
			return NetworkError::OtherError;
	}
}


GlobalStatus NetworkManager::toTrustedChannelStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply)
{
	const GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::LAST_URL, pNetworkReply->url().toString()}
	};
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::ServiceUnavailable:
			return {GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, infoMap};

		case NetworkManager::NetworkError::ServerError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Server_Error, infoMap};

		case NetworkManager::NetworkError::ClientError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Client_Error, infoMap};

		case NetworkManager::NetworkError::TimeOut:
			return {GlobalStatus::Code::Workflow_TrustedChannel_TimeOut, infoMap};

		case NetworkManager::NetworkError::ProxyError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error, infoMap};

		case NetworkManager::NetworkError::SecurityError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, infoMap};

		case NetworkManager::NetworkError::OtherError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error, infoMap};
	}

	return {GlobalStatus::Code::Unknown_Error, infoMap};
}


GlobalStatus NetworkManager::toStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply)
{
	const GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::LAST_URL, pNetworkReply->url().toString()}
	};
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::ServiceUnavailable:
			return {GlobalStatus::Code::Network_ServiceUnavailable, infoMap};

		case NetworkManager::NetworkError::ServerError:
			return {GlobalStatus::Code::Network_ServerError, infoMap};

		case NetworkManager::NetworkError::ClientError:
			return {GlobalStatus::Code::Network_ClientError, infoMap};

		case NetworkManager::NetworkError::TimeOut:
			return {GlobalStatus::Code::Network_TimeOut, infoMap};

		case NetworkManager::NetworkError::ProxyError:
			return {GlobalStatus::Code::Network_Proxy_Error, infoMap};

		case NetworkManager::NetworkError::SecurityError:
			return {GlobalStatus::Code::Network_Ssl_Establishment_Error, infoMap};

		case NetworkManager::NetworkError::OtherError:
			return {GlobalStatus::Code::Network_Other_Error, infoMap};
	}

	return {GlobalStatus::Code::Unknown_Error, infoMap};
}


bool NetworkManager::prepareConnection(QNetworkRequest& pRequest) const
{
	if (mApplicationExitInProgress)
	{
		return false;
	}

	pRequest.setTransferTimeout();
	if (pRequest.header(QNetworkRequest::UserAgentHeader).isNull())
	{
		pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentServerHeader());
	}

	if (pRequest.sslConfiguration() == QSslConfiguration::defaultConfiguration())
	{
		auto cfg = Env::getSingleton<SecureStorage>()->getTlsConfig().getConfiguration();
		pRequest.setSslConfiguration(cfg);
	}

	return true;
}


QSharedPointer<QNetworkReply> NetworkManager::trackConnection(QNetworkReply* pResponse)
{
	Q_ASSERT(pResponse);

	if (pResponse && !pResponse->isFinished())
	{
		++mOpenConnectionCount;

		connect(pResponse, &QNetworkReply::finished, this, [this] {
				--mOpenConnectionCount;
			});
		connect(this, &NetworkManager::fireShutdown, pResponse, &QNetworkReply::abort, Qt::QueuedConnection);
	}

	return QSharedPointer<QNetworkReply>(pResponse, &QObject::deleteLater);
}


QByteArray NetworkManager::getStatusMessage(int pStatus)
{
	switch (pStatus)
	{
#define XX(num, name, string) case num:\
			return QByteArrayLiteral(#string);

	HTTP_STATUS_MAP(XX)
#undef XX
	}

	return QByteArray();
}


QString NetworkManager::getFormattedStatusMessage(int pStatus)
{
	return QStringLiteral("%1 %2").arg(QString::number(pStatus), QString::fromLatin1(NetworkManager::getStatusMessage(pStatus)));
}


bool NetworkManager::isLoggingAllowed(const QSharedPointer<const QNetworkReply>& pReply)
{
	const auto attribute = QByteArrayLiteral("Pragma");
	if (pReply->hasRawHeader(attribute))
	{
		const auto& pragma = pReply->rawHeader(attribute);
		return !pragma.contains(QByteArrayLiteral("no-log"));
	}

	return true;
}


int NetworkManager::getLoggedStatusCode(const QSharedPointer<const QNetworkReply>& pReply, const MessageLogger& pLogger)
{
	if (pReply->error() != QNetworkReply::NoError)
	{
		pLogger.critical() << "An error occurred:" << pReply->error() << pReply->errorString();
	}

	const int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	pLogger.debug() << "Status Code:" << statusCode << getStatusMessage(statusCode);
	for (const auto& header : pReply->rawHeaderPairs())
	{
		pLogger.debug().nospace().noquote() << "Header | " << header.first << ": " << header.second;
	}
	return statusCode;
}


namespace
{
class NoProxyFactory
	: public QNetworkProxyFactory
{
	private:
		QList<QNetworkProxy> mProxies;

	public:
		NoProxyFactory()
			: QNetworkProxyFactory()
			, mProxies()
		{
			mProxies << QNetworkProxy::NoProxy;
		}


		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			qCDebug(network) << "Found proxies" << mProxies;
			return mProxies;
		}


};


class SystemProxyFactory
	: public QNetworkProxyFactory
{
	public:
		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			QList<QNetworkProxy> proxies = systemProxyForQuery(pInputQuery);
			qCDebug(network) << "Found proxies" << proxies;
			return proxies;
		}


};

class CustomProxyFactory
	: public QNetworkProxyFactory
{
	public:
		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto proxy = QNetworkProxy(settings.getCustomProxyType(), settings.getCustomProxyHost(), settings.getCustomProxyPort());
			auto proxies = QList<QNetworkProxy>({proxy});
			qCDebug(network) << "Found proxies" << proxies;
			return proxies;
		}


};

} // namespace

void NetworkManager::setApplicationProxyFactory()
{
	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (Q_UNLIKELY(mLockProxy))
	{
		qCWarning(network) << "Won't use any proxy because user locked it!";
		qCDebug(network) << "proxy -> none";
		QNetworkProxyFactory::setApplicationProxyFactory(new NoProxyFactory());
	}
	else if (settings.useCustomProxy())
	{
		qCDebug(network) << "proxy -> custom";
		QNetworkProxyFactory::setApplicationProxyFactory(new CustomProxyFactory());
	}
	else
	{
		qCDebug(network) << "proxy -> system";
		QNetworkProxyFactory::setApplicationProxyFactory(new SystemProxyFactory());
	}
}
