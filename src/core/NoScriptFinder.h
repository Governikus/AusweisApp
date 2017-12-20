/*!
 * \brief Determines, if a Firefox browser with NoScript extension is installed
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>

namespace governikus
{


class NoScriptFinderPrivate;


class NoScriptFinder
	: public QObject
{
	Q_OBJECT

	Q_DECLARE_PRIVATE(NoScriptFinder)
	NoScriptFinderPrivate * const d_ptr;

	public:
		NoScriptFinder(QObject* pParent = nullptr);
		~NoScriptFinder();

		bool isExtensionFound();
};


} /* namespace governikus */
