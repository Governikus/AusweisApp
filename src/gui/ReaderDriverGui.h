/*!
 * ReaderDriverGui.h
 *
 * \brief Qt widget based ReaderDriverUi implementation.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ReaderDriverDialog.h"
#include "context/ReaderDriverContext.h"

class QWidget;

namespace governikus
{

class ReaderDriverGui
	: public QObject
{
	Q_OBJECT

	public:
		ReaderDriverGui(QWidget* pParentWidget);
		virtual ~ReaderDriverGui();

		void activate();
		void deactivate();

	Q_SIGNALS:
		void fireFinished();

	private:
		ReaderDriverDialog* mDialog;
		void reactivate();

	private Q_SLOTS:
		void onFinished(int result);
};

} /* namespace governikus */
