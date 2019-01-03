/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SetupAssistantGui.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


SetupAssistantGui::SetupAssistantGui(QWidget* pParentWidget)
	: QObject(pParentWidget)
	, mWizard(nullptr)
{
}


SetupAssistantGui::~SetupAssistantGui()
{
}


void SetupAssistantGui::activate()
{
	if (!mWizard)
	{
		QWidget* dialogParent = qobject_cast<QWidget*>(parent());
		if (!dialogParent)
		{
			return;
		}

		mWizard = new SetupAssistantWizard(dialogParent);
		connect(mWizard.data(), &SetupAssistantWizard::fireChangePinButtonClicked, this, &SetupAssistantGui::fireChangePinButtonClicked);
	}

	mWizard->exec();
}


void SetupAssistantGui::deactivate()
{
	if (mWizard)
	{
		mWizard->close();
	}
}
