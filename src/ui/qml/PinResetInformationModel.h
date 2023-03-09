/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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
	Q_PROPERTY(QString noPinAndNoPukHint READ getNoPinAndNoPukHint NOTIFY fireUpdated)
	Q_PROPERTY(QString requestNewPinHint READ getRequestNewPinHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionHint READ getActivateOnlineFunctionHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionDescription READ getActivateOnlineFunctionDescription NOTIFY fireUpdated)
	Q_PROPERTY(QString pinResetActionText READ getPinResetActionText NOTIFY fireUpdated)

	public:
		PinResetInformationModel();

		[[nodiscard]] QUrl getPinResetUrl() const;
		[[nodiscard]] QString getNoPinAndNoPukHint() const;
		[[nodiscard]] QString getRequestNewPinHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionDescription() const;
		[[nodiscard]] QString getActivateOnlineFunctionActionText() const;
		[[nodiscard]] QString getPinResetActionText() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireUpdated();
};

} // namespace governikus
