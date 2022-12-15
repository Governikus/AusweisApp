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

	property alias providerModelItem: provider.modelItem
	readonly property real tabBarSpacing: Constants.is_layout_ios ? Constants.component_spacing : 0

	contentBehindTitlebar: true
	title: provider.shortName
	titleBarColor: Category.displayColor(provider.category)
	titleBarOpacity: header.titleBarOpacity

	content: Column {
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_padding

		ProviderHeader {
			id: header
			selectedProvider: provider
			width: baseItem.width
		}
		GPaneBackground {
			implicitHeight: description.implicitHeight + 2 * Constants.pane_padding

			anchors {
				left: parent.left
				margins: Constants.pane_padding
				right: parent.right
			}
			ProviderDetailDescription {
				id: description
				anchors.fill: parent
				anchors.margins: Constants.pane_padding
				description: provider.longDescription
			}
		}
		GPaneBackground {
			implicitHeight: contactInfo.implicitHeight + 2 * Constants.pane_padding

			anchors {
				left: parent.left
				margins: Constants.pane_padding
				right: parent.right
			}
			ProviderContactInfo {
				id: contactInfo
				anchors.fill: parent
				anchors.margins: Constants.pane_padding
				contactModel: provider.contactModel
				textStyle: Style.text.normal_accent
				titleTextStyle: Style.text.header_accent
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	ProviderModelItem {
		id: provider
	}
}
