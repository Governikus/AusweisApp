/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: root

	property alias direction: contentRow.layoutDirection
	property alias imageRotation: contentImage.rotation
	property alias imageSource: contentImage.source
	property alias text: contentText.text

	signal clicked()

	implicitWidth: contentImage.width + Math.ceil(contentText.implicitWidth) + Constants.component_spacing
	height: contentRow.height + 2 * Constants.component_spacing

	Accessible.name: contentText.text
	Accessible.role: Accessible.Button
	Accessible.onPressAction: clicked()

	MouseArea {
		anchors.fill: contentRow
		preventStealing: true
		onClicked: root.clicked()
	}

	RowLayout {
		id: contentRow

		width: parent.width
		height: contentText.height

		spacing: Constants.component_spacing

		anchors {
			verticalCenter: parent.verticalCenter
			left: parent.left
			right: parent.right
		}

		Image {
			id: contentImage

			width: height * (sourceSize.width / sourceSize.height)
			height: contentText.height

			Layout.alignment: Qt.AlignVCenter
			Layout.preferredHeight: height
			Layout.preferredWidth: width

			fillMode: Image.PreserveAspectFit
		}

		GText {
			id: contentText

			Layout.alignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Math.ceil(implicitWidth)

			Accessible.ignored: true

			textStyle: Style.text.normal_inverse
			elide: Text.ElideRight
			wrapMode: Text.NoWrap
		}
	}
}
