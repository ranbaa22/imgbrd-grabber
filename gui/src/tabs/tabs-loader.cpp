#include "tabs-loader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "ui_tag-tab.h"
#include "ui_pool-tab.h"


bool TabsLoader::load(QString path, QList<tagTab*> &tagTabs, QList<poolTab*> &poolTabs, Profile *profile, QMap<QString, Site*> &sites, mainWindow *parent)
{
	QFile f(path);
	if (!f.open(QFile::ReadOnly))
	{
		return false;
	}

	// Get the file's header to get the version
	QString header = f.readLine().trimmed();
	f.reset();

	// Version 1 is plain text
	if (!header.startsWith("{"))
	{
		QString links = f.readAll().trimmed();
		f.close();

		QStringList tabs = links.split("\r\n");
		for (int j = 0; j < tabs.size(); j++)
		{
			QStringList infos = tabs[j].split("¤");
			if (infos.size() > 3)
			{
				if (infos[infos.size() - 1] == "pool")
				{
					poolTab *tab = new poolTab(&sites, profile, parent);
					tab->ui->spinPool->setValue(infos[0].toInt());
					tab->ui->comboSites->setCurrentIndex(infos[1].toInt());
					tab->ui->spinPage->setValue(infos[2].toInt());
					tab->ui->spinImagesPerPage->setValue(infos[4].toInt());
					tab->ui->spinColumns->setValue(infos[5].toInt());
					tab->setTags(infos[2]);

					poolTabs.append(tab);
				}
				else
				{
					tagTab *tab = new tagTab(&sites, profile, parent);
					tab->ui->spinPage->setValue(infos[1].toInt());
					tab->ui->spinImagesPerPage->setValue(infos[2].toInt());
					tab->ui->spinColumns->setValue(infos[3].toInt());
					tab->setTags(infos[0]);

					tagTabs.append(tab);
				}
			}
		}

		return true;
	}

	// Other versions are JSON-based
	else
	{
		QByteArray data = f.readAll();
		QJsonDocument loadDoc = QJsonDocument::fromJson(data);
		QJsonObject object = loadDoc.object();

		int version = object["version"].toInt();
		switch (version)
		{
			case 2:
				QJsonArray tabs = object["tabs"].toArray();
				for (auto tab : tabs)
				{
					auto infos = tab.toObject();
					QString type = infos["type"].toString();

					if (type == "tag")
					{
						tagTab *tab = new tagTab(&sites, profile, parent);
						tab->ui->spinPage->setValue(infos["page"].toInt());
						tab->ui->spinImagesPerPage->setValue(infos["perpage"].toInt());
						tab->ui->spinColumns->setValue(infos["columns"].toInt());

						QStringList selectedSources;
						QJsonArray jsonSelectedSources = infos["sites"].toArray();
						for (auto site : jsonSelectedSources)
							selectedSources.append(site.toString());
						QList<bool> selectedSourcesBool;
						for (Site *site : sites)
							selectedSourcesBool.append(selectedSources.contains(site->name()));
						tab->saveSources(selectedSourcesBool);

						QStringList tags;
						QJsonArray jsonTags = infos["tags"].toArray();
						for (auto tag : jsonTags)
							tags.append(tag.toString());
						tab->setTags(tags.join(' '));

						QStringList postFilters;
						QJsonArray jsonPostFilters = infos["postFiltering"].toArray();
						for (auto tag : jsonPostFilters)
							postFilters.append(tag.toString());
						tab->setPostFilter(postFilters.join(' '));

						tagTabs.append(tab);
					}
					else if (type == "pool")
					{
						poolTab *tab = new poolTab(&sites, profile, parent);
						tab->ui->spinPool->setValue(infos["pool"].toInt());
						tab->ui->comboSites->setCurrentText(infos["site"].toString());
						tab->ui->spinPage->setValue(infos["page"].toInt());
						tab->ui->spinImagesPerPage->setValue(infos["perpage"].toInt());
						tab->ui->spinColumns->setValue(infos["columns"].toInt());

						QStringList tags;
						QJsonArray jsonTags = infos["tags"].toArray();
						for (auto tag : jsonTags)
							tags.append(tag.toString());
						tab->setTags(tags.join(' '));

						QStringList postFilters;
						QJsonArray jsonPostFilters = infos["postFiltering"].toArray();
						for (auto tag : jsonPostFilters)
							postFilters.append(tag.toString());
						tab->setPostFilter(postFilters.join(' '));

						poolTabs.append(tab);
					}
				}
				return true;
		}

	}

	return false;
}

bool TabsLoader::save(QString path, QList<tagTab*> &tagTabs, QList<poolTab*> &poolTabs, QList<searchTab*> &allTabs)
{
	QFile saveFile(path);
	if (!saveFile.open(QFile::WriteOnly))
	{
		return false;
	}

	QJsonArray tabsJson;
	for (tagTab *tab : tagTabs)
	{
		if (tab == nullptr || !allTabs.contains(tab))
			continue;

		QJsonObject tabJson;
		tab->write(tabJson);
		tabsJson.append(tabJson);
	}
	for (poolTab *tab : poolTabs)
	{
		if (tab == nullptr || !allTabs.contains(tab))
			continue;

		QJsonObject tabJson;
		tab->write(tabJson);
		tabsJson.append(tabJson);
	}

	// Generate result
	QJsonObject full;
	full["version"] = 2;
	full["tabs"] = tabsJson;

	// Write result
	QJsonDocument saveDoc(full);
	saveFile.write(saveDoc.toJson());
	saveFile.close();

	return true;
}
