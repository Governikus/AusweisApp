/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

ColumnLayout {
	id: root

	property alias chat: repeater.model
	property alias columns: grid.columns
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
	property bool writeAccess: false

	Accessible.focusable: true
	Accessible.name: title
	Accessible.role: Accessible.Grouping
	spacing: Style.dimens.pane_spacing
	visible: count > 0

	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	GText {
		id: dataTitle

		Accessible.ignored: true
		color: root.writeAccess ? Style.color.textNormal.basic : textStyle.textColor
		textStyle: Style.text.headline

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
	Grid {
		id: grid

		Layout.fillWidth: true
		Layout.preferredWidth: repeater.maxItemWidth * columns + grid.columnSpacing * (columns - 1)
		columnSpacing: Style.dimens.pane_spacing
		flow: Grid.TopToBottom
		verticalItemAlignment: Grid.AlignBottom

		GRepeater {
			id: repeater

			visible: count > 0

			Item {
				id: delegateItem

				required property int index
				readonly property bool isLast: index === repeater.count - 1
				required property var model
				required property string name
				required property bool optional
				required property bool selected

				height: fontMetrics.height * 2
				width: (grid.width - (grid.columnSpacing * (grid.columns - 1))) / grid.columns

				GSeparator {
					anchors.bottom: parent.bottom
					visible: !(delegateItem.isLast || ((delegateItem.index + 1) % Math.ceil(repeater.count / grid.columns)) === 0)
					width: parent.width
				}
				FontMetrics {
					id: fontMetrics

					font.pixelSize: Style.dimens.text
				}
				GText {
					//: LABEL DESKTOP
					Accessible.name: qsTr("%1, required right, element %2 of %3").arg(text).arg(delegateItem.index + 1).arg(repeater.count)
					anchors.fill: parent
					focusFrameVisible: false
					leftPadding: Style.dimens.text_spacing
					rightPadding: Style.dimens.text_spacing
					text: delegateItem.name
					visible: !delegateItem.optional

					onFocusChanged: if (focus)
						Utils.positionViewAtItem(this)

					FocusFrame {
						anchors {
							bottomMargin: Style.dimens.separator_size * 2
							leftMargin: 0
							rightMargin: 0
							topMargin: Style.dimens.separator_size
						}
					}
				}
				GCheckBox {
					//: LABEL DESKTOP
					Accessible.name: qsTr("%1, optional right, element %2 of %3").arg(text).arg(delegateItem.index + 1).arg(repeater.count)
					anchors.fill: parent
					checked: delegateItem.selected
					focusFrameVisible: false
					horizontalPadding: Style.dimens.text_spacing
					layoutDirection: Qt.RightToLeft
					text: delegateItem.name
					verticalPadding: 0
					visible: delegateItem.optional

					onCheckedChanged: delegateItem.model.selected = checked

					FocusFrame {
						anchors {
							bottomMargin: Style.dimens.separator_size * 2
							leftMargin: 0
							rightMargin: 0
							topMargin: Style.dimens.separator_size
						}
					}
				}
			}
		}
	}
}
