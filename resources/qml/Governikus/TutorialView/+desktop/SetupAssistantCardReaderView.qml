/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

SectionPage {
	id: baseItem

	signal advanceSetupAssistant()

	activeFocusOnTab: false

	SetupAssistantButton {
		id: disagreeButton

		width: ApplicationModel.scaleFactor * 120
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.margins: Constants.component_spacing

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		subText: qsTr("Advance") + SettingsModel.translationTrigger
		buttonImage: "qrc:///images/desktop/continue_arrow.svg"
		onClicked: baseItem.advanceSetupAssistant()
	}
}
