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

	signal reset()

	function scrollPageDown() {
		sectionPageFlickable.scrollPageDown()
	}

	function scrollPageUp() {
		sectionPageFlickable.scrollPageUp()
	}

	function highlightScrollbar() {
		sectionPageFlickable.highlightScrollbar()
	}

	function activated() {
		if (ApplicationModel.isScreenReaderRunning()) {
			updateFocus()
		}
		highlightScrollbar()
	}

	readonly property bool topLevelPage: StackView.index === 0

	property var navigationAction: null
	property string title: ""
	property var rightTitleBarAction: null

	property bool titleBarVisible: true
	property color titleBarColor: Style.color.accent
	property real titleBarOpacity: 1
	property bool contentBehindTitlebar: false

	// Main flickable of this view
	property var sectionPageFlickable: flickable

	// When enabled the section page will automatically add a safeAreaMargin to the bottom of the page
	property bool automaticSafeAreaMarginHandling: true
	property bool hiddenNavbarPadding: false

	property alias content: flickableContent.data

	GFlickable {
		id: flickable

		height: contentBehindTitlebar ? (parent.height + Style.dimens.titlebar_height) : parent.height
		width: parent.width
		anchors.bottom: parent.bottom

		scrollBarTopPadding: contentBehindTitlebar ? Style.dimens.titlebar_height : 0

		contentWidth: flickableContent.width
		contentHeight: flickableContent.height

		Item {
			id: flickableContent
			width: flickable.width
			height: childrenRect.height
		}
	}
}
