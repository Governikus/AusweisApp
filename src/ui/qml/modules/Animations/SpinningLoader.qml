/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Style
import Governikus.Type

Item {
	id: root

	implicitHeight: Style.dimens.progress_bar_height
	implicitWidth: Style.dimens.progress_bar_height

	Repeater {
		id: repeater

		property int indexOffset: 0

		model: 8

		delegate: Rectangle {
			required property int index
			readonly property double phiIncrement: 2 * Math.PI / repeater.count
			readonly property double positionRadius: root.width / 2 - height / 2
			readonly property double symbolSize: root.height / 5

			color: {
				if (UiPluginModel.highContrastEnabled) {
					return Style.color.image;
				}
				let localIndex = (repeater.count - index + repeater.indexOffset) % repeater.count;
				let factor = 1 + (localIndex / repeater.count) * 1.5;  // factor = 1 results in the original color
				return UiPluginModel.darkModeEnabled ? Qt.darker(Style.color.image, factor) : Qt.lighter(Style.color.image, factor);
			}
			height: symbolSize
			radius: 0.5 * height
			width: height
			x: positionRadius * Math.cos(phiIncrement * index) + root.width / 2 - width / 2
			y: positionRadius * Math.sin(phiIncrement * index) + root.height / 2 - height / 2
		}
	}
	Timer {
		interval: Style.animation_duration / 2
		repeat: true
		running: SettingsModel.useAnimations && !UiPluginModel.highContrastEnabled

		onTriggered: repeater.indexOffset = (repeater.indexOffset + 1) % repeater.count
	}
}
