/*!
 * \brief If a QLowEnergyCharacteristic is able to notify about changes, i.e.
 * it has property QLowEnergyCharacteristic::PropertyType::Notify,
 * one needs to enable notification explicitly.
 * To stop notifications one need to disable it explicitly.
 *
 * For details see the Qt documentation on QLowEnergyService and topic "Service Interaction".
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QEventLoop>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>


namespace governikus
{

class NotificationEnabler
	: public QObject
{
	Q_OBJECT

	private:
		QLowEnergyService* mService;
		QLowEnergyDescriptor mDescriptor;
		QEventLoop mEventLoop;
		int mTimeoutSeconds;
		QByteArray mValueWritten;
		QByteArray mValueConfirmed;

		Q_DISABLE_COPY(NotificationEnabler)

		bool writeToDescriptor(const QByteArray& pValue);

	private Q_SLOTS:
		void onDescriptorWritten(const QLowEnergyDescriptor& pDescriptor, const QByteArray& pNewValue);
		void onServiceError(QLowEnergyService::ServiceError pError);

	public:
		/*!
		 * \param pService the service corresponding to the characteristic to be enabled/disabled.
		 * \param pTimeoutSeconds time out in seconds.
		 */
		NotificationEnabler(QLowEnergyService* pService, int pTimeoutSeconds = 5);
		virtual ~NotificationEnabler() = default;

		bool enable(const QLowEnergyCharacteristic& pCharacteristic);
		bool disable(const QLowEnergyCharacteristic& pCharacteristic);
};

} // namespace governikus
