/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	required property int count
	property bool flowHorizontally: true
	required property int index
	property alias source: tabImage.source

	Accessible.name: {
		//: MOBILE Relative position of current navigation tab in navigation view. %1 is replaced with the current tab's index, %2 with the total count of tabs
		const elemString = qsTr("%1 of %2").arg(index + 1).arg(count);
		//: MOBILE
		var a11yName = [text, qsTr("Tab"), elemString];
		if (checked && Qt.platform.os === "ios") {
			//: IOS Selected navigation tab.
			a11yName.unshift(qsTr("Selection"));
			//: IOS Name of a11y element of selected navigation tab.
			a11yName.unshift(qsTr("Tab bar"));
		} else if (checked && Qt.platform.os === "android") {
			//: ANDROID Currently selected navigation tab of navigation view.
			a11yName.unshift(qsTr("Selected"));
		}
		return a11yName.join(", ");
	}
	Accessible.role: Accessible.PageTab
	Layout.minimumWidth: tabImage.implicitWidth + leftPadding + rightPadding
	padding: Style.dimens.text_spacing / 2

	background: Rectangle {
		id: pane

		border.color: colors.paneBorder
		color: colors.paneBackground
		radius: Style.dimens.control_radius
	}
	contentItem: Item {
		implicitHeight: grid.implicitHeight
		implicitWidth: Math.ceil(tabImage.implicitWidth + grid.columnSpacing + tabText.implicitWidth)

		GridLayout {
			id: grid

			Accessible.ignored: true
			anchors.centerIn: parent
			columnSpacing: Style.dimens.text_spacing
			columns: 2
			flow: root.flowHorizontally ? GridLayout.LeftToRight : GridLayout.TopToBottom
			rowSpacing: Style.dimens.navigation_bar_text_padding
			rows: 2

			TintableIcon {
				id: tabImage

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignRight : Qt.AlignCenter
				Layout.maximumWidth: implicitWidth
				sourceSize.height: Style.dimens.navigation_bar_icon_size
				tintColor: tabText.color
			}
			GText {
				id: tabText

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignLeft : Qt.AlignCenter
				Layout.preferredWidth: Math.min(Math.ceil(implicitWidth), root.contentItem.width)
				color: colors.textNormal
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 1
				text: root.text
				textStyle: Style.text.navigation
			}
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
	FocusFrame {
	}
}
