/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

ProgressBar {
	id: root

	readonly property alias effectiveVisualPosition: bar.mutableVisualPosition
	property string text: ""

	Accessible.focusable: true

	//: ALL_PLATFORMS
	Accessible.name: qsTr("Progress")
	Accessible.role: Accessible.ProgressBar
	background: null
	from: 0
	to: 100

	contentItem: Rectangle {
		Accessible.ignored: true
		border.color: Style.color.control.border.basic_unchecked
		border.width: 2 * Style.dimens.border_width
		color: Style.color.background
		implicitHeight: 2 * (progressText.font.pixelSize + border.width)
		implicitWidth: Math.max(10 * implicitHeight, progressText.implicitWidth + implicitHeight)
		radius: height / 2

		GText {
			id: progressText

			Accessible.ignored: true
			anchors.centerIn: parent
			elide: Text.ElideMiddle
			font.weight: Style.font.bold
			maximumLineCount: 1
			text: root.text
		}
		Item {
			id: barSpace

			anchors.fill: parent
			anchors.margins: 3 * parent.border.width

			Rectangle {
				id: bar

				property real mutableVisualPosition: SettingsModel.useAnimations || root.visualPosition === 1 ? root.visualPosition : 0.25

				border.color: Style.color.control.border.basic_unchecked
				border.width: Style.dimens.border_width
				clip: true
				color: Style.color.control.background.basic_unchecked
				height: parent.height
				radius: height / 2
				width: parent.width * mutableVisualPosition

				Behavior on mutableVisualPosition {
					enabled: SettingsModel.useAnimations

					SmoothedAnimation {
						velocity: 0.5
					}
				}

				GText {
					Accessible.ignored: true
					color: Style.color.control.content.basic_unchecked
					elide: Text.ElideMiddle
					font.weight: Style.font.bold
					maximumLineCount: 1
					text: root.text

					anchors {
						left: parent.left
						leftMargin: progressText.x - barSpace.anchors.margins
						verticalCenter: parent.verticalCenter
					}
				}
			}
		}
	}

	FocusFrame {
	}
}
