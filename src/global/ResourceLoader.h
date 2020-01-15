/*
 * \brief Handler to load and manage resources and resource files.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QStringList>

namespace governikus
{

class ResourceLoader
{
	private:
		const QStringList mFilenames;
		QStringList mLoadedResources;

		Q_DISABLE_COPY(ResourceLoader)

	protected:
		ResourceLoader();
		~ResourceLoader();

	public:
		static ResourceLoader& getInstance();
		void init();
		void shutdown();
		bool isLoaded();
};

} // namespace governikus
