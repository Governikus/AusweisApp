/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

GPaneBackground {
	id: root

	property alias content: paneContent
	property int contentPadding: Style.dimens.pane_padding
	default property alias data: paneContent.data
	property alias spacing: paneContent.spacing
	property alias title: titleText.text
	property int titleMargins: Style.dimens.pane_padding
	property alias titleTextStyle: titleText.textStyle

	Layout.maximumHeight: containerCol.Layout.maximumHeight
	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	ColumnLayout {
		id: containerCol

		anchors.fill: parent
		spacing: 0

		GText {
			id: titleText

			Layout.leftMargin: root.titleMargins
			Layout.rightMargin: root.titleMargins
			Layout.topMargin: root.titleMargins
			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.subline
			visible: text !== ""
		}
		ColumnLayout {
			id: paneContent

			Layout.bottomMargin: root.contentPadding
			Layout.leftMargin: root.contentPadding
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.rightMargin: root.contentPadding
			Layout.topMargin: titleText.visible ? Style.dimens.pane_spacing : root.contentPadding
			spacing: Style.dimens.pane_spacing
		}
	}
}
