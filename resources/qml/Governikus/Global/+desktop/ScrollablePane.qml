/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel

Item {
	id: root

	property alias backgroundColor: background.color
	property alias content: paneContent
	default property alias data: paneContent.data
	property bool enableDropShadow: false
	property real minimumVisibleContentHeight: 0
	property alias title: titleText.text

	function scrollYPositionIntoView(pYposition) {
		let availableFlickableHeight = flickable.height - paneContent.anchors.margins;
		let dy = pYposition - flickable.contentY - availableFlickableHeight;
		if (dy > 0 || flickable.contentY > 0) {
			flickable.contentY += dy;
			if (flickable.contentY < 0)
				flickable.contentY = 0;
			else if (flickable.contentY + availableFlickableHeight > flickable.contentHeight)
				flickable.contentY = flickable.contentHeight - availableFlickableHeight;
		}
	}

	Accessible.focusable: true
	Accessible.name: titleText.text
	Accessible.role: Accessible.Grouping
	Layout.minimumHeight: content.Layout.minimumHeight + 2 * content.anchors.margins
	implicitHeight: content.implicitHeight + 2 * content.anchors.margins
	implicitWidth: content.implicitWidth + 2 * content.anchors.margins

	GPaneBackground {
		id: background

		anchors.fill: parent

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software && enableDropShadow

			effect: GDropShadow {
			}
		}
	}
	ColumnLayout {
		id: content

		anchors.fill: parent
		anchors.margins: Style.dimens.border_width
		clip: true

		GFlickable {
			id: flickable

			readonly property real reservedTitleHeight: (root.title === "" ? 0 : titleText.height + Constants.pane_spacing)

			Layout.alignment: Qt.AlignTop
			Layout.bottomMargin: Constants.pane_padding
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.maximumHeight: contentHeight
			Layout.minimumHeight: root.minimumVisibleContentHeight ? reservedTitleHeight + root.minimumVisibleContentHeight : -1
			Layout.preferredHeight: contentColumn.implicitHeight
			Layout.topMargin: Constants.pane_padding
			contentHeight: contentColumn.implicitHeight

			ColumnLayout {
				id: contentColumn

				anchors.fill: parent
				anchors.rightMargin: Constants.pane_padding
				spacing: Constants.pane_spacing

				GText {
					id: titleText

					elide: Text.ElideRight
					textStyle: Style.text.headline
					visible: text !== ""

					FocusFrame {
						scope: root
					}
				}
				ColumnLayout {
					id: paneContent

					Layout.fillWidth: true
					spacing: Constants.pane_spacing
				}
			}
		}
	}
	ScrollGradients {
		anchors.fill: parent
		anchors.margins: Style.dimens.border_width
		color: background.color
	}
}
