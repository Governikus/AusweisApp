/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property bool showRemoteReaderInfos: {
		switch (ApplicationModel.currentWorkflow) {
		case ApplicationModel.Workflow.CHANGE_PIN:
		case ApplicationModel.Workflow.AUTHENTICATION:
		case ApplicationModel.Workflow.SELF_AUTHENTICATION:
			return true;
		default:
			return false;
		}
	}

	signal showRemoteServiceSettings

	spacing: Style.dimens.pane_spacing
	//: LABEL ANDROID IOS
	title: qsTr("My ID card is not detected")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	GText {
		//: INFO ANDROID IOS
		text: qsTr("If your ID card is not recognized when it is read, there could be various reasons.")
	}
	ResultNfcAnimation {
		Layout.alignment: Qt.AlignHCenter
		sourceSize.height: Style.dimens.large_icon_size
		symbol.type: Symbol.Type.QUESTION
	}
	ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: INFO ANDROID IOS
			text: qsTr("NFC interface")
			textStyle: Style.text.subline
		}
		GText {
			//: INFO ANDROID IOS
			text: qsTr("The NFC interface, and therefore the point at which the ID card must be placed, varies depending on the smartphone model. Find out where the NFC interface is on your smartphone model.")
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			icon.source: "qrc:///images/open_website.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Tutorial: Using NFC")
			tintIcon: true

			onClicked: {
				if (Qt.platform.os === "ios") {
					Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/video-nfc-ios".arg(SettingsModel.language));
				} else {
					Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/video-nfc-android".arg(SettingsModel.language));
				}
			}
		}
	}
	ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: INFO ANDROID IOS
			text: qsTr("Sources of interference")
			textStyle: Style.text.subline
		}
		GText {
			text: qsTr("Remove your phone case and allow the ID card to come into direct contact with the device. Low battery power can negatively impact NFC functionality, so make sure your smartphone is charged and not in power saving mode.")
		}
	}
	ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: INFO ANDROID IOS
			text: qsTr("Alternatives")
			textStyle: Style.text.subline
		}
		GText {
			//: INFO ANDROID IOS
			text: qsTr("If the connection does not work despite following the tips, you can use another smartphone as a card reader.")
			visible: root.showRemoteReaderInfos
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			//: INFO ANDROID IOS
			text: qsTr("Connect using a card reader")
			tintIcon: true
			visible: root.showRemoteReaderInfos

			onClicked: root.showRemoteServiceSettings()
		}
		GText {
			//: INFO ANDROID IOS "%1" is replaced with "AusweisApp"
			text: qsTr("You can also use %1 on a PC and connect an USB card reader to read the ID card.").arg(Qt.application.name)
		}
	}
}
