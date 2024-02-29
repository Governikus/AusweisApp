/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.SettingsModel

TintableIcon {
	id: root

	readonly property bool animated: SettingsModel.useAnimations && GraphicsInfo.api !== GraphicsInfo.Software

	source: animated ? "qrc:///animations/hourglass_background.svg" : "qrc:///animations/hourglass.svg"
	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	NumberAnimation {
		id: animation

		property real value: 0.0

		duration: 6000
		from: 0.0
		loops: Animation.Infinite
		property: "value"
		running: root.animated
		target: animation
		to: 1.0
	}
	TintableIcon {
		rotation: animation.value * 360
		source: "qrc:///animations/hourglass_arrows.svg"
		sourceSize.height: parent.height * 0.286
		tintColor: root.tintColor
		tintEnabled: root.tintEnabled

		anchors {
			centerIn: parent
			horizontalCenterOffset: -(parent.width * 0.305)
			verticalCenterOffset: -(parent.height * 0.075)
		}
	}
	Item {
		id: topSandContainer

		anchors.fill: topMask
		visible: false

		Image {
			source: "qrc:///animations/hourglass_sand.svg"
			sourceSize.height: parent.height
			y: height * (animation.value * 0.6 + 0.2)
		}
	}
	MultiEffect {
		anchors.fill: topSandContainer
		maskEnabled: true
		maskSource: topMask
		source: topSandContainer
		visible: root.animated
	}
	TintableIcon {
		id: topMask

		source: "qrc:///animations/hourglass_mask_top.svg"
		sourceSize.height: parent.height * 0.366
		tintColor: root.tintColor
		tintEnabled: root.tintEnabled
		visible: false

		anchors {
			centerIn: parent
			horizontalCenterOffset: parent.width * 0.137
			verticalCenterOffset: -(parent.height * 0.2)
		}
	}
	Item {
		id: bottomSandContainer

		anchors.fill: bottomMask
		visible: false

		Image {
			source: "qrc:///animations/hourglass_sand.svg"
			sourceSize.height: parent.height
			y: height - height * (animation.value * 0.6 + 0.2)
		}
	}
	MultiEffect {
		anchors.fill: bottomSandContainer
		maskEnabled: true
		maskSource: bottomMask
		source: bottomSandContainer
		visible: root.animated
	}
	TintableIcon {
		id: bottomMask

		source: "qrc:///animations/hourglass_mask_bottom.svg"
		sourceSize.height: parent.height * 0.366
		tintColor: root.tintColor
		tintEnabled: root.tintEnabled
		visible: false

		anchors {
			centerIn: parent
			horizontalCenterOffset: parent.width * 0.137
			verticalCenterOffset: parent.height * 0.195
		}
	}
}
