/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyModel.h"

#include "AppSettings.h"
#include "DeviceInfo.h"
#include "Env.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "SecureStorage.h"
#include "VolatileSettings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QNetworkReply>
#include <QNetworkRequest>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;

#define VALUE_NAME(_name, _key)\
	inline QLatin1String _name(){\
		return QLatin1String(_key);\
	}


namespace
{
VALUE_NAME(ROM, "Rom")
VALUE_NAME(BUILD_NUMBER, "BuildNumber")
VALUE_NAME(OS_VERSION, "OsVersion")
VALUE_NAME(KERNEL_VERSION, "KernelVersion")
VALUE_NAME(MAXIMUM_NFC_PACKET_LENGTH, "MaximumNfcPacketLength")
VALUE_NAME(VENDOR, "Vendor")
VALUE_NAME(MODEL_NUMBER, "ModelNumber")
VALUE_NAME(MODEL_NAME, "ModelName")
VALUE_NAME(AUSWEISAPP_VERSION_NUMBER, "AusweisAppVersionNumber")
VALUE_NAME(NFC_TAG_TYPE, "NfcTagType")
} // namespace


SurveyModel::SurveyModel()
	: QAbstractListModel()
	, mBuildNumber(DeviceInfo::getOSBuildNumber())
	, mOsVersion(DeviceInfo::getOSVersion())
	, mKernelVersion(DeviceInfo::getKernelVersion())
	, mMaximumNfcPacketLength(0)
	, mVendor(DeviceInfo::getVendor())
	, mModelNumber(DeviceInfo::getModelNumber())
	, mModelName(DeviceInfo::getModelName())
	, mAusweisAppVersionNumber(QCoreApplication::applicationVersion())
	, mNfcTagType(QStringLiteral("U"))
	, mNfcDataAvailable(false)
	, mAuthWasSuccessful(false)
{
	buildDataObject();
}


int SurveyModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mData.size());
}


QVariant SurveyModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto entry = mData[pIndex.row()];
		if (pRole == TITLE)
		{
			return entry.first;
		}
		if (pRole == VALUE)
		{
			return entry.second;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> SurveyModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(TITLE, "title");
	roles.insert(VALUE, "value");
	return roles;
}


void SurveyModel::buildDataObject()
{
	beginResetModel();
	mData.clear();
	mData += qMakePair(tr("Vendor"), mVendor);
	mData += qMakePair(tr("Model Name"), mModelName);
	mData += qMakePair(tr("Model Number"), mModelNumber);
	mData += qMakePair(tr("Build Number"), mBuildNumber);
	mData += qMakePair(tr("OS version"), mOsVersion);
	mData += qMakePair(tr("Kernel version"), mKernelVersion);
	mData += qMakePair(tr("Max. NFC Packet Length"), QString::number(mMaximumNfcPacketLength));
	mData += qMakePair(tr("%1 Version").arg(QCoreApplication::applicationName()), mAusweisAppVersionNumber);
	mData += qMakePair(tr("NFC Tag Type"), mNfcTagType);
	endResetModel();
}


QByteArray SurveyModel::toJsonByteArray() const
{
	QJsonObject rom;
	rom[BUILD_NUMBER()] = mBuildNumber;
	rom[OS_VERSION()] = mOsVersion;
	rom[KERNEL_VERSION()] = mKernelVersion;
	rom[MAXIMUM_NFC_PACKET_LENGTH()] = QJsonValue(mMaximumNfcPacketLength);

	QJsonObject mainObject;
	mainObject[ROM()] = rom;
	mainObject[VENDOR()] = mVendor;
	mainObject[MODEL_NUMBER()] = mModelNumber;
	mainObject[MODEL_NAME()] = mModelName;
	mainObject[AUSWEISAPP_VERSION_NUMBER()] = mAusweisAppVersionNumber;
	mainObject[NFC_TAG_TYPE()] = mNfcTagType;

#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		return QJsonDocument(mainObject).toJson(QJsonDocument::Indented);
	}
#endif

	return QJsonDocument(mainObject).toJson(QJsonDocument::Compact);
}


void SurveyModel::resetNfcData()
{
	mNfcDataAvailable = false;
	mNfcTagType = QStringLiteral("U");
	mMaximumNfcPacketLength = 0;
	buildDataObject();
}


void SurveyModel::setReaderInfo(const ReaderInfo& pReaderInfo)
{
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return;
	}

	if (pReaderInfo.getPlugInType() != ReaderManagerPlugInType::NFC)
	{
		if (!mAuthWasSuccessful)
		{
			resetNfcData();
		}

		return;
	}

	if (pReaderInfo.hasEid())
	{
		mMaximumNfcPacketLength = pReaderInfo.getMaxApduLength();

		switch (pReaderInfo.getCardInfo().getTagType())
		{
			case CardInfo::TagType::NFC_4A:
				mNfcTagType = QStringLiteral("A");
				break;

			case CardInfo::TagType::NFC_4B:
				mNfcTagType = QStringLiteral("B");
				break;

			default:
				mNfcTagType = QStringLiteral("U");
		}
		mNfcDataAvailable = true;
		buildDataObject();
	}
}


void SurveyModel::setAuthWasSuccessful(bool pSuccess)
{
	mAuthWasSuccessful = pSuccess && mNfcDataAvailable;
}


bool SurveyModel::askForDeviceSurvey() const
{
	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	return mNfcDataAvailable && mAuthWasSuccessful && settings.askForDeviceSurvey();
}


bool SurveyModel::isDeviceSurveyPending() const
{
	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	return mNfcDataAvailable && mAuthWasSuccessful && settings.isDeviceSurveyPending();
}


void SurveyModel::setDeviceSurveyPending(bool pValue) const
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(pValue);
}


void SurveyModel::transmitSurvey()
{
	const QUrl whitelistServerBaseUrl = Env::getSingleton<SecureStorage>()->getWhitelistServerBaseUrl();

	const QUrl postSurveyUrl(whitelistServerBaseUrl.toString() + QStringLiteral("/new"));
	QNetworkRequest request(postSurveyUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json; charset=UTF-8"));

	const QByteArray jsonData = toJsonByteArray();
	mReply = Env::getSingleton<NetworkManager>()->postAsUpdater(request, jsonData);

	connect(mReply.data(), &QNetworkReply::finished, this, &SurveyModel::onNetworkReplyFinished);

	qDebug().noquote() << "Sent survey to whitelist server:" << jsonData;
}


void SurveyModel::onNetworkReplyFinished()
{
	if (mReply)
	{
		qDebug() << "Survey result:" << mReply->error();
		(void) NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));
		mReply->disconnect(this);
		mReply.reset();
	}
}
