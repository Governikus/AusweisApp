/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model for accessing PIN, CAN, PUK, according to the
 * currently active workflow.
 */

#pragma once

#include "EnumHelper.h"
#include "Env.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>


class test_UIPlugInQml;


namespace governikus
{

defineEnumType(PasswordType, TRANSPORT_PIN, PIN, CAN, PUK, NEW_PIN, NEW_PIN_CONFIRMATION, REMOTE_PIN, SMART_PIN, NEW_SMART_PIN, NEW_SMART_PIN_CONFIRMATION, SMART_BLOCKING_CODE)

class NumberModel
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_UIPlugInQml;

	Q_PROPERTY(PasswordType passwordType READ getPasswordType NOTIFY firePasswordTypeChanged)
	Q_PROPERTY(QString can READ getCan WRITE setCan NOTIFY fireCanChanged)
	Q_PROPERTY(QString pin READ getPin WRITE setPin NOTIFY firePinChanged)
	Q_PROPERTY(QString newPin READ getNewPin WRITE setNewPin NOTIFY fireNewPinChanged)
	Q_PROPERTY(QString newPinConfirmation READ getNewPinConfirmation WRITE setNewPinConfirmation NOTIFY fireNewPinConfirmationChanged)
	Q_PROPERTY(QString puk READ getPuk WRITE setPuk NOTIFY firePukChanged)
	Q_PROPERTY(bool hasPasswordError READ hasPasswordError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(QString inputError READ getInputError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(int retryCounter READ getRetryCounter NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool isCanAllowedMode READ isCanAllowedMode NOTIFY fireCanAllowedModeChanged)

	private:
		QSharedPointer<WorkflowContext> mContext;
		QString mNewPin;
		QString mNewPinConfirmation;

		NumberModel();
		~NumberModel() override = default;

		CardReturnCode getInputErrorCode() const;
		void clearNewPinAndConfirmation();
		bool newPinAndConfirmationMatch() const;

	private Q_SLOTS:
		void onCardConnectionChanged();

	public:
		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] PasswordType getPasswordType() const;

		[[nodiscard]] QString getCan() const;
		void setCan(const QString& pCan);

		[[nodiscard]] QString getPin() const;
		void setPin(const QString& pPin);

		[[nodiscard]] QString getNewPin() const;
		void setNewPin(const QString& pNewPin);
		[[nodiscard]] QString getNewPinConfirmation() const;
		void setNewPinConfirmation(const QString& pNewPinConfirmation);
		Q_INVOKABLE bool commitNewPin();

		[[nodiscard]] QString getPuk() const;
		void setPuk(const QString& pPuk);

		[[nodiscard]] bool hasPasswordError() const;
		[[nodiscard]] QString getInputError() const;

		[[nodiscard]] int getRetryCounter() const;
		[[nodiscard]] bool isCanAllowedMode() const;

	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pInfo);

	Q_SIGNALS:
		void fireCanChanged();
		void firePinChanged();
		void fireNewPinChanged();
		void fireNewPinConfirmationChanged();
		void firePukChanged();
		void fireInputErrorChanged();
		void fireReaderInfoChanged();
		void fireCanAllowedModeChanged();
		void firePasswordTypeChanged();
};


} // namespace governikus
