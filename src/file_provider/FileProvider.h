/*!
 * \brief Class that holds a table of the UpdatableFile instances currently in use.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UpdatableFile.h"

#include <QHash>
#include <QSharedPointer>


namespace governikus
{
class FileProvider
{
	Q_GADGET

	private:
		QHash<QString, QSharedPointer<UpdatableFile> > mUpdatableFiles;

	protected:
		FileProvider();
		~FileProvider() = default;

	public:
		const QSharedPointer<UpdatableFile> getFile(const QString& pSection, const QString& pName, const QString& pDefaultPath = QString());

		static FileProvider& getInstance();
};

} // namespace governikus
