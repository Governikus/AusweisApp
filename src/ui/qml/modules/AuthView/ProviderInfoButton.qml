/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	property alias name: nameText.text

	//: ALL_PLATFORMS
	Accessible.description: qsTr("Show more information about the service provider")
	Accessible.name: subheading.text + ". " + nameText.text
	Accessible.role: Accessible.Button
	padding: Style.dimens.pane_padding

	background: GPaneBackground {
		Accessible.ignored: true
		border.color: colors.paneBorder
		color: colors.paneBackground

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		spacing: Style.dimens.pane_spacing

		ColumnLayout {
			spacing: Style.dimens.text_spacing

			Subheading {
				id: subheading

				Accessible.ignored: true
				//: ALL_PLATFORMS
				text: qsTr("Service Provider")
			}
			GText {
				id: nameText

				Accessible.ignored: true
				visible: text !== ""
			}
		}
		TintableIcon {
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: nameText.color
		}
	}

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
}
