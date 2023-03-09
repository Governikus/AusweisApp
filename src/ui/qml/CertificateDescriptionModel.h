/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the CV certificate description.
 */

#pragma once

#include "Env.h"
#include "asn1/CertificateDescription.h"
#include "context/AuthContext.h"

#include <QAbstractListModel>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace governikus
{

class CertificateDescriptionModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString subjectName READ getSubjectName NOTIFY fireChanged)
	Q_PROPERTY(QString purpose READ getPurpose NOTIFY fireChanged)

	private:
		QVector<QPair<QString, QString>> mData;
		QSharedPointer<AuthContext> mContext;

		CertificateDescriptionModel();
		~CertificateDescriptionModel()override = default;

		inline QSharedPointer<const CertificateDescription> getCertificateDescription() const;
		inline QString getValidity() const;
		void initModelData(const QSharedPointer<const CertificateDescription>& pCertDescription);

	private Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		enum UserRoles
		{
			LABEL = Qt::UserRole + 1,
			TEXT
		};

		void resetContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		[[nodiscard]] QString getSubjectName() const;
		[[nodiscard]] QString getSubjectUrl() const;
		[[nodiscard]] QString getPurpose() const;

		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

	Q_SIGNALS:
		void fireChanged();
};


} // namespace governikus
