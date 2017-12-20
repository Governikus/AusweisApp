/*!
 * \brief Qt widget based DiagnosisUi implementation.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DiagnosisDialog.h"

#include <QPointer>
#include <QWidget>

namespace governikus
{

class DiagnosisGui
	: public QObject
{
	Q_OBJECT

	public:
		DiagnosisGui(QWidget* pParentWidget);
		virtual ~DiagnosisGui();

		void activate();
		void deactivate();

	Q_SIGNALS:
		void fireFinished();

	private:
		QPointer<DiagnosisDialog> mDialog;
};

} /* namespace governikus */
