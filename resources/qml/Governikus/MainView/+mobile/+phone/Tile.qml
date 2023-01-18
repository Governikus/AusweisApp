/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

GPaneBackground {
	id: root

	property alias image: image.source
	property alias title: text.text

	signal clicked

	Accessible.name: title
	Accessible.role: Accessible.Button
	color: Qt.darker(Style.color.accent, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)

	Accessible.onPressAction: clicked()

	MouseArea {
		id: mouseArea
		anchors.fill: root

		onClicked: root.clicked()
	}
	TintableIcon {
		id: image
		Accessible.ignored: true
		height: root.height - Constants.pane_padding
		sourceSize.height: Style.dimens.large_icon_size
		tintColor: text.color

		anchors {
			left: root.left
			leftMargin: Constants.component_spacing
			verticalCenter: root.verticalCenter
		}
	}
	GText {
		id: text
		Accessible.ignored: true
		color: Qt.darker(textStyle.textColor, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)
		elide: Text.ElideRight
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.title_inverse
		verticalAlignment: Text.AlignVCenter

		anchors {
			bottom: root.bottom
			left: image.right
			leftMargin: Constants.text_spacing
			right: root.right
			rightMargin: Constants.text_spacing
			top: root.top
		}
	}
}
