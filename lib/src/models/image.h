#ifndef IMAGE_H
#define IMAGE_H

#include <QDateTime>
#include <QMap>
#include <QMetaType>
#include <QNetworkReply>
#include <QObject>
#include <QPixmap>
#include <QString>
#include "image-size.h"
#include "loader/downloadable.h"
#include "loader/token.h"
#include "models/pool.h"
#include "tags/tag.h"


class ExtensionRotator;
class Page;
class Profile;
class QSettings;
class Site;

class Image : public QObject, public Downloadable
{
	Q_OBJECT

	public:
		Image();
		Image(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent = nullptr);
		Image(const Image &other);

		// TODO: remove these two methods
		QMap<QString, Image::SaveResult> save(const QString &filename, const QString &path, bool addMd5 = true, bool startCommands = false, int count = 1);

		int value() const;
		QString md5() const;
		const QString &rating() const;
		const QList<Tag> &tags() const;
		QStringList tagsString() const;
		const QList<Pool> &pools() const;
		qulonglong id() const;
		int fileSize() const;
		int width() const;
		int height() const;
		const QDateTime &createdAt() const;
		const QUrl &pageUrl() const;
		const QUrl &fileUrl() const;
		QSize size() const;
		const QString &name() const;
		Page *page() const;
		const QStringList &search() const;
		Site *parentSite() const;
		ExtensionRotator *extensionRotator() const;
		bool hasTag(QString tag) const;
		bool hasAnyTag(const QStringList &tags) const;
		bool hasAllTags(const QStringList &tags) const;
		bool hasUnknownTag() const;
		void setUrl(const QUrl &url);
		void setSize(QSize size);
		void setFileSize(int size);
		void setFileExtension(const QString &ext);
		void setTemporaryPath(const QString &path);
		void setSavePath(const QString &path);
		QString savePath() const;
		Size preferredDisplaySize() const;
		bool isVideo() const;
		QString isAnimated() const;
		void setTags(const QList<Tag> &tags);
		bool isGallery() const;
		QString extension() const;

		// Preview pixmap store
		QPixmap previewImage() const;
		const QPixmap &previewImage();
		void setPreviewImage(const QPixmap &preview);

		// Displayable
		QColor color() const override;
		QString tooltip() const override;
		QString counter() const override;
		QList<QStrP> detailsData() const override;

		// Downloadable
		QUrl url(Size size = Size::Full) const override;
		void preload(const Filename &filename) override;
		QStringList paths(const QString &filename, const QString &folder, int count) const;
		QStringList paths(const Filename &filename, const QString &folder, int count) const override;
		QMap<QString, Token> generateTokens(Profile *profile) const override;
		SaveResult preSave(const QString &path) override;
		void postSave(const QString &path, SaveResult result, bool addMd5, bool startCommands, int count) override;

		// Templates
		/*template <typename T>
		T token(const QString &name) const
		{
			return tokens(m_profile).value(name).value<T>();
		}*/

	protected:
		QList<Tag> filteredTags(const QStringList &remove) const;
		void setRating(const QString &rating);

		// Saving
		SaveResult save(const QString &path, bool force = false, bool basic = false, bool addMd5 = true, bool startCommands = false, int count = 1, bool postSave = true);
		void postSaving(const QString &path, bool addMd5 = true, bool startCommands = false, int count = 1, bool basic = false);
		QMap<QString, Image::SaveResult> save(const QStringList &paths, bool addMd5 = true, bool startCommands = false, int count = 1, bool force = false);

	public slots:
		void loadDetails(bool rateLimit = false);
		void loadDetailsNow();
		void abortTags();
		void parseDetails();

	signals:
		void finishedLoadingPreview();
		void finishedLoadingTags();
		void urlChanged(const QUrl &before, const QUrl &after);

	private:
		Profile *m_profile;
		Page *m_parent;
		qulonglong m_id;
		int m_score, m_parentId, m_authorId;
		bool m_hasChildren, m_hasNote, m_hasComments, m_hasScore;
		QUrl m_url;
		QString mutable m_md5;
		QString m_author, m_name, m_status, m_rating, m_site;
		QStringList m_sources;
		QUrl m_pageUrl, m_fileUrl, m_sampleUrl, m_previewUrl;
		QPixmap m_imagePreview;
		QRect m_previewRect;
		QDateTime m_createdAt;
		QNetworkReply *m_loadDetails;
		QList<Tag> m_tags;
		QList<Pool> m_pools;
		QTime m_timer;
		QSettings *m_settings;
		QStringList m_search;
		Site *m_parentSite;
		ExtensionRotator *m_extensionRotator;
		bool m_loadingDetails, m_loadedDetails;
		bool m_isGallery = false;
		int m_galleryCount;
		QMap<Image::Size, ImageSize> m_sizes;
};

Q_DECLARE_METATYPE(Image)
Q_DECLARE_METATYPE(Image::SaveResult)

#endif // IMAGE_H
