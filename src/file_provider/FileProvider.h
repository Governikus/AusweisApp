/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class that holds a table of the UpdatableFile instances currently in use.
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
