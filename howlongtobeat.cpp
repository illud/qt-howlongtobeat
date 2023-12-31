
#include "howlongtobeat.h"
#include "ui_howlongtobeat.h"
#include <charconv>
#include <curl/curl.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <sstream>
#include "ImageUtil.h"

HowLongTobeat::HowLongTobeat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HowLongTobeat)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    // Fetch api at the start of the app
    HowLongTobeat::on_searchBtn_clicked();
}

HowLongTobeat::~HowLongTobeat()
{
    delete ui;
}

QString secondsToTime(int time){
    int h = time / 3600;
    int m = time % 3600 / 60;

    QString hours = QString::number(h);

    QString minutes = QString::number(m);

    return hours + "h " + minutes + "m";
}

QVector<QString> RemoveDupWord(std::string str)
{
    QVector<QString> words;
    // Used to split string around spaces.
    std::istringstream  ss(str);

    std::string word; // for storing each word

    // Traverse through all wordsF
    // while loop till we get
    // strings to store in string word
    while (ss >> word)
    {
        // print the read word
        words.push_back(word.c_str());
    }

    return words;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void HowLongTobeat::on_searchBtn_clicked()
{
    QVector<QString> splitWords = RemoveDupWord(ui->lineEdit->text().toStdString());

    // build a string by sequentially adding data to it.
    std::stringstream ss;

    for (int i = 0; i < splitWords.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << "\"" << splitWords[i].toStdString() << "\""; // Include the " symbols around each element
    }

    std::string searchTerms = ss.str();

    //qDebug() << searchTerms;

    //qDebug() << splitWords.data()->toStdString();
    //ui->lineEdit->text().toStdString()
    CURL *curl;
    CURLcode res;
    struct curl_slist *header;
    std::string readBuffer;
    std::string jsonstr =  "{\"searchType\": \"games\",\"searchTerms\": ["+searchTerms+"],\"searchPage\": 1,\"size\": 20,\"searchOptions\": { \"games\": {\"userId\": 0,\"platform\": \"\",\"sortCategory\": \"popular\",\"rangeCategory\": \"main\",\"rangeTime\": { \"min\": 0, \"max\": 0},\"gameplay\": { \"perspective\": \"\", \"flow\": \"\", \"genre\": \"\"},\"modifier\": \"\" }, \"users\": {\"sortCategory\": \"postcount\" }, \"filter\": \"\", \"sort\": 0, \"randomizer\": 0} }";
    //qDebug() << jsonstr;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {

        header = NULL;
        header = curl_slist_append(header, "Content-Type: application/json");
        header = curl_slist_append(header, "Accept: */*");
        header = curl_slist_append(header, "Origin: https://howlongtobeat.com");
        header = curl_slist_append(header, "Referer: https://howlongtobeat.com");
        header = curl_slist_append(header, "User-Agent: Mozilla/4.0 (Windows 7 6.1) Java/1.7.0_51");

        /* First set the URL that is about to receive our POST. This URL can
       just as well be an https:// URL if that is what should receive the
       data. */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.howlongtobeat.com/api/search");

        /* Now specify the POST data */
       //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonstr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonstr.length());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res == CURLE_OK)
            qDebug() << "Good" ;
        else
             qDebug() << curl_easy_strerror((CURLcode)res);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    //Parse JSON object
    QJsonDocument jsonResponse = QJsonDocument::fromJson(readBuffer.c_str());
    QJsonArray jsonArray = jsonResponse["data"].toArray();

    //Sets tableWidget row count
    ui->tableWidget->setRowCount(jsonArray.count());

    // Sets icon size
    ui->tableWidget->setIconSize(QSize(300, 300));

    int currentRow = 0;
    for (int var = 0; var < jsonArray.count(); ++var) {
        // Set the row height for a specific row
        ui->tableWidget->setRowHeight(var, 300);

        // Download image from url and set image as icon
        ImageUtil* imageUtil = new ImageUtil();
        imageUtil->loadFromUrl(QUrl("https://howlongtobeat.com/games/" + jsonArray[var].toObject()["game_image"].toString()));
        imageUtil->connect(imageUtil, &ImageUtil::loaded,
                           [=]() {
                               QImage image = imageUtil->image(); // Get the image from ImageUtil

                               // Convert QImage to QPixmap for display
                               QPixmap pixmap = QPixmap::fromImage(image);

                               // Create a QTableWidgetItem and set the image as its icon
                               QTableWidgetItem *imageItem = new QTableWidgetItem();

                               // Sets icon
                               imageItem->setIcon(QIcon(pixmap));

                               // Set the item in the table widget
                               ui->tableWidget->setItem(currentRow, 0, imageItem);
                           });

       // ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem("https://howlongtobeat.com/games/" + jsonArray[var].toObject()["game_image"].toString()));
        ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(jsonArray[var].toObject()["game_name"].toString()));
        ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_main"].toInt())));
        ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_plus"].toInt())));
        ui->tableWidget->setItem(currentRow, 4, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_100"].toInt())));

        currentRow = currentRow + 1;
    }
}

