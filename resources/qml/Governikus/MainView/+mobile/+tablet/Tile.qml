/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

GPaneBackground {
	id: root

	property alias title: text.text
	property alias image: image.source

	signal clicked()

	Accessible.role: Accessible.Button
	Accessible.name: title
	Accessible.onPressAction: clicked()

	color: Qt.darker(Style.color.accent, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)

	MouseArea {
		id: mouseArea

		anchors.fill: root

		onClicked: root.clicked()
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_spacing

		spacing: Constants.text_spacing

		GSpacer { Layout.fillHeight: true }

		TintableIcon {
			id: image

			Layout.preferredHeight: Style.dimens.huge_icon_size
			Layout.maximumHeight: Style.dimens.huge_icon_size
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter

			Accessible.ignored: true

			sourceSize.height: Style.dimens.huge_icon_size
			tintColor: text.color
		}

		GText {
			id: text

			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.minimumHeight: lineHeight

			Accessible.ignored: true

			elide: Text.ElideRight
			textStyle: Style.text.title_inverse
			color: Qt.darker(textStyle.textColor, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.bold: true
		}

		GSpacer { Layout.fillHeight: true }
	}
}
