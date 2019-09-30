/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: control

	property var textStyle: enabled ? Style.text.button : Style.text.button_disabled
	property color buttonColor: Style.color.accent
	property alias cursorShape: mouseArea.cursorShape

	padding: Constants.pane_padding / 2

	contentItem: RowLayout {
		spacing: Constants.groupbox_spacing

		Image {
			visible: source.toString().length > 0
			sourceSize.height: ApplicationModel.scaleFactor * 40

			source: control.icon.source
		}

		Item {
			id: textContainer

			Layout.fillWidth: true
			implicitHeight: originalSize.implicitHeight
			implicitWidth: originalSize.implicitWidth

			GText {
				id: originalSize

				visible: false
				anchors.fill: parent

				text: control.text
				textStyle: control.textStyle
			}

			GText {
				anchors.fill: parent

				text: control.text
				textStyle: control.textStyle
				font.pixelSize: textStyle.textSize - ApplicationModel.scaleFactor * (control.down ? 1 : 0)
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter

				FocusFrame {
					scope: control
				}
			}
		}
	}

	background: Rectangle {
		color: enabled ? buttonColor : Style.color.accent_disabled
		radius: Style.dimens.button_radius
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onPressed: mouse.accepted = false
		cursorShape: Qt.PointingHandCursor
	}
}
