/*!
 * \brief Provides information of provider json.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"
#include "Env.h"
#include "ProviderConfigurationInfo.h"
#include "UpdatableFile.h"

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class test_HistoryModel;

namespace governikus
{

class ProviderConfiguration
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_HistoryModel;

	private:
		const QSharedPointer<UpdatableFile> mUpdatableFile;
		QVector<ProviderConfigurationInfo> mProviderConfigurationInfos;
		QMap<QString, CallCost> mCallCosts;

		ProviderConfiguration();
		~ProviderConfiguration() override = default;
		bool parseProviderConfiguration(const QString& pPath);

	private Q_SLOTS:
		void onFileUpdated();

	public:
		void update();
		[[nodiscard]] const QVector<ProviderConfigurationInfo>& getProviderConfigurationInfos() const;
		[[nodiscard]] const CallCost getCallCost(const ProviderConfigurationInfo& pProvider) const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();
};


} // namespace governikus
