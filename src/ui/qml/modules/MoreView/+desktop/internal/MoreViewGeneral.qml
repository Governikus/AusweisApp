/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

GPane {
	id: root

	signal startOnboarding

	spacing: Style.dimens.pane_spacing

	GMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Start onboarding")
		iconSource: "qrc:/images/npa.svg"
		tintIcon: false
		//: LABEL DESKTOP
		title: qsTr("Onboarding")

		onClicked: root.startOnboarding()
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language)
		iconSource: "qrc:/images/faq_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("FAQ - Frequently asked questions")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language)
		iconSource: "qrc:/images/desktop/help_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("Contact")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language)
		iconSource: "qrc:/images/identify.svg"

		//: LABEL DESKTOP
		title: qsTr("List of Providers")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
}
