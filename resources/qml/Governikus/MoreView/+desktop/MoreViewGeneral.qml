/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import Governikus.Type.UiModule

GPane {
	spacing: Constants.component_spacing

	MoreViewMenuItem {
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
	MoreViewMenuItem {
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
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
		iconSource: "qrc:/images/desktop/privacy_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("Privacy statement")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language)
		iconSource: "qrc:/images/desktop/a11y_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("Accessibility statement")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/open_website.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you want to see a list of service providers?")
		iconSource: "qrc:/images/identify.svg"

		//: LABEL DESKTOP
		title: qsTr("List of Providers")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
}
