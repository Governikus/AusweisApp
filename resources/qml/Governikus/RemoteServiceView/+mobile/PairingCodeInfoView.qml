/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	property alias text: description.text

	signal navActionClicked

	hiddenNavbarPadding: true

	//: LABEL ANDROID IOS
	title: qsTr("Pairing Information")

	content: ColumnLayout {
		anchors.left: parent.left
		anchors.margins: Constants.pane_padding
		anchors.right: parent.right
		spacing: Constants.component_spacing

		Image {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: Style.dimens.medium_icon_size
			Layout.topMargin: Constants.pane_padding
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/phone_to_pc.svg"
			sourceSize.height: Style.dimens.medium_icon_size
		}
		GText {
			id: description
			Layout.alignment: Qt.AlignCenter
			Layout.topMargin: Constants.component_spacing
			//: LABEL ANDROID IOS
			textStyle: Style.text.header_accent
		}
		Repeater {
			model: [
				//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 3
				qsTr("Open %1 on your %2other device%3.").arg(Qt.application.name).arg("<b>").arg("</b>"),
				//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 3. %1 and %2 are surrounding tags for bold font.
				qsTr("On that device go to %1Settings%2 and then %1Smartphone as card reader%2 resp. %1Manage pairings%2.").arg("<b>").arg("</b>"),
				//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 3
				qsTr("Choose this smartphone in the list to pair it.")]

			RowLayout {
				Layout.topMargin: Constants.component_spacing
				spacing: Constants.text_spacing
				width: parent.width

				GText {
					Accessible.ignored: true
					Layout.alignment: Qt.AlignTop
					Layout.preferredWidth: Style.dimens.small_icon_size
					text: (index + 1) + "."
				}
				GText {
					Accessible.name: (index + 1) + ". " + ApplicationModel.stripHtmlTags(modelData)
					Layout.alignment: Qt.AlignTop
					Layout.fillWidth: true
					text: modelData
				}
			}
		}
		RemoteServiceWifiInfo {
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing
		}
	}
	navigationAction: NavigationAction {
		id: navAction
		action: NavigationAction.Action.Back

		onClicked: navActionClicked()
	}
}
