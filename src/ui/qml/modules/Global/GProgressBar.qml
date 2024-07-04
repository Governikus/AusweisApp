/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

ProgressBar {
	id: progressBar

	readonly property alias effectiveVisualPosition: bar.mutableVisualPosition
	property alias text: progressText.text

	Accessible.name: qsTr("%1 percent done").arg(value)
	Accessible.role: Accessible.ProgressBar
	background: null
	from: 0
	to: 100

	contentItem: Rectangle {
		border.color: Style.color.control.border.basic
		border.width: Style.dimens.progress_bar_border
		color: Style.color.background
		implicitHeight: Style.dimens.progress_bar_height
		radius: height / 2

		Item {
			anchors.fill: parent
			anchors.margins: Style.dimens.progress_bar_border * 3

			Rectangle {
				id: bar

				property real mutableVisualPosition: SettingsModel.useAnimations || visualPosition === 1 ? visualPosition : 0.25

				border.color: Style.color.control.border.basic
				border.width: Style.dimens.border_width
				color: Style.color.control.background.basic
				height: parent.height
				radius: height / 2
				width: parent.width * mutableVisualPosition

				Behavior on mutableVisualPosition {
					enabled: SettingsModel.useAnimations

					SmoothedAnimation {
						velocity: 0.5
					}
				}
			}
		}
		GText {
			id: progressText

			color: Style.color.progressbar_text
			elide: Text.ElideMiddle
			font.weight: Font.Bold
			horizontalAlignment: Text.AlignHCenter
			maximumLineCount: 1

			anchors {
				left: parent.left
				right: parent.right
				verticalCenter: parent.verticalCenter
			}
		}
	}

	FocusFrame {
	}
}
