/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	id: root

	Accessible.role: Accessible.Grouping
	Accessible.name: title
	Accessible.focusable: title !== ""

	activeFocusOnTab: title !== ""

	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	readonly property int availableContentHeight: {
		var availableHeight = height - containerCol.topPadding - containerCol.bottomPadding

		if (title === "") {
			return availableHeight
		}

		return availableHeight - titleText.height - containerCol.spacing
	}

	property alias content: paneContent
	default property alias data: paneContent.data

	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	GPaneBackground {
		anchors.fill: parent
	}

	Column {
		id: containerCol

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding
		topPadding: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing

		GText {
			id: titleText

			width: Math.min(parent.width, implicitWidth)

			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.header_accent

			FocusFrame {
				scope: root
			}
		}

		Column {
			id: paneContent

			width: parent.width
			spacing: Constants.pane_spacing
		}
	}

}
