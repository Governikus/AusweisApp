/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import Governikus.Global
import Governikus.Style

SectionPage {
	id: root

	property bool fillWidth: false
	property real margins: Style.dimens.pane_padding
	default property alias pageData: flickable.layoutData
	property alias spacing: flickable.spacing

	function positionViewAtItem(pItem) {
		Utils.positionFlickableAtItem(flickable, pItem);
	}
	function scrollPageDown() {
		flickable.scrollPageDown();
	}
	function scrollPageUp() {
		flickable.scrollPageUp();
	}

	contentIsScrolled: !flickable.atYBeginning

	Keys.onPressed: event => {
		flickable.handleKeyPress(event);
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
