/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property alias description: descriptionText.text

	Accessible.name: root.text + ". " + root.description
	horizontalPadding: Constants.component_spacing
	verticalPadding: Constants.component_spacing

	background: GPaneBackground {
		id: pane

		border.color: colors.paneBorder
		color: colors.paneBackground

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		id: rowLayout

		spacing: Constants.component_spacing

		TintableIcon {
			source: root.icon.source
			sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: title.color
		}
		ColumnLayout {
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			spacing: Constants.text_spacing / 2

			GText {
				id: title

				Accessible.ignored: true
				elide: Text.ElideRight
				text: root.text
				textStyle: Style.text.subline
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				elide: Text.ElideRight
			}
		}
		TintableIcon {
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: descriptionText.color
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
