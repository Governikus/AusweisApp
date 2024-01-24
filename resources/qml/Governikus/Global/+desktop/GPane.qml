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
	property int contentPadding: Constants.pane_padding
	default property alias data: paneContent.data
	property bool drawShadow: true
	property alias spacing: paneContent.spacing
	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	Accessible.focusable: title !== ""
	Accessible.name: title
	Accessible.role: Accessible.Grouping
	Layout.maximumHeight: containerCol.Layout.maximumHeight
	activeFocusOnTab: title !== ""
	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software && drawShadow

		effect: GDropShadow {
		}
	}
	ColumnLayout {
		id: containerCol

		anchors.fill: parent
		spacing: Constants.text_spacing

		GText {
			id: titleText

			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			Layout.topMargin: Constants.pane_padding
			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.subline
			visible: text !== ""

			FocusFrame {
				scope: root
			}
		}
		ColumnLayout {
			id: paneContent

			Layout.bottomMargin: root.contentPadding
			Layout.leftMargin: root.contentPadding
			Layout.rightMargin: root.contentPadding
			Layout.topMargin: titleText.visible ? 0 : root.contentPadding
			spacing: Constants.pane_spacing
		}
	}
}
