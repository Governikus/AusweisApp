/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

GPaneBackground {
	id: root

	property alias bold: titleText.font.bold
	property alias contentSpacing: paneContent.spacing
	property bool drawShadow: true
	property alias horizontalTitleAlignment: titleText.horizontalAlignment
	property int padding: Constants.pane_padding
	default property alias paneData: paneContent.data
	property int spacing: Constants.pane_spacing
	property alias textStyle: titleText.textStyle
	property alias title: titleText.text

	height: childrenRect.height
	implicitHeight: content.implicitHeight

	Column {
		id: content

		anchors.left: parent.left
		anchors.leftMargin: root.padding
		anchors.right: parent.right
		anchors.rightMargin: root.padding
		bottomPadding: root.padding
		spacing: root.spacing
		topPadding: root.padding

		PaneTitle {
			id: titleText

			width: parent.width
		}
		Column {
			id: paneContent

			spacing: root.spacing
			width: parent.width
		}
	}
	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software && drawShadow

		effect: GDropShadow {
		}
	}
}
