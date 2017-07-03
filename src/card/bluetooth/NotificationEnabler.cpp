/*!
 * NotificationEnabler.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */
#include "NotificationEnabler.h"

#include <QLoggingCategory>
#include <QTimer>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(bluetooth)


NotificationEnabler::NotificationEnabler(QLowEnergyService* pService, int pTimeoutSeconds)
	: mService(pService)
	, mDescriptor()
	, mEventLoop()
	, mTimeoutSeconds(pTimeoutSeconds)
	, mValueWritten()
	, mValueConfirmed()
{
	Q_ASSERT(mTimeoutSeconds > 0);
	connect(mService, &QLowEnergyService::descriptorWritten, this, &NotificationEnabler::onDescriptorWritten);
	connect(mService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &NotificationEnabler::onServiceError);
}


NotificationEnabler::~NotificationEnabler()
{
}


bool NotificationEnabler::enable(const QLowEnergyCharacteristic& pCharacteristic)
{
	// see Qt documentation on QLowEnergyService and topic "Service Interaction"
	mDescriptor = pCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
	return writeToDescriptor(QByteArray::fromHex("0100"));
}


bool NotificationEnabler::disable(const QLowEnergyCharacteristic& pCharacteristic)
{
	// see Qt documentation on QLowEnergyService and topic "Service Interaction"
	mDescriptor = pCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
	return writeToDescriptor(QByteArray::fromHex("0000"));
}


bool NotificationEnabler::writeToDescriptor(const QByteArray& pValue)
{
	mValueWritten = pValue;

	if (!mDescriptor.isValid())
	{
		qCCritical(bluetooth) << "Cannot write to descriptor, the descriptor is invalid";
		return false;
	}

	QTimer::singleShot(mTimeoutSeconds * 1000, &mEventLoop, &QEventLoop::quit);
	mService->writeDescriptor(mDescriptor, mValueWritten);
	qCDebug(bluetooth) << "Value written to descriptor" << mValueWritten.toHex();
	mEventLoop.exec();

	return mValueWritten == mValueConfirmed;
}


void NotificationEnabler::onDescriptorWritten(const QLowEnergyDescriptor& pDescriptor, const QByteArray& pNewValue)
{
	if (pDescriptor != mDescriptor)
	{
		return;
	}
	mValueConfirmed += pNewValue;
	if (mValueWritten == mValueConfirmed)
	{
		mEventLoop.quit();
	}
}


void NotificationEnabler::onServiceError(QLowEnergyService::ServiceError pError)
{
	qCCritical(bluetooth) << "Service error" << pError << "abort write";
	mEventLoop.quit();
}
