/*!
 * \brief Handles updating information about the App release
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReleaseInformation.h"

namespace governikus
{

class ReleaseInformationConfiguration
	: public QObject
{
	Q_OBJECT

	private:
		void setUpdateInformation(ReleaseInformation* pInfo);
		void setCurrentInformation(ReleaseInformation* pInfo);

		QSharedPointer<ReleaseInformation> mReleaseInformationCurrent;
		QSharedPointer<ReleaseInformation> mReleaseInformationUpdate;

	public:
		ReleaseInformationConfiguration();
		~ReleaseInformationConfiguration() override = default;

		void update();
		void reload();
		void updateIfNeeded();
		void setUpdateVersion(const VersionNumber& pVersion);
		[[nodiscard]] QSharedPointer<const ReleaseInformation> getCurrentInformation() const;
		[[nodiscard]] QSharedPointer<const ReleaseInformation> getUpdateInformation() const;

	Q_SIGNALS:
		void fireCurrentInformationChanged();
		void fireUpdateInformationChanged();

};

} // namespace governikus
