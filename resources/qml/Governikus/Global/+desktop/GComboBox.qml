/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

ComboBox {
	id: control

	property var textStyle: Style.text.normal_inverse

	spacing: Constants.groupbox_spacing
	font.pixelSize: textStyle.fontSize

	indicator: Image {
		visible: control.model.length > 1 || control.model.count > 1
		x: control.width - width - control.rightPadding
		y: control.topPadding + (control.availableHeight - height) / 2

		source: "qrc:///images/triangle.svg"
		sourceSize.width: ApplicationModel.scaleFactor * 20
		sourceSize.height: ApplicationModel.scaleFactor * 20
	}

	contentItem: GText {
		padding: control.spacing
		rightPadding: control.indicator.width + control.spacing
		text: control.displayText
		textStyle: control.textStyle
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	delegate: ItemDelegate {
		width: control.width
		highlighted: control.highlightedIndex === index

		GText {
			anchors.verticalCenter: parent.verticalCenter
			leftPadding: ApplicationModel.scaleFactor * 5

			text: modelData
			textStyle: control.textStyle
			font.bold: control.currentIndex === index
		}
	}

	FocusFrame {
		scope: control
		marginFactor: 1.5
	}

	background: Rectangle {
		border.color: Constants.black
	}
}
