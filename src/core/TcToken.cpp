/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */


#include "TcToken.h"

#include "AppSettings.h"

#include <QLoggingCategory>
#include <QRegularExpression>
#include <QXmlStreamReader>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


TcToken::TcToken(const QByteArray& pData)
	: mSchemaConform(false)
	, mBinding()
	, mPathSecurityProtocol()
	, mPsk()
	, mSessionIdentifier()
	, mServerAddress()
	, mCommunicationErrorAddress()
	, mRefreshAddress()
{
	parse(pData);
}


TcToken::~TcToken()
{
}


void TcToken::parse(const QByteArray& pData)
{
	qDebug().noquote() << "Parsing TcToken:\n" << pData;
	QXmlStreamReader reader(pData);

	QString binding, pathSecurityProtocol, serverAddress, refreshAddress, communicationErrorAddress;
	QByteArray psk, sessionIdentifier;

	for (reader.readNext(); !reader.isEndDocument() && !reader.hasError(); reader.readNext())
	{
		if (reader.isStartElement())
		{
			QString currentName = reader.name().toString();
			if (currentName == QLatin1String("Binding"))
			{
				binding = readElementValue(reader);
			}
			else if (currentName == QLatin1String("PathSecurity-Protocol"))
			{
				pathSecurityProtocol = readElementValue(reader);
			}
			else if (currentName == QLatin1String("PSK"))
			{
				psk = readElementValue(reader).toUtf8();
			}
			else if (currentName == QLatin1String("ServerAddress"))
			{
				serverAddress = readElementValue(reader);
			}
			else if (currentName == QLatin1String("SessionIdentifier"))
			{
				sessionIdentifier = readElementValue(reader).toUtf8();
			}
			else if (currentName == QLatin1String("CommunicationErrorAddress"))
			{
				communicationErrorAddress = readElementValue(reader);
			}
			else if (currentName == QLatin1String("RefreshAddress"))
			{
				refreshAddress = readElementValue(reader);
			}
		}
	}

	if (reader.hasError())
	{
		qDebug() << "XML error:" << reader.errorString();
	}
	else if (valuesAreSchemaConform(binding, pathSecurityProtocol, psk, sessionIdentifier, serverAddress, communicationErrorAddress, refreshAddress))
	{
		mSchemaConform = true;
		mBinding = binding;
		mPathSecurityProtocol = pathSecurityProtocol;
		mPsk = psk;
		mServerAddress.setUrl(serverAddress);
		mSessionIdentifier = sessionIdentifier;
		mCommunicationErrorAddress.setUrl(communicationErrorAddress);
		mRefreshAddress.setUrl(refreshAddress);
	}

	reader.clear();
}


QString TcToken::readElementValue(QXmlStreamReader& pReader)
{
	// helper method to distinguish between the cases,
	// 1) where the element was present but empty (so the value is not null but empty) and
	// 2) where the element was absent (so the value is null).
	QString value = pReader.readElementText();
	return value.isNull() ? QLatin1String("") : value.trimmed();
}


bool TcToken::valuesAreSchemaConform(const QString& pBinding,
		const QString& pPathSecurityProtocol,
		const QByteArray& pPsk,
		const QByteArray& pSessionIdentifier,
		const QString& pServerAddress,
		const QString& pCommunicationErrorAddress,
		const QString& pRefreshAddress) const
{
	if (pBinding.isNull() || !isAnyUri(pBinding))
	{
		qCritical() << "Binding is no valid anyUri:" << pBinding;
		return false;
	}

	if (!pPathSecurityProtocol.isNull() && !QUrl(pPathSecurityProtocol).isValid())
	{
		qCritical() << "PathSecurity-Protocol is no valid URI:" << pPathSecurityProtocol;
	}

	if (pPsk.isNull())
	{
		// PSK is not needed for attached eID-Server model
		qWarning() << "PSK is null";
	}

	if (pSessionIdentifier.isNull())
	{
		qWarning() << "SessionIdentifier is null";
	}

	if (pServerAddress.isNull() || !isAnyUri(pServerAddress))
	{
		qCritical() << "ServerAddress no valid anyUri:" << pServerAddress;
		return false;
	}

	if (pRefreshAddress.isNull() || !isAnyUri(pRefreshAddress))
	{
		qCritical() << "RefreshAddress no valid anyUri:" << pRefreshAddress;
		return false;
	}

	if (!isAnyUri(pCommunicationErrorAddress))
	{
		qCritical() << "CommunicationErrorAddress no  valid anyUri:" << pCommunicationErrorAddress;
		return false;
	}

	return true;
}


bool TcToken::isAnyUri(const QString& pCandidate) const
{
	return pCandidate.isEmpty() || QUrl(pCandidate).isValid();
}


bool TcToken::isValid() const
{
	if (!mSchemaConform)
	{
		return false;
	}

	if (mBinding != QLatin1String("urn:liberty:paos:2006-08"))
	{
		qCritical() << "Wrong binding:" << mBinding;
		return false;
	}

	if (!mPathSecurityProtocol.isNull() && mPathSecurityProtocol != QLatin1String("urn:ietf:rfc:4279"))
	{
		qCritical() << "Wrong PathSecurity-Protocol:" << mPathSecurityProtocol;
		return false;
	}

	if (mServerAddress.scheme() != QLatin1String("https"))
	{
		qCritical() << "ServerAddress no valid https url:" << mServerAddress;
		return false;
	}

	if (mRefreshAddress.scheme() != QLatin1String("https"))
	{
		const QString errorInvalidUrl = QStringLiteral("RefreshAddress no valid https url: %1").arg(mRefreshAddress.toString());
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << errorInvalidUrl;
		}
		else
		{
			qCritical() << errorInvalidUrl;
			return false;
		}
	}

	if (!mPsk.isNull() && !isHexBinary(QString::fromLatin1(mPsk)))
	{
		qCritical() << "PSK no valid hexBinary:" << mPsk;
		return false;
	}

	return true;
}


bool TcToken::isHexBinary(const QString& pCandidate) const
{
	static const QRegularExpression hexMatcher(QStringLiteral("^[0-9A-F]*$"), QRegularExpression::CaseInsensitiveOption);
	return pCandidate.isEmpty() || (hexMatcher.match(pCandidate).hasMatch() && pCandidate.size() % 2 == 0);
}


const QByteArray& TcToken::getSessionIdentifier() const
{
	return mSessionIdentifier;
}


const QUrl& TcToken::getServerAddress() const
{
	return mServerAddress;
}


const QUrl& TcToken::getRefreshAddress() const
{
	return mRefreshAddress;
}


const QUrl& TcToken::getCommunicationErrorAddress() const
{
	return mCommunicationErrorAddress;
}


bool TcToken::usePsk() const
{
	return !mPsk.isNull();
}


const QByteArray& TcToken::getPsk() const
{
	return mPsk;
}


const QString& TcToken::getBinding() const
{
	return mBinding;
}
