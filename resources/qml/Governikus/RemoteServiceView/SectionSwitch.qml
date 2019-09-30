/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


Rectangle {
	id: baseItem
	height: sectionRow.height
	color: Style.color.background

	enum Section {
		STARTSTOP = 0,
		REMOTE = 1,
		LOCAL = 2
	}
	property int selectedSection: SectionSwitch.Section.STARTSTOP

	Row {
		id: sectionRow
		spacing: 30
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		TechnologySwitchButton {
			buttonActive: selectedSection !== SectionSwitch.Section.STARTSTOP
			onClicked: selectedSection = SectionSwitch.Section.STARTSTOP
			imageSource: "qrc:///images/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Service") + SettingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedSection !== SectionSwitch.Section.REMOTE
			onClicked: selectedSection = SectionSwitch.Section.REMOTE
			imageSource: "qrc:///images/icon_pair.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Pairing") + SettingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedSection !== SectionSwitch.Section.LOCAL
			onClicked: selectedSection = SectionSwitch.Section.LOCAL
			imageSource: "qrc:///images/icon_settings.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Settings") + SettingsModel.translationTrigger
		}
	}
}
