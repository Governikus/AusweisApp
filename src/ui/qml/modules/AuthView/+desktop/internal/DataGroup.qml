/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

ColumnLayout {
	id: root

	property alias chat: repeater.model
	property alias columns: grid.columns
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
	property bool writeAccess: false

	spacing: Constants.pane_spacing
	visible: count > 0

	GText {
		id: dataTitle

		Accessible.name: dataTitle.text
		activeFocusOnTab: true
		color: writeAccess ? Style.color.textNormal.basic : titleStyle.textColor
		textStyle: Style.text.headline

		FocusFrame {
		}
	}
	Grid {
		id: grid

		Layout.fillWidth: true
		Layout.preferredWidth: repeater.maxItemWidth * columns + grid.columnSpacing * (columns - 1)
		columnSpacing: Constants.pane_spacing
		flow: Grid.TopToBottom
		verticalItemAlignment: Grid.AlignBottom

		GRepeater {
			id: repeater

			visible: count > 0

			Loader {
				id: rightItem

				property bool isLast: index === repeater.count - 1
				property int modelIndex: index
				property string modelName: name
				property bool modelSelected: selected
				property bool modelWriteRight: writeRight

				function updateModel(checked) {
					selected = checked;
				}

				height: fontMetrics.height * 2
				sourceComponent: optional ? optionalDelegate : requiredDelegate
				width: (grid.width - (grid.columnSpacing * (grid.columns - 1))) / grid.columns

				Keys.onSpacePressed: if (optional)
					selected = !selected

				FocusFrame {
					anchors {
						bottomMargin: Style.dimens.separator_size * 2
						leftMargin: 0
						rightMargin: 0
						topMargin: Style.dimens.separator_size
					}
				}
				GSeparator {
					anchors.bottom: parent.bottom
					visible: !(isLast || ((index + 1) % Math.ceil(repeater.count / grid.columns)) === 0)
					width: parent.width
				}
				FontMetrics {
					id: fontMetrics

					font.pixelSize: Style.dimens.text
				}
			}
		}
	}
	Component {
		id: optionalDelegate

		GCheckBox {
			id: checkBox

			//: LABEL DESKTOP
			Accessible.name: qsTr("%1, optional right, element %2 of %3").arg(text).arg(modelIndex + 1).arg(repeater.count)
			checked: modelSelected
			focusFrameVisible: false
			horizontalPadding: Constants.text_spacing
			layoutDirection: Qt.RightToLeft
			text: modelName
			verticalPadding: 0

			onCheckedChanged: updateModel(checked)
		}
	}
	Component {
		id: requiredDelegate

		GText {
			id: dataText

			//: LABEL DESKTOP
			Accessible.name: qsTr("%1, required right, element %2 of %3").arg(text).arg(modelIndex + 1).arg(repeater.count)
			activeFocusOnTab: true
			leftPadding: Constants.text_spacing
			rightPadding: Constants.text_spacing
			text: modelName
		}
	}
}
