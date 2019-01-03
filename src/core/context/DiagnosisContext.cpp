/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisContext.h"

using namespace governikus;

DiagnosisContext::DiagnosisContext()
	: mPcscVersion()
	, mPcscComponents()
	, mPcscDrivers()
	, mReaderInfos()
	, mTimestamp()
{
	connect(this, &DiagnosisContext::pcscInfoChanged, this, &DiagnosisContext::modelChanged);
	connect(this, &DiagnosisContext::readerInfosChanged, this, &DiagnosisContext::modelChanged);
	connect(this, &DiagnosisContext::timestampChanged, this, &DiagnosisContext::modelChanged);
}


void DiagnosisContext::setPcscInfo(const QString& pVersion, const QVector<ComponentInfo>& pComponents,
		const QVector<ComponentInfo>& pDrivers)
{
	mPcscVersion = pVersion;
	mPcscComponents = pComponents;
	mPcscDrivers = pDrivers;

	Q_EMIT pcscInfoChanged();
}


void DiagnosisContext::setReaderInfos(const QVector<ReaderInfo>& mInfos)
{
	mReaderInfos = mInfos;

	Q_EMIT readerInfosChanged();
}


void DiagnosisContext::setTimestamp(const QDateTime& pTimestamp)
{
	mTimestamp = pTimestamp;

	Q_EMIT timestampChanged();
}


void DiagnosisContext::setNetworkInterfaces(const QList<QNetworkInterface>& pNetworkInterface)
{
	mNetworkInterfaces = pNetworkInterface;

	Q_EMIT fireNetworkInfoChanged();
}


const QList<QNetworkInterface>& DiagnosisContext::getNetworkInterfaces() const
{
	return mNetworkInterfaces;
}
