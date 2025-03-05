/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

SectionPage {
	id: root

	default property alias data: flickable.data
	property bool fillWidth: false
	property real margins: Style.dimens.pane_padding
	property alias spacing: flickable.spacing

	function positionViewAtItem(pItem) {
		flickable.positionViewAtItem(pItem);
	}
	function scrollPageDown() {
		flickable.scrollPageDown();
	}
	function scrollPageUp() {
		flickable.scrollPageUp();
	}

	contentIsScrolled: !flickable.atYBeginning

	Keys.onPressed: event => {
		flickable.handleKeyPress(event.key);
	}

	Connections {
		function onActivate() {
			flickable.highlightScrollbar();
		}
	}
	GFlickableColumnLayout {
		id: flickable

		anchors.fill: parent
		bottomMargin: root.margins
		leftMargin: root.margins
		maximumContentWidth: root.fillWidth ? -1 : Style.dimens.max_text_width
		rightMargin: root.margins
		spacing: 0
		topMargin: root.margins
	}
}
