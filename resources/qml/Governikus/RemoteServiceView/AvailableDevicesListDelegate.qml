/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


MouseArea {
	signal requestPairing(string pDeviceId)

	height: textColumn.height + separator.height + (Constants.groupbox_spacing / 2)

	Accessible.role: Accessible.ListItem
	Accessible.name: qsTr("Device %1 is available for pairing").arg(nameText.text) + SettingsModel.translationTrigger
	Accessible.onPressAction: if (Qt.platform.os === "ios" && isSupported) clicked(null)

	onClicked: if (isSupported) requestPairing(deviceId)

	LinkQuality {
		id: linkQual

		height: nameText.height
		width: height
		anchors.verticalCenter: parent.verticalCenter

		percent: linkQualityInPercent
	}

	Column {
		id: textColumn

		width: parent.width - linkQual.width - anchors.leftMargin
		anchors.left: linkQual.right
		anchors.leftMargin: Constants.component_spacing

		spacing: 2
		topPadding: spacing
		bottomPadding: spacing

		GText {
			id: nameText

			width: parent.width

			text: {
				SettingsModel.translationTrigger

				if (isSupported) {
					return remoteDeviceName;
				}
				return remoteDeviceName + " (" + remoteDeviceStatus + ")"
			}
			textStyle: Style.text.normal_secondary
		}

		GText {
			id: dateText

			width: parent.width

			//: LABEL ANDROID IOS
			text: (linkQualityInPercent >= 50 ? qsTr("Great quality") : qsTr("Bad quality")) + SettingsModel.translationTrigger
			textStyle: Style.text.hint_secondary
		}
	}

	GSeparator {
		id: separator

		width: parent.width
		anchors.bottom: parent.bottom
		color: Style.color.border_dark
	}
}
