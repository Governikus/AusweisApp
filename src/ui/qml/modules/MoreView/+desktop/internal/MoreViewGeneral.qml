/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Type

GPane {
	spacing: Constants.component_spacing

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
