/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import Governikus.Style

Item {
	id: root

	property bool desaturate: false
	property alias playAnimation: animation.enabled
	property alias source: image.source
	property alias sourceSize: image.sourceSize
	property color tintColor: Style.color.textNormal.basic
	property bool tintEnabled: true

	implicitHeight: image.implicitHeight
	implicitWidth: image.implicitWidth

	Behavior on source {
		id: animation

		enabled: false

		SequentialAnimation {
			PropertyAnimation {
				duration: root.source == "" ? 0 : Style.animation_duration
				easing.type: Easing.InCubic
				property: "opacity"
				target: root
				to: 0
			}
			PropertyAction {
				property: "source"
				target: root
			}
			PropertyAnimation {
				duration: root.source == "" ? Style.animation_duration * 2 : Style.animation_duration
				easing.type: root.source == "" ? Easing.InOutCubic : Easing.InCubic
				property: "opacity"
				target: root
				to: 1
			}
		}
	}

	Image {
		id: image

		anchors.fill: parent
		fillMode: Image.PreserveAspectFit
		visible: !root.tintEnabled || GraphicsInfo.api !== GraphicsInfo.Software

		layer {
			effect: root.desaturate ? desaturateLayer : colorLayer
			enabled: root.tintEnabled
		}
	}
	Button {
		anchors.fill: parent
		background: null
		enabled: false
		icon.color: root.tintColor
		icon.height: root.height
		icon.source: root.source
		icon.width: root.width
		padding: 0
		visible: !image.visible
	}
	Component {
		id: desaturateLayer

		GDesaturate {
		}
	}
	Component {
		id: colorLayer

		GColorOverlay {
			colorizationColor: root.tintColor
		}
	}
}
