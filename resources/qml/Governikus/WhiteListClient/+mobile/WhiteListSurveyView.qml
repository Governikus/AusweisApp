/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.SurveyModel

FlickableSectionPage {
	id: root

	signal done(bool pUserAccepted)

	spacing: Constants.pane_spacing
	//: LABEL ANDROID IOS
	title: qsTr("Feedback")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.done(false)
	}

	PaneTitle {
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Header
		text: qsTr("Send device data?")
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
		text: qsTr("Would you like to help us to improve the %1?").arg(Qt.application.name)
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
		text: qsTr("Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.")
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
		text: qsTr("The transmission is anonymous. No personal data is collected or transmitted!")
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GCollapsible {
		Layout.fillWidth: true
		Layout.leftMargin: -Constants.pane_padding * 2
		Layout.rightMargin: -Constants.pane_padding * 2
		horizontalMargin: Constants.pane_padding * 3
		title: qsTr("Collected data")

		Repeater {
			model: SurveyModel

			delegate: LabeledText {
				Layout.fillWidth: true
				label: title
				text: value
			}
		}
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Thank you message
		text: qsTr("Thank you for your assistance!")
	}
	GSpacer {
		Layout.fillHeight: true
	}
	Row {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		Layout.topMargin: Constants.pane_padding
		height: childrenRect.height
		spacing: Constants.component_spacing

		GButton {
			//: LABEL ANDROID IOS
			text: qsTr("Do not send")

			onClicked: root.done(false)
		}
		GButton {
			//: LABEL ANDROID IOS
			text: qsTr("Send")

			onClicked: root.done(true)
		}
	}
}
