/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View

MouseArea {
	id: root

	property alias description: descriptionText.text
	required property var deviceId
	required property int index
	required property bool isLastAddedDevice
	required property bool isNetworkVisible
	required property bool isPaired
	required property bool isSupported
	required property int linkQualityInPercent
	property alias linkQualityVisible: linkQualityItem.visible
	required property string remoteDeviceName
	required property string remoteDeviceStatus
	property alias titleColor: titleText.color

	signal activate(var pIsSupported, var pDeviceId)

	Accessible.name: qsTr("Device %1. %2.").arg(titleText.text).arg(description)
	Accessible.role: Accessible.ListItem
	activeFocusOnTab: true
	implicitHeight: content.implicitHeight
	implicitWidth: content.implicitWidth

	Keys.onEnterPressed: clicked(null)
	Keys.onReturnPressed: clicked(null)
	Keys.onSpacePressed: clicked(null)
	onClicked: activate(isSupported, deviceId)

	FocusFrame {
		marginFactor: 3
	}
	ColumnLayout {
		id: content

		anchors.fill: parent
		spacing: Style.dimens.text_spacing

		GSeparator {
			Layout.fillWidth: true
			visible: root.index > 0
		}
		RowLayout {
			spacing: Style.dimens.groupbox_spacing

			ColumnLayout {
				Layout.fillWidth: true
				spacing: 2

				GText {
					id: titleText

					Accessible.ignored: true
					elide: Text.ElideRight
					font.weight: root.isLastAddedDevice ? Style.font.bold : Style.font.normal
					maximumLineCount: 1
					text: root.remoteDeviceName + (root.isSupported ? "" : (" (" + root.remoteDeviceStatus + ")"))
					textFormat: Text.PlainText
					textStyle: Style.text.subline
				}
				GText {
					id: descriptionText

					Accessible.ignored: true
					elide: Text.ElideRight
					maximumLineCount: 1
					visible: text !== ""
				}
			}
			GSpacer {
				Layout.fillWidth: true
			}
			LinkQualityAnimation {
				id: linkQualityItem

				inactive: !root.isNetworkVisible && root.isPaired
				percent: root.linkQualityInPercent
			}
		}
	}
}
