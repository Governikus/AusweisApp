/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

Item {
	id: root

	signal unpairDevice(string pDeviceId)
	signal pairDevice(string pDeviceId)

	property int iconHeight: ApplicationModel.scaleFactor * 80

	implicitHeight: rowLayout.implicitHeight
	activeFocusOnTab: true

	FocusFrame {
		dynamic: false
		border.color: Constants.black
	}

	RowLayout {
		id: rowLayout

		width: parent.width

		spacing: 0

		ColumnLayout {
			Layout.fillHeight: true

			GText {
				Layout.preferredWidth: implicitWidth
				textStyle: Style.text.header_inverse
				text: remoteDeviceName
			}

			GText {
				Layout.preferredWidth: implicitWidth
				textStyle: Style.text.normal_inverse
				text: {
					if (!isPaired) {
						return qsTr("Click to pair") + SettingsModel.translationTrigger
					}

					return remoteDeviceStatus
				}
			}
		}

		Row {
			Layout.preferredHeight: iconHeight
			Layout.alignment: Qt.AlignRight

			spacing: Constants.component_spacing

			Image {
				source: {
					if (!isNetworkVisible && isPaired) {
						return "qrc:///images/icon_remote_inactive.svg"
					}

					if (linkQualityInPercent < 25) {
						return "qrc:///images/icon_remote_0.svg"
					}
					if (linkQualityInPercent < 50) {
						return "qrc:///images/icon_remote_25.svg"
					}
					if (linkQualityInPercent < 75) {
						return "qrc:///images/icon_remote_50.svg"
					}
					if (linkQualityInPercent < 100) {
						return "qrc:///images/icon_remote_75.svg"
					}

					return "qrc:///images/icon_remote_100.svg"
				}

				sourceSize.height: iconHeight
				fillMode: Image.PreserveAspectFit
			}

			Image {
				id: removeIcon

				visible: isPaired

				source: "qrc:///images/trash_icon.svg"
				sourceSize.height: iconHeight
				fillMode: Image.PreserveAspectFit

				MouseArea {
					anchors.fill: parent
					onClicked: unpairDevice(deviceId)
					cursorShape: Qt.PointingHandCursor
				}
			}

			Item {
				id: spacer

				visible: !removeIcon.visible

				height: removeIcon.height
				width: removeIcon.width
			}
		}
	}

	MouseArea {
		anchors.fill: parent

		visible: !isPaired
		onClicked: pairDevice(deviceId)
		cursorShape: Qt.PointingHandCursor
	}
}
