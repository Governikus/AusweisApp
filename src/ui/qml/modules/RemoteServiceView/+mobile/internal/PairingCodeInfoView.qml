/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	property alias text: descriptionContainer.title

	signal navActionClicked

	spacing: Constants.component_spacing

	//: LABEL ANDROID IOS
	title: qsTr("Pairing Information")

	navigationAction: NavigationAction {
		id: navAction

		action: NavigationAction.Action.Back

		onClicked: navActionClicked()
	}

	GOptionsContainer {
		id: descriptionContainer

		containerPadding: Constants.pane_padding

		ColumnLayout {
			spacing: Constants.component_spacing
			width: parent.width

			TintableIcon {
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: parent.width
				Layout.preferredHeight: Style.dimens.medium_icon_size
				source: "qrc:///images/phone_to_pc.svg"
				sourceSize.height: Style.dimens.medium_icon_size
				tintColor: Style.color.image
			}
			Repeater {
				model: [
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 3
					qsTr("Open %1 on your %2other device%3.").arg(Qt.application.name).arg("<b>").arg("</b>"),
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 3. %1 and %2 are surrounding tags for bold font.
					qsTr("On that device go to %1Settings%2 and then %1Smartphone as card reader%2 resp. %1Manage pairings%2.").arg("<b>").arg("</b>"),
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 3
					qsTr("Choose this smartphone in the list to pair it.")]

				GText {
					Accessible.name: ApplicationModel.stripHtmlTags(text)
					Layout.alignment: Qt.AlignTop
					text: (index + 1) + ". " + modelData
				}
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RemoteServiceWifiInfo {
		Layout.fillWidth: true
	}
}
