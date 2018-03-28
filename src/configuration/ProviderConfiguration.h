/*!
 * \brief Provides information of provider json.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"
#include "ProviderConfigurationInfo.h"
#include "UpdatableFile.h"

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>


namespace governikus
{


class ProviderConfiguration
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<UpdatableFile> mUpdatableFile;
		QVector<ProviderConfigurationInfo> mProviderConfigurationInfos;
		QMap<QString, CallCost> mCallCosts;

		bool parseProviderConfiguration();

	private Q_SLOTS:
		void onFileUpdated();

	protected:
		ProviderConfiguration();
		virtual ~ProviderConfiguration() = default;

	public:
		static ProviderConfiguration& getInstance();

		void update();
		const QVector<ProviderConfigurationInfo>& getProviderConfigurationInfos() const;
		const CallCost getCallCost(const ProviderConfigurationInfo& pProvider) const;

	Q_SIGNALS:
		void fireUpdated();
};


} /* namespace governikus */
