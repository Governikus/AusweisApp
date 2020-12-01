/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

ComboBox {
	id: control

	property TextStyle textStyle: Style.text.normal

	Accessible.role: Accessible.ComboBox
	Accessible.name: displayText

	spacing: Constants.groupbox_spacing
	font.pixelSize: textStyle.fontSize

	popup.topMargin: plugin.safeAreaMargins.top
	popup.bottomMargin: plugin.safeAreaMargins.bottom

	indicator: TintableIcon {
		visible: control.count > 1

		width: height
		height: Math.round(control.height / 4)
		x: Math.round(control.width - width - control.rightPadding)
		y: Math.round(control.topPadding + (control.availableHeight - height) / 2)

		source: "qrc:///images/triangle.svg"
		tintColor: control.textStyle.textColor
	}

	contentItem: GText {
		padding: control.spacing
		rightPadding: control.indicator.width + control.spacing
		text: control.displayText
		textStyle: control.textStyle
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
		maximumLineCount: 1
	}

	delegate: ItemDelegate {
		width: control.width
		contentItem: GText {
			text: modelData
			textStyle: control.textStyle
			elide: Text.ElideRight
			verticalAlignment: Text.AlignVCenter
		}
		highlighted: control.highlightedIndex === index

		background: Rectangle {
			implicitWidth: 100
			implicitHeight: Style.dimens.list_item_height
			color: highlighted ? Style.color.background_pane_active : Style.color.background_pane

			GSeparator {
				anchors {
					left: parent.left
					right: parent.right
					bottom: parent.bottom
				}
			}
		}
	}

	background: Rectangle {
		border.color: Style.color.border
		border.width: control.visualFocus ? 2 : 1
		radius: Style.dimens.separator_size
		color: Style.color.background_pane
	}

	FocusFrame {
		scope: control
		marginFactor: 1.5
	}
}
