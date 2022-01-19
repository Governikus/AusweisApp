/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0

Item {
	signal reset

	function firePush(pSectionPage) {
		StackView.view.push(pSectionPage)
	}

	function firePushWithProperties(pSectionPage, pProperties) {
		StackView.view.push(pSectionPage, pProperties)
	}

	function fireReplace(pSectionPage) {
		if (StackView.view.depth <= 1) {
			StackView.view.push(pSectionPage)
			return
		}
		StackView.view.replace(pSectionPage)
	}

	function firePop() {
		StackView.view.pop()
	}

	function firePopAll() {
		StackView.view.pop(null)
	}

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
		forceActiveFocus()
		highlightScrollbar()
	}

	readonly property bool topLevelPage: StackView.index === 0

	property var navigationAction: null
	property string title: ""
	property var rightTitleBarAction: null
	property var subTitleBarAction: null

	property bool titleBarVisible: true
	property color titleBarColor: Style.color.accent
	property real titleBarOpacity: 1
	property bool contentBehindTitlebar: false

	// Main flickable of this view
	property var sectionPageFlickable: flickable

	// When enabled the section page will automatically add a safeAreaMargin to the bottom of the page
	property bool automaticSafeAreaMarginHandling: true

	property alias content: flickableContent.data

	Accessible.onScrollDownAction: flickable.scrollPageDown()
	Accessible.onScrollUpAction: flickable.scrollPageUp()

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
