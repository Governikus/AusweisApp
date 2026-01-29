/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace governikus
{

class PinResetInformationModel
	: public QObject
	, public SingletonCreator<PinResetInformationModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;

	Q_PROPERTY(bool hasPinResetService READ hasPinResetService NOTIFY fireUpdated)

	Q_PROPERTY(QUrl pinResetUrl READ getPinResetUrl NOTIFY fireUpdated)
	Q_PROPERTY(QUrl pinResetActivationUrl READ getPinResetActivationUrl CONSTANT)
	Q_PROPERTY(QUrl administrativeSearchUrl READ getAdministrativeSearchUrl NOTIFY fireUpdated)

	Q_PROPERTY(QString activateOnlineFunctionForPRSHint READ getActivateOnlineFunctionForPRSHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionAtAuthorityHint READ getActivateOnlineFunctionAtAuthorityHint NOTIFY fireUpdated)
	Q_PROPERTY(QString activateOnlineFunctionDescription READ getActivateOnlineFunctionDescription NOTIFY fireUpdated)

	Q_PROPERTY(QString resetPinWithPRSActionText READ getResetPinWithPRSActionText NOTIFY fireUpdated)
	Q_PROPERTY(QString resetPinAtAuthorityActionText READ getResetPinAtAuthorityActionText NOTIFY fireUpdated)

	Q_PROPERTY(QString resetPinWithPRSHintTitle READ getResetPinWithPRSHintTitle NOTIFY fireUpdated)
	Q_PROPERTY(QString resetPinAtAuthorityHintTitle READ getResetPinAtAuthorityHintTitle NOTIFY fireUpdated)

	Q_PROPERTY(QString resetPinWithPRSHint READ getResetPinWithPRSHint NOTIFY fireUpdated)
	Q_PROPERTY(QString resetPinAtAuthorityHint READ getResetPinAtAuthorityHint NOTIFY fireUpdated)

	private:
		PinResetInformationModel();
		~PinResetInformationModel() override = default;

	public:
		bool hasPinResetService() const;

		[[nodiscard]] QUrl getPinResetUrl() const;
		[[nodiscard]] QUrl getAdministrativeSearchUrl() const;
		[[nodiscard]] QUrl getPinResetActivationUrl() const;

		[[nodiscard]] QString getActivateOnlineFunctionForPRSHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionAtAuthorityHint() const;
		[[nodiscard]] QString getActivateOnlineFunctionActionText() const;
		[[nodiscard]] QString getActivateOnlineFunctionDescription() const;

		[[nodiscard]] QString getResetPinWithPRSActionText() const;
		[[nodiscard]] QString getResetPinAtAuthorityActionText() const;

		[[nodiscard]] QString getResetPinWithPRSHintTitle() const;
		[[nodiscard]] QString getResetPinAtAuthorityHintTitle() const;

		[[nodiscard]] QString getResetPinWithPRSHint() const;
		[[nodiscard]] QString getResetPinAtAuthorityHint() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireUpdated();
};

} // namespace governikus
