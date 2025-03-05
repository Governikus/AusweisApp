/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "UpdatableFile.h"

#include <QHash>
#include <QMutex>
#include <QSharedPointer>


namespace governikus
{
class FileProvider
{
	Q_GADGET
	friend class Env;

	private:
		QHash<QString, QSharedPointer<UpdatableFile>> mUpdatableFiles;
		QMutex mGetFileMutex;

	protected:
		FileProvider();
		~FileProvider() = default;
		static FileProvider& getInstance();

	public:
		QSharedPointer<UpdatableFile> getFile(const QString& pSection, const QString& pName, const QString& pDefaultPath = QString());
};

} // namespace governikus
