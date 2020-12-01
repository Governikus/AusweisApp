/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12


/*
 * Convenience utility to access properties of a ProviderModel item
 * This ensures having always a defined string, i.e. a non-null string object.
 */
Item {
	id: baseItem
	property var modelItem

	readonly property string category: !!modelItem && !!modelItem.providerCategory ? modelItem.providerCategory : ""
	readonly property string shortName: !!modelItem && !!modelItem.providerShortName ? modelItem.providerShortName : ""
	readonly property string longName: !!modelItem && !!modelItem.providerLongName ? modelItem.providerLongName : ""
	readonly property string shortDescription: !!modelItem && !!modelItem.providerShortDescription ? modelItem.providerShortDescription : ""
	readonly property string longDescription: !!modelItem && !!modelItem.providerLongDescription ? modelItem.providerLongDescription : ""
	readonly property string address: !!modelItem && !!modelItem.providerAddress ? modelItem.providerAddress : ""
	readonly property string addressDomain: !!modelItem && !!modelItem.providerAddressDomain ? modelItem.providerAddressDomain : ""
	readonly property string homepage: !!modelItem && !!modelItem.providerHomepage ? modelItem.providerHomepage : ""
	readonly property string homepageBase: !!modelItem && !!modelItem.providerHomepageBase ? modelItem.providerHomepageBase : ""
	readonly property string phone: !!modelItem && !!modelItem.providerPhone ? modelItem.providerPhone : ""
	readonly property string phoneCost: !!modelItem && !!modelItem.providerPhoneCost ? modelItem.providerPhoneCost : ""
	readonly property string email: !!modelItem && !!modelItem.providerEmail ? modelItem.providerEmail : ""
	readonly property string postalAddress: !!modelItem && !!modelItem.providerPostalAddress ? modelItem.providerPostalAddress : ""
	readonly property string icon: !!modelItem && !!modelItem.providerIcon ? modelItem.providerIcon : ""
	readonly property string image: !!modelItem && !!modelItem.providerImage ? modelItem.providerImage : ""

	readonly property ListModel contactModel: ListModel {
		readonly property alias homepage: baseItem.homepage
		readonly property alias email: baseItem.email
		readonly property alias phone: baseItem.phone
		readonly property alias phoneCost: baseItem.phoneCost
		readonly property string phoneDisplayString: {
			var s = ""
			if (!!phone) {
				s = '<a href="tel:' + phone + '">' + phone + "</a>"
				if (!!phoneCost) {
					s += "<br/>" + phoneCost
				}
			}
			return s
		}
		readonly property alias  postalAddress: baseItem.postalAddress

		function removeHtml(htmlString) {
			return htmlString.replace(/<\/?[a-z][a-z0-9]*[^>]*>/ig, " ");
		}

		onHomepageChanged: {
			setProperty(0, "text", !!homepage ? '<a href="' + homepage + '">' + homepage + "</a>" : "")
			setProperty(0, "link", homepage)
		}
		onEmailChanged: {
			setProperty(1, "text", !!email ? '<a href="mailto:' + email + '">' + email + "</a>" : "")
			setProperty(1, "link", !!email ? "mailto:" + email : "")
		}
		onPhoneDisplayStringChanged: {
			setProperty(2, "text", phoneDisplayString)
			setProperty(2, "link", !!phone? "tel:" + phone : "")
		}
		onPostalAddressChanged: {
			var dest
			if (Qt.platform.os === "android") {
				dest = 'geo:0,0?q='
			}
			else if (Qt.platform.os === "ios") {
				dest = 'maps:0,0?q='
			}
			else {
				dest = 'https://www.google.com/maps?q='
			}

			dest = !!postalAddress ? dest + encodeURIComponent(postalAddress.replace(/<br\s*[\/]?>/gi,' ')) : ""
			setProperty(3, "text", !!postalAddress ? '<a href="' + dest + '">' + postalAddress + "</a>" : "")
			setProperty(3, "link", dest)
		}

		ListElement {
			iconSource: "qrc:///images/material_open_in_new.svg"
			label: QT_TR_NOOP("Homepage")
			text: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_mail.svg"
			label: QT_TR_NOOP("E-Mail")
			text: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_phone.svg"
			label: QT_TR_NOOP("Phone")
			text: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_location.svg"
			label: QT_TR_NOOP("Contact")
			text: ""
			link: ""
		}
	}
}
