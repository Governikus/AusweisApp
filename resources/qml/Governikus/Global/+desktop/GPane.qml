/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: root

	readonly property int availableContentHeight: {
		var availableHeight = height - containerCol.topPadding - containerCol.bottomPadding;
		if (title === "") {
			return availableHeight;
		}
		return availableHeight - titleText.height - containerCol.spacing;
	}
	property alias content: paneContent
	default property alias data: paneContent.data
	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	Accessible.focusable: title !== ""
	Accessible.name: title
	Accessible.role: Accessible.Grouping
	activeFocusOnTab: title !== ""
	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	GPaneBackground {
		anchors.fill: parent
	}
	Column {
		id: containerCol
		anchors.left: parent.left
		anchors.leftMargin: Constants.pane_padding
		anchors.right: parent.right
		anchors.rightMargin: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing
		topPadding: Constants.pane_padding

		GText {
			id: titleText
			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.header_accent
			width: Math.min(parent.width, implicitWidth)

			FocusFrame {
				scope: root
			}
		}
		Column {
			id: paneContent
			spacing: Constants.pane_spacing
			width: parent.width
		}
	}
}
