/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

ColumnLayout {
	id: root

	property bool alwaysReserveSelectionTitleHeight: false
	property bool arrowToLeft: false
	property alias backgroundColor: collapsibleContentBackground.color
	property int contentBottomMargin: Constants.groupbox_spacing
	property int contentHorizontalMargin: horizontalMargin
	property alias contentSpacing: contentItem.spacing
	property int contentTopMargin: Constants.groupbox_spacing
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	default property alias expandableData: contentItem.data
	property bool expanded: false
	property int horizontalMargin: Constants.component_spacing
	property alias selectionIcon: selectionIcon.source
	property alias selectionTitle: selectionTitle.text
	property alias tintIcon: selectionIcon.tintEnabled
	property alias title: title.text

	signal receivedFocus(var pItem)

	spacing: 0

	AbstractButton {
		id: expandButton

		Accessible.name: root.title + ". " +
		//: LABEL ANDROID IOS
		(expanded ? qsTr("collapse") :
			//: LABEL ANDROID IOS
			qsTr("expand") + ". ") +
		//: LABEL ANDROID IOS
		(root.selectionTitle !== "" ? qsTr("Currently selected is %1").arg(root.selectionTitle) : "")
		Accessible.role: Accessible.Button
		Layout.fillWidth: true
		implicitHeight: bannerLayout.implicitHeight + Constants.component_spacing * 2
		implicitWidth: bannerLayout.implicitWidth

		background: RoundedRectangle {
			id: background

			bottomLeftCorner: drawBottomCorners && !expanded
			bottomRightCorner: drawBottomCorners && !expanded
			color: colors.paneBackground
			topLeftCorner: drawTopCorners
			topRightCorner: drawTopCorners

			FocusFrame {
				anchors.margins: Constants.component_spacing / 2
				scope: expandButton
			}
		}
		contentItem: RowLayout {
			id: bannerLayout

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: 0

			LeftRightArrow {
				isLeft: root.arrowToLeft
				visible: root.arrowToLeft
			}
			ColumnLayout {
				Layout.leftMargin: root.arrowToLeft ? 0 : root.horizontalMargin
				spacing: Constants.subtext_spacing

				GText {
					id: title

					Accessible.ignored: true
					textStyle: Style.text.subline
					visible: text !== ""
				}
				GText {
					id: selectionTitle

					Accessible.ignored: true
					visible: alwaysReserveSelectionTitleHeight || text !== ""

					Behavior on text {
						SequentialAnimation {
							PropertyAnimation {
								duration: Constants.animation_duration
								easing.type: Easing.InCubic
								property: "opacity"
								target: selectionTitle
								to: 0
							}
							PropertyAction {
								property: "text"
								target: selectionTitle
							}
							PropertyAnimation {
								duration: Constants.animation_duration
								easing.type: Easing.OutCubic
								property: "opacity"
								target: selectionTitle
								to: 1
							}
						}
					}
				}
			}
			GSpacer {
				Layout.fillWidth: true
			}
			TintableIcon {
				id: selectionIcon

				Layout.rightMargin: root.arrowToLeft ? root.horizontalMargin : 0
				sourceSize.height: Style.dimens.small_icon_size
				tintColor: Style.color.textNormal.basic
				tintEnabled: false
				visible: source.toString() !== ""
			}
			LeftRightArrow {
				isLeft: root.arrowToLeft
				visible: !root.arrowToLeft
			}
		}

		onClicked: expanded = !expanded
		onFocusChanged: if (focus)
			root.receivedFocus(this)

		StatefulColors {
			id: colors

			statefulControl: expandButton
		}
	}
	RoundedRectangle {
		id: collapsibleContentBackground

		Layout.fillWidth: true
		bottomLeftCorner: drawBottomCorners
		bottomRightCorner: drawBottomCorners
		clip: true
		color: Style.color.paneSublevel.background.basic
		implicitHeight: expanded ? (contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin) : 0
		implicitWidth: contentItem.implicitWidth + contentItem.anchors.leftMargin + contentItem.anchors.rightMargin
		topLeftCorner: false
		topRightCorner: false
		visible: expanded

		Behavior on implicitHeight {
			NumberAnimation {
				duration: Constants.animation_duration
			}
		}

		ColumnLayout {
			id: contentItem

			spacing: Constants.groupbox_spacing

			anchors {
				bottomMargin: contentBottomMargin
				fill: parent
				leftMargin: contentHorizontalMargin
				rightMargin: contentHorizontalMargin
				topMargin: contentTopMargin
			}
		}
	}

	component LeftRightArrow: TintableIcon {
		property bool isLeft: false

		Layout.leftMargin: isLeft ? root.horizontalMargin : Constants.text_spacing
		Layout.rightMargin: isLeft ? Constants.text_spacing : root.horizontalMargin
		source: expanded ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
		sourceSize.height: Style.text.normal.textSize
		tintColor: Style.color.textNormal.basic
		tintEnabled: true
	}
}
