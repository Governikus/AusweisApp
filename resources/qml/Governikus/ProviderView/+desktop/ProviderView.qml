import QtQuick 2.10

import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Provider 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage {
	id: sectionPage

	KeyNavigation.tab: navSuccessor
	Accessible.role: Accessible.Grouping
	Accessible.name: qsTr("Provider view")
	Accessible.description: qsTr("This is the provider view of the AusweisApp2.")

	titleBarAction: TitleBarAction {
		text: qsTr("Provider")
		onClicked: {
			rect.visible = true
			detailView.visible = false
		}
		customSubAction: SearchBar {
			availableWidth: ApplicationModel.scaleFactor * 768
		}
		showHelp: false
	}

	Rectangle {
		id: rect

		anchors.fill: parent
		color: "red"
		opacity: 0.5
		visible: true

		MouseArea {
			anchors.fill: parent
			onClicked: {
				rect.visible = false
				detailView.visible = true
			}
		}
	}

	ProviderDetailView {
		id: detailView
		visible: false
		onNextView: sectionPage.nextView(pName)
	}
}
