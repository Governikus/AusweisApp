/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
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
				duration: source == "" ? 0 : Constants.animation_duration
				easing.type: Easing.InCubic
				property: "opacity"
				targets: root
				to: 0
			}
			PropertyAction {
				property: "source"
				target: root
			}
			PropertyAnimation {
				duration: source == "" ? Constants.animation_duration * 2 : Constants.animation_duration
				easing.type: source == "" ? Easing.InOutCubic : Easing.InCubic
				property: "opacity"
				targets: root
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
			enabled: root.tintEnabled

			effect: ShaderEffect {
				property color color: root.tintColor

				fragmentShader: root.desaturate ? "qrc:/shader/DesaturateShader.frag" : "qrc:/shader/ColorOverlayShader.frag"
			}
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
}
