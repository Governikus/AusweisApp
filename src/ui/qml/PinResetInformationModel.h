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

		QUrl getPinResetUrl() const;
		QString getPinUnknownText() const;
		QString getPinUnknownHint() const;
		QString getNoPinAndNoPukHint() const;
		QString getNoPinAndNoTransportPinHint() const;
		QString getPinForgottenHint() const;
		QString getPinForgottenTutorialHint() const;
		QString getRequestNewPinHint() const;
		QString getActivateOnlineFunctionHint() const;
		QString getActivateOnlineFunctionDescription() const;
		QString getPinResetActionText() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireUpdated();
};

} // namespace governikus
