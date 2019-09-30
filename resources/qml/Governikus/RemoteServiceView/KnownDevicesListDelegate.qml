/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


Item {
	id: root

	height: textColumn.height + separator.height + (Constants.groupbox_spacing / 2)

	Accessible.role: Accessible.ListItem
	Accessible.name: qsTr("Device %1 has status %2").arg(nameText.text).arg(statusText.text) + SettingsModel.translationTrigger
	Accessible.onPressAction: if (Qt.platform === "ios") iconClick.clicked(null)

	LinkQuality {
		id: linkQual

		height: nameText.height
		anchors.verticalCenter: textColumn.verticalCenter

		percent: linkQualityInPercent
		inactive: !isNetworkVisible
	}

	Column {
		id: textColumn

		width: parent.width - linkQual.width - iconClick.width - 2 * anchors.leftMargin
		anchors.left: linkQual.right
		anchors.right: deleteIcon.left
		anchors.margins: Constants.component_spacing

		spacing: 2
		topPadding: spacing
		bottomPadding: spacing

		GText {
			id: nameText

			width: parent.width

			text: remoteDeviceName
			textStyle: Style.text.normal_secondary
		}

		GText {
			id: statusText

			width: parent.width

			text: {
				SettingsModel.translationTrigger

				if (!isNetworkVisible) {
					//: LABEL ANDROID IOS
					return qsTr("Not available");
				}
				//: LABEL ANDROID IOS
				return remoteDeviceStatus + ", " + (linkQualityInPercent >= 50 ? qsTr("Great quality") : qsTr("Bad quality")) + SettingsModel.translationTrigger
			}
			textStyle: Style.text.hint_secondary
		}

		GText {
			width: parent.width

			//: LABEL ANDROID IOS
			text: qsTr("Last connection:") + " " + lastConnected + SettingsModel.translationTrigger
			textStyle: Style.text.hint_secondary
		}
	}

	Image {
		id: deleteIcon

		sourceSize.width: Style.dimens.small_icon_size
		anchors.right: root.right
		anchors.verticalCenter: root.verticalCenter
		source: "qrc:///images/iOS/search_cancel.svg"

		MouseArea {
			id: iconClick

			anchors.fill: parent

			onClicked: RemoteServiceModel.forgetDevice(deviceId)
		}
	}

	GSeparator {
		id: separator

		width: parent.width
		anchors.bottom: root.bottom
		color: Style.color.border_dark
	}
}
