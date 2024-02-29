/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

Column {
	id: column

	property var chat
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
	property bool writeAccess: false

	signal scrollPageDown
	signal scrollPageUp

	visible: repeater.count > 0
	width: parent.width

	PaneTitle {
		id: dataTitle

		color: writeAccess ? Style.color.warning : titleStyle.textColor
		height: implicitHeight * 1.5
		topPadding: Constants.pane_padding
		verticalAlignment: Text.AlignTop
		width: parent.width

		anchors {
			left: parent.left
			leftMargin: Constants.pane_padding
			right: parent.right
			rightMargin: Constants.pane_padding
		}
	}
	ColumnLayout {
		id: grid

		spacing: 0
		width: parent.width

		Repeater {
			id: repeater

			model: chat
			visible: repeater.count > 0

			Loader {
				id: delegateLoader

				property bool isLast: index === repeater.count - 1
				property string modelName: name
				property bool modelSelected: selected
				property bool modelWriteRight: writeRight

				function updateModel(checked) {
					selected = checked;
				}

				Layout.fillWidth: true
				sourceComponent: optional ? optionalDelegate : requiredDelegate

				GSeparator {
					anchors.left: parent.left
					visible: !isLast

					anchors {
						leftMargin: Constants.pane_padding
						right: parent.right
						rightMargin: Constants.pane_padding
						top: parent.bottom
					}
				}
			}
		}
	}
	Component {
		id: optionalDelegate

		GCheckBox {
			id: checkBox

			checked: modelSelected
			horizontalPadding: Constants.pane_padding
			layoutDirection: Qt.RightToLeft
			text: modelName
			textStyle: modelWriteRight ? Style.text.normal_warning : Style.text.normal
			verticalPadding: Constants.text_spacing

			background: RoundedRectangle {
				bottomLeftCorner: isLast
				bottomRightCorner: isLast
				color: checkBox.preferredBackgroundColor
				topLeftCorner: false
				topRightCorner: false
			}

			onCheckedChanged: updateModel(checked)
		}
	}
	Component {
		id: requiredDelegate

		GText {
			bottomPadding: Constants.text_spacing
			leftPadding: Constants.pane_padding
			rightPadding: Constants.pane_padding
			text: modelName
			textStyle: modelWriteRight ? Style.text.normal_warning : Style.text.normal
			topPadding: Constants.text_spacing
		}
	}
}
