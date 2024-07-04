/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Item {
	id: root

	property real angle: 0
	property Gradient gradient: Gradient {
		GradientStop {
			color: "white"
			position: 0.0
		}
		GradientStop {
			color: "black"
			position: 1.0
		}
	}
	property int horizontalOffset: 0
	property var source: Rectangle {
		height: root.height
		width: root.width
	}
	property int verticalOffset: 0

	ShaderEffectSource {
		id: baseSource

		anchors.fill: parent
		smooth: true
		sourceItem: root.source
		visible: false
	}
	ShaderEffect {
		id: colorEffect

		property var center: Qt.point(0.5 + root.horizontalOffset / width, 0.5 + root.verticalOffset / height)
		property var gradientSource: ShaderEffectSource {
			hideSource: true
			smooth: true

			sourceItem: Rectangle {
				gradient: root.gradient
				height: 256
				smooth: true
				width: 16
			}
		}
		property var source: baseSource
		property real startAngle: (root.angle - 90) * Math.PI / 180

		anchors.fill: baseSource
		fragmentShader: "qrc:/shader/ConicalGradientShader.frag"
		smooth: true
	}
}
