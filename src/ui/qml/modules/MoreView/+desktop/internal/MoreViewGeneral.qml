/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type

GPane {
	id: root

	signal startOnboarding

	GMenuItem {
		Layout.fillWidth: true
		//: DESKTOP
		buttonText: qsTr("Start setup")
		iconSource: "qrc:/images/npa.svg"
		tintIcon: false
		//: DESKTOP
		title: qsTr("Setup")

		onClicked: root.startOnboarding()
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language)
		iconSource: "qrc:/images/faq_icon.svg"
		linkToOpen: buttonTooltip

		//: DESKTOP
		title: qsTr("FAQ - Frequently asked questions")
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language)
		iconSource: "qrc:/images/desktop/help_icon.svg"
		linkToOpen: buttonTooltip

		//: DESKTOP
		title: qsTr("Contact")
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language)
		iconSource: "qrc:/images/identify.svg"
		linkToOpen: buttonTooltip

		//: DESKTOP
		title: qsTr("List of Providers")
	}
}
