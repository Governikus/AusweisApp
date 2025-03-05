/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

Column {
	id: root

	property alias chat: repeater.model
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

		color: root.writeAccess ? Style.color.textNormal.basic : textStyle.textColor
		height: implicitHeight * 1.5
		topPadding: Style.dimens.pane_padding
		verticalAlignment: Text.AlignTop
		width: parent.width

		anchors {
			left: parent.left
			leftMargin: Style.dimens.pane_padding
			right: parent.right
			rightMargin: Style.dimens.pane_padding
		}
	}
	ColumnLayout {
		id: grid

		spacing: 0
		width: parent.width

		Repeater {
			id: repeater

			visible: repeater.count > 0

			Loader {
				id: delegateLoader

				required property int index
				readonly property bool isLast: index === repeater.count - 1
				required property var model
				required property string name
				required property bool optional
				required property bool selected

				Layout.fillWidth: true
				sourceComponent: delegateLoader.optional ? optionalDelegate : requiredDelegate

				onFocusChanged: item.focus = focus // QTBUG-122734

				GSeparator {
					anchors.left: parent.left
					visible: !delegateLoader.isLast

					anchors {
						leftMargin: Style.dimens.pane_padding
						right: parent.right
						rightMargin: Style.dimens.pane_padding
						top: parent.bottom
					}
				}
				Component {
					id: optionalDelegate

					GCheckBox {
						id: checkBox

						//: LABEL ANDROID IOS
						Accessible.name: qsTr("%1, optional right, element %2 of %3").arg(text).arg(delegateLoader.index + 1).arg(repeater.count)
						checked: delegateLoader.selected
						horizontalPadding: Style.dimens.pane_padding
						layoutDirection: Qt.RightToLeft
						text: delegateLoader.name
						verticalPadding: Style.dimens.text_spacing

						background: RoundedRectangle {
							bottomLeftCorner: delegateLoader.isLast
							bottomRightCorner: delegateLoader.isLast
							color: checkBox.preferredBackgroundColor
							topLeftCorner: false
							topRightCorner: false
						}

						onCheckedChanged: delegateLoader.model.selected = checked
					}
				}
				Component {
					id: requiredDelegate

					GText {
						//: LABEL ANDROID IOS
						Accessible.name: qsTr("%1, required right, element %2 of %3").arg(text).arg(delegateLoader.index + 1).arg(repeater.count)
						bottomPadding: Style.dimens.text_spacing
						leftPadding: Style.dimens.pane_padding
						rightPadding: Style.dimens.pane_padding
						text: delegateLoader.name
						topPadding: Style.dimens.text_spacing

						onFocusChanged: if (focus)
							Utils.positionViewAtItem(this)
					}
				}
			}
		}
	}
}
