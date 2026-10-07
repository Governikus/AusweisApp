/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	//: ALL_PLATFORMS
	readonly property string a11yDescription: qsTr("Show more information about the service provider")
	readonly property string a11yName: subheading.text + ". " + nameText.text
	property alias name: nameText.text

	Accessible.description: Utils.resolveA11yDescription(a11yName, a11yDescription)
	Accessible.name: Utils.resolveA11yName(a11yName, a11yDescription)
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
