/*!
 * \brief Utility for sharing text.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QString>

namespace governikus
{

class ShareUtil
	: public QObject
{
	Q_OBJECT

	public:
		Q_INVOKABLE void shareText(const QString& pText, const QString& pChooserTitle);
};

}
