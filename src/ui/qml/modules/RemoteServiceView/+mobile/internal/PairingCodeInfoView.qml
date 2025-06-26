/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.TitleBar
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	readonly property string currentPin: RemoteServiceModel.psk
	property alias text: headline.text

	signal navActionClicked

	spacing: Style.dimens.pane_spacing

	//: LABEL ANDROID IOS
	title: qsTr("Pairing Information")

	navigationAction: NavigationAction {
		id: navAction

		action: NavigationAction.Action.Back

		onClicked: root.navActionClicked()
	}

	GText {
		id: headline

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: parent.width
		Layout.preferredHeight: Style.dimens.medium_icon_size
		source: "qrc:///images/phone_to_pc.svg"
		sourceSize.height: Style.dimens.medium_icon_size
		tintColor: Style.color.image
	}
	GText {
		//: LABEL ANDROID IOS
		text: qsTr("Please follow these steps:")
		textStyle: Style.text.subline
	}
	Repeater {
		model: [
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 5
			qsTr("Open %1 on your %2other device%3.").arg(Qt.application.name).arg("<b>").arg("</b>"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are surrounding tags for bold font.
			qsTr("Make sure that both devices are on the %1same network%2 (e.g. WiFi).").arg("<b>").arg("</b>"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are surrounding tags for bold font.
			qsTr("On your other device, go to %1Settings%2 and then %1Smartphone as card reader%2 resp. %1Manage pairings%2.").arg("<b>").arg("</b>"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 4 of 5
			qsTr("Choose this smartphone in the list to pair it."),
			//: LABEL ANDROID IOS Provide pairing code. Step 5 of 5
			qsTr("Enter the pairing code \"%1\".").arg(root.currentPin)]

		GText {
			required property int index
			required property string modelData

			Accessible.name: ApplicationModel.stripHtmlTags(text)
			Layout.alignment: Qt.AlignTop
			text: (index + 1) + ". " + modelData
		}
	}
}
