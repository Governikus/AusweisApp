/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0

Controller {
	id: root

	// When enabled the section page will automatically add a safeAreaMargin to the bottom of the page
	property bool automaticSafeAreaMarginHandling: true
	property alias content: flickableContent.data
	property bool contentBehindTitlebar: false
	property bool hiddenNavbarPadding: false
	property var navigationAction: null
	property var rightTitleBarAction: null

	// Main flickable of this view
	property var sectionPageFlickable: flickable
	property string title: ""
	property color titleBarColor: Style.color.accent
	property real titleBarOpacity: 1
	property bool titleBarVisible: true
	readonly property bool topLevelPage: StackView.index === 0

	signal reset

	function activated() {
		if (ApplicationModel.isScreenReaderRunning()) {
			updateFocus();
		}
		highlightScrollbar();
	}
	function highlightScrollbar() {
		sectionPageFlickable.highlightScrollbar();
	}
	function scrollPageDown() {
		sectionPageFlickable.scrollPageDown();
	}
	function scrollPageUp() {
		sectionPageFlickable.scrollPageUp();
	}

	GFlickable {
		id: flickable
		anchors.bottom: parent.bottom
		contentHeight: flickableContent.height
		contentWidth: flickableContent.width
		height: contentBehindTitlebar ? (parent.height + Style.dimens.titlebar_height) : parent.height
		scrollBarTopPadding: contentBehindTitlebar ? Style.dimens.titlebar_height : 0
		width: parent.width

		Item {
			id: flickableContent
			height: childrenRect.height
			width: flickable.width
		}
	}
}
