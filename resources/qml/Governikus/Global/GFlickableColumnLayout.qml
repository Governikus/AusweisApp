/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0

GFlickable {
	id: root

	default property alias children: contentLayout.children
	property alias spacing: contentLayout.spacing
	property real maximumContentWidth: -1
	property bool fillHeight: true
	property real minimumContentHeight: implicitContentHeight
	readonly property real implicitContentHeight: contentLayout.implicitHeight

	contentWidth: root.width - leftMargin - rightMargin
	contentHeight: (fillHeight && d.contentHeightFitsOnScreen) ? d.availableContentHeight : root.minimumContentHeight

	leftMargin: Constants.component_spacing
	rightMargin: Constants.component_spacing
	topMargin: Constants.component_spacing
	bottomMargin: Constants.component_spacing

	QtObject {
		id: d

		property real availableContentHeight: root.height - topMargin - bottomMargin
		property bool contentHeightFitsOnScreen: root.minimumContentHeight <= availableContentHeight
	}

	ColumnLayout {
		id: contentLayout

		anchors.horizontalCenter: parent.horizontalCenter
		width: maximumContentWidth > 0 ? Math.min(root.contentWidth, maximumContentWidth) : root.contentWidth
		Binding on height {
			delayed: fillHeight
			value: root.contentHeight
		}
	}
}
