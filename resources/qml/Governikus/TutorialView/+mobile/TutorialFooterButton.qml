/*
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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

	signal clicked

	Accessible.name: contentText.text
	Accessible.role: Accessible.Button
	height: contentRow.height + 2 * Constants.component_spacing
	implicitWidth: contentImage.width + Math.ceil(contentText.implicitWidth) + Constants.component_spacing

	Accessible.onPressAction: clicked()

	MouseArea {
		anchors.fill: contentRow
		preventStealing: true

		onClicked: root.clicked()
	}
	RowLayout {
		id: contentRow
		height: contentText.height
		spacing: Constants.component_spacing
		width: parent.width

		anchors {
			left: parent.left
			right: parent.right
			verticalCenter: parent.verticalCenter
		}
		Image {
			id: contentImage
			Layout.alignment: Qt.AlignVCenter
			Layout.preferredHeight: height
			Layout.preferredWidth: width
			fillMode: Image.PreserveAspectFit
			height: contentText.height
			width: height * (sourceSize.width / sourceSize.height)
		}
		GText {
			id: contentText
			Accessible.ignored: true
			Layout.alignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Math.ceil(implicitWidth)
			elide: Text.ElideRight
			textStyle: Style.text.normal_inverse
			wrapMode: Text.NoWrap
		}
	}
}
