/*!
 * \brief Model for accessing PIN, CAN, PUK, according to the
 * currently active workflow.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "Env.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>

namespace governikus
{

defineEnumType(PasswordType, TRANSPORT_PIN, PIN, CAN, PUK, NEW_PIN, REMOTE_PIN, SMART_PIN, NEW_SMART_PIN, SMART_BLOCKING_CODE)

class NumberModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(PasswordType passwordType READ getPasswordType NOTIFY firePasswordTypeChanged)
	Q_PROPERTY(QString can READ getCan WRITE setCan NOTIFY fireCanChanged)
	Q_PROPERTY(QString pin READ getPin WRITE setPin NOTIFY firePinChanged)
	Q_PROPERTY(QString newPin READ getNewPin WRITE setNewPin NOTIFY fireNewPinChanged)
	Q_PROPERTY(QString puk READ getPuk WRITE setPuk NOTIFY firePukChanged)
	Q_PROPERTY(bool hasError READ hasError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(bool hasPasswordError READ hasPasswordError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(QString inputError READ getInputError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(int retryCounter READ getRetryCounter NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool isCanAllowedMode READ isCanAllowedMode NOTIFY fireCanAllowedModeChanged)

	private:
		QSharedPointer<WorkflowContext> mContext;

		NumberModel();
		~NumberModel() override = default;

		CardReturnCode getInputErrorCode() const;

	private Q_SLOTS:
		void onCardConnectionChanged();

	public:
		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		PasswordType getPasswordType() const;

		QString getCan() const;
		void setCan(const QString& pCan);

		QString getPin() const;
		void setPin(const QString& pPin);

		QString getNewPin() const;
		void setNewPin(const QString& pNewPin);

		QString getPuk() const;
		void setPuk(const QString& pPuk);

		bool hasError() const;
		bool hasPasswordError() const;
		QString getInputError() const;

		int getRetryCounter() const;
		bool isCanAllowedMode() const;

	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pInfo);

	Q_SIGNALS:
		void fireCanChanged();
		void firePinChanged();
		void fireNewPinChanged();
		void firePukChanged();
		void fireInputErrorChanged();
		void fireReaderInfoChanged();
		void fireCanAllowedModeChanged();
		void firePasswordTypeChanged();
};


} // namespace governikus
