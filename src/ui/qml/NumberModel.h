/*!
 * \brief Model for accessing PIN, CAN, PUK, according to the
 * currently active workflow.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "Env.h"

#include <QObject>
#include <QSharedPointer>

class test_NumberModel;

namespace governikus
{

class NumberModel
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_NumberModel;

	Q_PROPERTY(governikus::PacePasswordId establishPaceChannelType READ getEstablishPaceChannelType)
	Q_PROPERTY(QString can READ getCan WRITE setCan NOTIFY fireCanChanged)
	Q_PROPERTY(QString pin READ getPin WRITE setPin NOTIFY firePinChanged)
	Q_PROPERTY(QString newPin READ getNewPin WRITE setNewPin NOTIFY fireNewPinChanged)
	Q_PROPERTY(QString puk READ getPuk WRITE setPuk NOTIFY firePukChanged)
	Q_PROPERTY(bool hasError READ hasError NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(CardReturnCode inputErrorCode READ getInputErrorCode NOTIFY fireInputErrorChanged)
	Q_PROPERTY(QString inputError READ getInputError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(int retryCounter READ getRetryCounter NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool pinDeactivated READ isPinDeactivated NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool isCanAllowedMode READ isCanAllowedMode NOTIFY fireCanAllowedModeChanged)
	Q_PROPERTY(bool requestTransportPin READ isRequestTransportPin WRITE setRequestTransportPin NOTIFY fireRequestTransportPinChanged)

	private:
		QSharedPointer<WorkflowContext> mContext;
		bool mRequestTransportPin;

	private Q_SLOTS:
		void onCardConnectionChanged();

	protected:
		NumberModel();
		~NumberModel() override = default;
		static NumberModel& getInstance();

	public:
		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		PacePasswordId getEstablishPaceChannelType() const;

		QString getCan() const;
		void setCan(const QString& pCan);

		QString getPin() const;
		void setPin(const QString& pPin);

		QString getNewPin() const;
		void setNewPin(const QString& pNewPin);

		QString getPuk() const;
		void setPuk(const QString& pPuk);

		bool hasError();
		CardReturnCode getInputErrorCode() const;
		QString getInputError() const;

		int getRetryCounter() const;
		bool isPinDeactivated() const;
		bool isCanAllowedMode();

		bool isRequestTransportPin() const;
		void setRequestTransportPin(bool pEnabled);

	private Q_SLOTS:
		void onReaderInfoChanged(const QString& pReaderName);

	Q_SIGNALS:
		void fireCanChanged();
		void firePinChanged();
		void fireNewPinChanged();
		void firePukChanged();
		void fireInputErrorChanged();
		void fireReaderInfoChanged();
		void fireCanAllowedModeChanged();
		void fireRequestTransportPinChanged();
};


} // namespace governikus
