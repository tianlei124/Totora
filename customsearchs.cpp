#include "customsearchs.h"
#include "ui_customsearchs.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QDir>

CustomSearchs::CustomSearchs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomSearchs),
    searchEngines(), isNeedStore(false)
{
    ui->setupUi(this);

    QDir runtimeDir(QCoreApplication::applicationDirPath());
    QString configJsonPath(runtimeDir.absoluteFilePath("searchEngines.json"));

    QJsonDocument jsonDoc{};
    QFileInfo fileInfo(configJsonPath);
    if (fileInfo.isFile() && !loadJsonFile(configJsonPath, jsonDoc))
    {
        bool ret = loadJsonFile(":/configs/searchEngines.json", jsonDoc);
        qDebug() << "try :/configs/searchEngines.json, result " << ret;

        if (!ret)
        {
            std::exit(-1);
        }
    }

    QJsonObject rootObj = jsonDoc.object();
    if (rootObj.contains("searchEngines"))
    {
        QJsonArray subArray = rootObj.value("searchEngines").toArray();
        for (int i = 0;i < subArray.size();++i)
        {
            QJsonObject engineObject = subArray.at(i).toObject();
            searchEngine engineItem{};
            if (engineObject.contains("title"))
            {
                engineItem.title = engineObject.value("title").toString();
            }
            if (engineObject.contains("url"))
            {
                engineItem.url = engineObject.value("url").toString();
            }
            if (engineObject.contains("hotKey"))
            {
                QJsonArray hotKeyArray = engineObject.value("hotKey").toArray();
                for (int j = 0;j < hotKeyArray.size();++j)
                {
                    engineItem.hotKey[j] = hotKeyArray.at(j).toString()[0];
                }
            }
            bool isEnable = true;
            if (engineObject.contains("isEnable"))
            {
                isEnable = engineObject.value("isEnable").toBool(true);
            }
            searchEngines.push_back(std::make_pair(engineItem, isEnable));
        }
    }

    for (size_t i = 0;i < searchEngines.size();++i)
    {
        QString title = searchEngines[i].first.title;

        if (!searchEngines[i].second)
        {
            int disable_icon = ui->searchEngineItems->fontMetrics().width("ø");
            int title_width = ui->searchEngineItems->fontMetrics().width(searchEngines[i].first.title);
            for (int j = 0;j < (ui->searchEngineItems->width() - title_width - disable_icon) /
                 ui->searchEngineItems->fontMetrics().width(" ");++j)
            {
                title += " ";
            }
            title += "ø";
        }
        ui->searchEngineItems->addItem(title);
    }


    storeConfigTimer = new QTimer;
    if (!storeConfigTimer->isActive())
    {
        storeConfigTimer->start(10 * 60 * 100);
    }
    connect(this->storeConfigTimer, SIGNAL(timeout()), this, SLOT(rewriteSearchEngineConfig()));
}

CustomSearchs::~CustomSearchs()
{
    delete ui;
}

QString CustomSearchs::getSearchEngineURL(const QString& title)
{
    QString URL = "https://www.bing.com/search?q=@totora";
    for (const std::pair<searchEngine, bool>& engine : searchEngines)
    {
        if (engine.first.title == title)
        {
            URL = engine.first.url;
        }
    }
    return URL;
}

void CustomSearchs::on_searchEngineItems_clicked(const QModelIndex &index)
{
    ui->titleLineEdit->setText(searchEngines[index.row()].first.title);
    ui->urlLineEdit->setText(searchEngines[index.row()].first.url);

    ui->firstHotKey->setKeySequence(QKeySequence(searchEngines[index.row()].first.hotKey[0]));
    ui->secondHotKey->setKeySequence(QKeySequence(searchEngines[index.row()].first.hotKey[1]));
    ui->thirdHotKey->setKeySequence(QKeySequence(searchEngines[index.row()].first.hotKey[2]));

    if (searchEngines[index.row()].second)
    {
        ui->enableButton->setText("disable");
    }
    else
    {
        ui->enableButton->setText("enable");
    }
}

bool CustomSearchs::rewriteSearchEngineConfig()
{
    if (isNeedStore && !ui->searchEngineItems->isVisible())
    {
        QFile loadFile(QCoreApplication::applicationDirPath() + "/searchEngines.json");
        if (!loadFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "could't open projects json when write data";
            return false;
        }
        QJsonArray jsonArray;
        for (const std::pair<searchEngine, bool>& engine : searchEngines)
        {
            QJsonObject subObject;
            subObject.insert("title", engine.first.title);
            subObject.insert("url", engine.first.url);
            QJsonArray hotKeyArray;
            for (size_t j = 0;j < engine.first.hotKey.size();++j)
            {
                if (engine.first.hotKey[j] >= 'A' && engine.first.hotKey[j] <= 'Z')
                {
                    QJsonValue singleKey(engine.first.hotKey[j]);
                    hotKeyArray.push_back(singleKey);
                }
            }
            subObject.insert("hotKey", hotKeyArray);
            subObject.insert("isEnable", engine.second);

            jsonArray.push_back(subObject);
        }
        QJsonObject jsonObject;
        jsonObject.insert("searchEngines", jsonArray);

        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);

        loadFile.write(jsonDoc.toJson());
        loadFile.close();

        isNeedStore = false;
    }
    return true;
}

void CustomSearchs::on_deleteItemButton_clicked()
{
    if (static_cast<size_t>(ui->searchEngineItems->count()) != searchEngines.size() ||
            static_cast<size_t>(ui->searchEngineItems->currentRow()) > searchEngines.size() ||
            ui->searchEngineItems->currentRow() < 0)
        return ;

    qDebug() << "detete title " << searchEngines[ui->searchEngineItems->currentRow()].first.title << ", url"
             << searchEngines[ui->searchEngineItems->currentRow()].first.url;
    searchEngines.erase(searchEngines.begin() + ui->searchEngineItems->currentRow());

    QListWidgetItem * item = ui->searchEngineItems->takeItem(ui->searchEngineItems->currentRow());
    delete item;

    if (ui->searchEngineItems->count())
    {
        ui->titleLineEdit->setText(searchEngines[ui->searchEngineItems->currentRow()].first.title);
        ui->urlLineEdit->setText(searchEngines[ui->searchEngineItems->currentRow()].first.url);
        ui->firstHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[0]));
        ui->secondHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[1]));
        ui->thirdHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[2]));

        if (searchEngines[ui->searchEngineItems->currentRow()].second)
            ui->enableButton->setText("disable");
        else
            ui->enableButton->setText("enable");
    }

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_addItemButton_clicked()
{
    QString default_title = "New Custom Search";
    QString default_url = "Search for \"@totora\" on your desired site and copy and paste the URL of the result page here";
    ui->searchEngineItems->addItem(default_title);

    searchEngines.push_back(std::make_pair(searchEngine{default_title, default_url, {}}, false));
    ui->searchEngineItems->setCurrentItem(ui->searchEngineItems->item(ui->searchEngineItems->count() - 1));

    QString status = searchEngines[ui->searchEngineItems->currentRow()].second ? "disable" : "enable";
    ui->enableButton->setText(status);

    ui->titleLineEdit->setText(searchEngines[ui->searchEngineItems->count() - 1].first.title);
    ui->urlLineEdit->setText(searchEngines[ui->searchEngineItems->count() - 1].first.url);
    ui->firstHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->count() - 1].first.hotKey[0]));
    ui->secondHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->count() - 1].first.hotKey[1]));
    ui->thirdHotKey->setKeySequence(QKeySequence(searchEngines[ui->searchEngineItems->count() - 1].first.hotKey[2]));
}

void CustomSearchs::on_turnUpButton_clicked()
{
    if (ui->searchEngineItems->currentRow() <= 0)
        return ;
    std::swap(searchEngines[ui->searchEngineItems->currentRow()], searchEngines[ui->searchEngineItems->currentRow() - 1]);

    QString titleDown = ui->searchEngineItems->currentItem()->text();
    QString titleUp = ui->searchEngineItems->item(ui->searchEngineItems->currentRow() - 1)->text();

    ui->searchEngineItems->currentItem()->setText(titleUp);
    ui->searchEngineItems->item(ui->searchEngineItems->currentRow() - 1)->setText(titleDown);
    ui->searchEngineItems->setCurrentItem(ui->searchEngineItems->item(ui->searchEngineItems->currentRow() - 1));

    QString status = searchEngines[ui->searchEngineItems->currentRow()].second ? "disable" : "enable";
    ui->enableButton->setText(status);

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_turnDownButton_clicked()
{
    if (ui->searchEngineItems->currentRow() >= ui->searchEngineItems->count() - 1)
        return ;
    std::swap(searchEngines[ui->searchEngineItems->currentRow()], searchEngines[ui->searchEngineItems->currentRow() + 1]);

    QString titleUp = ui->searchEngineItems->currentItem()->text();
    QString titleDown = ui->searchEngineItems->item(ui->searchEngineItems->currentRow() + 1)->text();

    ui->searchEngineItems->currentItem()->setText(titleDown);
    ui->searchEngineItems->item(ui->searchEngineItems->currentRow() + 1)->setText(titleUp);
    ui->searchEngineItems->setCurrentItem(ui->searchEngineItems->item(ui->searchEngineItems->currentRow() + 1));

    QString status = searchEngines[ui->searchEngineItems->currentRow()].second ? "disable" : "enable";
    ui->enableButton->setText(status);

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_titleLineEdit_editingFinished()
{
    if (ui->titleLineEdit->hasFocus())
    {
        return ;
    }
    QString current_title = ui->titleLineEdit->text();
    if (current_title != searchEngines[ui->searchEngineItems->currentRow()].first.title)
    {
        QString oldTitle = ui->searchEngineItems->currentItem()->text();
        ui->searchEngineItems->currentItem()->setText(oldTitle.replace(
                                      searchEngines[ui->searchEngineItems->currentRow()].first.title, current_title));
        searchEngines[ui->searchEngineItems->currentRow()].first.title = current_title;
    }

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_urlLineEdit_editingFinished()
{
    if (ui->urlLineEdit->hasFocus())
    {
        return ;
    }
    QString current_url = ui->urlLineEdit->text();
    if (current_url != searchEngines[ui->searchEngineItems->currentRow()].first.url ||
            current_url.count() > 0 || current_url.contains("@totora", Qt::CaseSensitive))
    {
        searchEngines[ui->searchEngineItems->currentRow()].first.url = current_url;
    }

    isNeedStore = true;
}

void CustomSearchs::on_firstHotKey_keySequenceChanged(const QKeySequence &keySequence)
{
    QString key = keySequence.toString();
    if (key.count() > 1 || key.count() < 0)
    {
        return ;
    }
    searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[0] = key[0];

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_secondHotKey_keySequenceChanged(const QKeySequence &keySequence)
{
    QString key = keySequence.toString();
    if (key.count() > 1 || key.count() < 0)
    {
        return ;
    }
    searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[1] = key[0];

    isNeedStore = true;
    emit updateSearchEngines();
}

void CustomSearchs::on_thirdHotKey_keySequenceChanged(const QKeySequence &keySequence)
{
    QString key = keySequence.toString();
    if (key.count() > 1 || key.count() < 0)
    {
        return ;
    }
    searchEngines[ui->searchEngineItems->currentRow()].first.hotKey[2] = key[0];

    isNeedStore = true;
    emit updateSearchEngines();
}

bool CustomSearchs::loadJsonFile(QString filePath, QJsonDocument &jsonDoc)
{
    QFile loadFile(filePath);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "couldn't open json file " << filePath;
        return false;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError json_error;
    jsonDoc = QJsonDocument::fromJson(allData, &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error in " << filePath;
        return false;
    }
    return true;
}

void CustomSearchs::on_enableButton_clicked()
{
    QString current_text = ui->enableButton->text();
    if (current_text == "enable")
    {
        searchEngines[ui->searchEngineItems->currentRow()].second = true;
        ui->enableButton->setText("disable");
        ui->searchEngineItems->currentItem()->setText(searchEngines[ui->searchEngineItems->currentRow()].first.title);
    }
    else if (current_text == "disable")
    {
        searchEngines[ui->searchEngineItems->currentRow()].second = false;
        ui->enableButton->setText("enable");

        QString title = searchEngines[ui->searchEngineItems->currentRow()].first.title;
        int disable_icon = ui->searchEngineItems->fontMetrics().width("ø");
        int title_width = ui->searchEngineItems->fontMetrics().width(title);
        for (int j = 0;j < (ui->searchEngineItems->width() - title_width - disable_icon) /
             ui->searchEngineItems->fontMetrics().width(" ");++j)
        {
            title += " ";
        }
        title += "ø";
        ui->searchEngineItems->currentItem()->setText(title);
    }

    isNeedStore = true;
    emit updateSearchEngines();
}
