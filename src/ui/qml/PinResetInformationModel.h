/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>

namespace governikus
{

class PinResetInformationModel
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QUrl pinResetUrl READ getPinResetUrl NOTIFY fireUpdated)
	Q_PROPERTY(QString pinUnknownText READ getPinUnknownText NOTIFY fireUpdated)
	Q_PROPERTY(QString pinUnknownHint READ getPinUnknownHint NOTIFY fireUpdated)
	Q_PROPERTY(QString noPinAndNoPukHint READ getNoPinAndNoPukHint NOTIFY fireUpdated)
	Q_PROPERTY(QString noPinAndNoTransportPinHint READ getNoPinAndNoTransportPinHint NOTIFY fireUpdated)
	Q_PROPERTY(QString pinForgottenHint READ getPinForgottenHint NOTIFY fireUpdated)
	Q_PROPERTY(QString pinForgottenTutorialHint READ getPinForgottenTutorialHint NOTIFY fireUpdated)
	Q_PROPERTY(QString requestNewPinHint READ getRequestNewPinHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionHint READ getActivateOnlineFunctionHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionDescription READ getActivateOnlineFunctionDescription NOTIFY fireUpdated)
	Q_PROPERTY(QString pinResetActionText READ getPinResetActionText NOTIFY fireUpdated)

	private:
		bool hasPinResetService() const;

	public:
		PinResetInformationModel();

		[[nodiscard]] QUrl getPinResetUrl() const;
		[[nodiscard]] QString getPinUnknownText() const;
		[[nodiscard]] QString getPinUnknownHint() const;
		[[nodiscard]] QString getNoPinAndNoPukHint() const;
		[[nodiscard]] QString getNoPinAndNoTransportPinHint() const;
		[[nodiscard]] QString getPinForgottenHint() const;
		[[nodiscard]] QString getPinForgottenTutorialHint() const;
		[[nodiscard]] QString getRequestNewPinHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionDescription() const;
		[[nodiscard]] QString getPinResetActionText() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireUpdated();
};

} // namespace governikus
