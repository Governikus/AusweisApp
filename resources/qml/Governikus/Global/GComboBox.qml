/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

ComboBox {
	id: control

	property TextStyle textStyle: Style.text.normal

	Accessible.name: displayText
	Accessible.role: Accessible.ComboBox
	font.pixelSize: textStyle.fontSize
	popup.bottomMargin: plugin.safeAreaMargins.bottom
	popup.leftMargin: plugin.safeAreaMargins.left
	popup.rightMargin: plugin.safeAreaMargins.right
	popup.topMargin: plugin.safeAreaMargins.top
	spacing: Constants.groupbox_spacing

	background: Rectangle {
		border.color: Style.color.border
		border.width: control.visualFocus ? 2 : 1
		color: Style.color.background_pane
		radius: Style.dimens.separator_size
	}
	contentItem: GText {
		elide: Text.ElideRight
		maximumLineCount: 1
		padding: control.spacing
		rightPadding: control.indicator.width + control.spacing
		text: control.displayText
		textStyle: control.textStyle
		verticalAlignment: Text.AlignVCenter
	}
	delegate: ItemDelegate {
		highlighted: control.highlightedIndex === index
		width: control.width

		background: Rectangle {
			color: highlighted ? Style.color.background_pane_active : Style.color.background_pane
			implicitHeight: Style.dimens.list_item_height
			implicitWidth: 100

			GSeparator {
				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}
			}
		}
		contentItem: GText {
			elide: Text.ElideRight
			text: modelData
			textStyle: control.textStyle
			verticalAlignment: Text.AlignVCenter
		}
	}
	indicator: TintableIcon {
		height: Math.round(control.height / 4)
		source: "qrc:///images/triangle.svg"
		tintColor: control.textStyle.textColor
		visible: control.count > 1
		width: height
		x: Math.round(control.width - width - control.rightPadding)
		y: Math.round(control.topPadding + (control.availableHeight - height) / 2)
	}

	FocusFrame {
		marginFactor: 1.5
		scope: control
	}
}
