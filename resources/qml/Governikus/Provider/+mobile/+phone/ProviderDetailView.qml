/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem

	readonly property real tabBarSpacing: Constants.is_layout_ios ? Constants.component_spacing : 0
	property alias providerModelItem: provider.modelItem

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }
	title: provider.shortName
	titleBarColor: Category.displayColor(provider.category)
	titleBarOpacity: header.titleBarOpacity
	contentBehindTitlebar: true

	ProviderModelItem {
		id: provider
	}

	content: Column {
		spacing: Constants.pane_padding
		bottomPadding: Constants.pane_padding

		ProviderHeader {
			id: header

			width: baseItem.width

			selectedProvider: provider
		}

		GPaneBackground {
			anchors {
				left: parent.left
				right: parent.right
				margins: Constants.pane_padding
			}

			implicitHeight: description.implicitHeight + 2 * Constants.pane_padding

			ProviderDetailDescription {
				id: description

				anchors.fill: parent
				anchors.margins: Constants.pane_padding

				description: provider.longDescription
			}
		}

		GPaneBackground {
			anchors {
				left: parent.left
				right: parent.right
				margins: Constants.pane_padding
			}

			implicitHeight: contactInfo.implicitHeight + 2 * Constants.pane_padding

			ProviderContactInfo {
				id: contactInfo

				anchors.fill: parent
				anchors.margins: Constants.pane_padding

				titleTextStyle: Style.text.header_accent
				textStyle: Style.text.normal_accent

				contactModel: provider.contactModel
			}
		}

	}
}
