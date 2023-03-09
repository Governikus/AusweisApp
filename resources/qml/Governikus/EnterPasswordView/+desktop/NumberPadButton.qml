/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Button {
	id: control

	property alias color: circle.color
	property real fontScale: 1
	property bool visualPrivacy: false

	height: (1.5 * contentItem.font.pixelSize) / fontScale
	padding: 0
	width: height

	background: Rectangle {
		id: circle
		border.color: Style.text.header.textColor
		border.width: !visualPrivacy && control.focus ? Math.max(1, ApplicationModel.scaleFactor * 2) : 0
		color: Style.color.accent
		radius: control.height / 2
		visible: control.enabled

		Rectangle {
			anchors.centerIn: parent
			color: Constants.black
			opacity: 0.1
			radius: height / 2

			SequentialAnimation on height  {
				alwaysRunToEnd: true
				running: !visualPrivacy && control.pressed

				NumberAnimation {
					from: 0
					to: background.height
				}
				PauseAnimation {
					duration: 100
				}
				PropertyAction {
					value: 0
				}
			}
			SequentialAnimation on width  {
				alwaysRunToEnd: true
				running: !visualPrivacy && control.pressed

				NumberAnimation {
					from: 0
					to: background.width
				}
				PauseAnimation {
					duration: 100
				}
				PropertyAction {
					value: 0
				}
			}
		}
	}
	contentItem: GText {
		font.pixelSize: fontScale * ApplicationModel.scaleFactor * 50
		horizontalAlignment: Text.AlignHCenter
		text: control.text
		textStyle: Style.text.header_inverse
		verticalAlignment: Text.AlignVCenter
		visible: control.enabled
	}

	FocusFrame {
	}
}
