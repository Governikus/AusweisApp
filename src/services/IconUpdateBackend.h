/*
 * \brief Class that ensures that all the icons and images specified
 *        in the application settings are in the application cache.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "UpdateBackend.h"

#include <QDateTime>
#include <QObject>


namespace governikus
{
class IconUpdateBackend
	: public QObject
	, public UpdateBackend
{
	Q_OBJECT

	private:
		const QString mIcon;

		const QString mIconPathInCache;

	public:
		IconUpdateBackend(const QString& pIconCacheBasePath, const QString& pIcon);

		virtual ~IconUpdateBackend();

		virtual QDateTime getIssueDate() const override;

		virtual void processSuccess(const QByteArray& pData) override;

		virtual void processError() override;

};


} /* namespace governikus */
