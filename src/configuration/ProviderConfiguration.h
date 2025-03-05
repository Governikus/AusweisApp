/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"
#include "Env.h"
#include "ProviderConfigurationInfo.h"
#include "UpdatableFile.h"

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>


class test_PinResetInformationModel;


namespace governikus
{

class ProviderConfiguration
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_PinResetInformationModel;

	private:
		const QSharedPointer<UpdatableFile> mUpdatableFile;
		QList<ProviderConfigurationInfo> mProviderConfigurationInfos;
		QMap<QString, CallCost> mCallCosts;

		ProviderConfiguration();
		~ProviderConfiguration() override = default;
		bool parseProviderConfiguration(const QString& pPath);

	private Q_SLOTS:
		void onFileUpdated();

	public:
		void update();
		[[nodiscard]] const QList<ProviderConfigurationInfo>& getProviderConfigurationInfos() const;
		[[nodiscard]] CallCost getCallCost(const ProviderConfigurationInfo& pProvider) const;
		[[nodiscard]] ProviderConfigurationInfo getProviderInfo(const QString& pInternalId) const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();
};


} // namespace governikus
