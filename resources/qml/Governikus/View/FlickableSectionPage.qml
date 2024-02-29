/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

SectionPage {
	id: root

	default property alias data: flickable.data
	property bool fillWidth: false
	property real margins: Constants.pane_padding
	property alias spacing: flickable.spacing

	function scrollPageDown() {
		flickable.scrollPageDown(baseItem);
	}
	function scrollPageUp() {
		flickable.scrollPageUp(baseItem);
	}

	contentIsScrolled: !flickable.atYBeginning

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
		maximumContentWidth: fillWidth ? -1 : Style.dimens.max_text_width
		rightMargin: root.margins
		spacing: 0
		topMargin: root.margins
	}
}
