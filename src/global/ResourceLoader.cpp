/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"

#include "FileDestination.h"
#include "SingletonHelper.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QResource>

#ifndef QT_NO_DEBUG
	#include <QDirIterator>
#endif

using namespace governikus;

defineSingleton(ResourceLoader)

ResourceLoader::ResourceLoader()
	: mFilenames(
		{
			QStringLiteral("AusweisApp.rcc")
		})
	, mLoadedResources()
{
}


ResourceLoader::~ResourceLoader()
{
	shutdown();
}


void ResourceLoader::init()
{
	for (const auto& file : mFilenames)
	{
		const QString path = FileDestination::getPath(file);
		const auto loaded = QResource::registerResource(path);
		qDebug() << "Register resource:" << path << '|' << loaded;
		if (loaded)
		{
			mLoadedResources << path;
		}
	}

#ifndef QT_NO_DEBUG
	const auto& show = qEnvironmentVariable("SHOW_RESOURCES").toLower();
	if (show == QLatin1String("true") || show == QLatin1String("on") || show == QLatin1String("1"))
	{
		QDirIterator iter(QStringLiteral(":"), QDirIterator::Subdirectories);
		while (iter.hasNext())
		{
			qDebug() << iter.next();
		}
	}
#endif
}


void ResourceLoader::shutdown()
{
	for (const auto& path : std::as_const(mLoadedResources))
	{
		const auto result = QResource::unregisterResource(path);
		qDebug() << "Unregister resource:" << path << '|' << result;
	}
	mLoadedResources.clear();
}


bool ResourceLoader::isLoaded() const
{
	return !mLoadedResources.isEmpty();
}
