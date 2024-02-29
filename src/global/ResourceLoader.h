/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Handler to load and manage resources and resource files.
 */

#pragma once

#include <QStringList>

namespace governikus
{

class ResourceLoader
{
	Q_DISABLE_COPY(ResourceLoader)

	private:
		const QStringList mFilenames;
		QStringList mLoadedResources;

	protected:
		ResourceLoader();
		~ResourceLoader();

	public:
		static ResourceLoader& getInstance();
		void init();
		void shutdown();
		[[nodiscard]] bool isLoaded() const;
};

} // namespace governikus
