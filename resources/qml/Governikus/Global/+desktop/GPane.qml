/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel

GPaneBackground {
	id: root

	property alias content: paneContent
	default property alias data: paneContent.data
	property bool drawShadow: true
	property alias spacing: paneContent.spacing
	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	Accessible.focusable: title !== ""
	Accessible.name: title
	Accessible.role: Accessible.Grouping
	activeFocusOnTab: title !== ""
	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software && drawShadow

		effect: GDropShadow {
		}
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
			textStyle: Style.text.subline
			width: Math.min(parent.width, Math.ceil(implicitWidth))

			FocusFrame {
				scope: root
			}
		}
		ColumnLayout {
			id: paneContent

			spacing: Constants.pane_spacing
			width: parent.width
		}
	}
}
