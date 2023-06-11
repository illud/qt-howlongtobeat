
#include "howlongtobeat.h"
#include "ui_howlongtobeat.h"
#include <curl/curl.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <sstream>

HowLongTobeat::HowLongTobeat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HowLongTobeat)
{
    ui->setupUi(this);
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
    //QVector<QString> splitWords = RemoveDupWord(ui->lineEdit->text().toStdString());
    //qDebug() << splitWords;

    CURL *curl;
    CURLcode res;
    struct curl_slist *header;
    std::string readBuffer;
    std::string jsonstr = "{\"searchType\":\"games\",\"searchTerms\":[\""+ui->lineEdit->text().toStdString()+"\"],\"searchPage\":1,\"size\":20,\"searchOptions\":{\"games\":{\"userId\":279067,\"platform\":\"\",\"sortCategory\":\"popular\",\"rangeCategory\":\"main\",\"rangeTime\":{\"min\":null,\"max\":null},\"gameplay\":{\"perspective\":\"\",\"flow\":\"\",\"genre\":\"\"},\"rangeYear\":{\"min\":\"\",\"max\":\"\"},\"modifier\":\"\"},\"users\":{\"sortCategory\":\"postcount\"},\"lists\":{\"sortCategory\":\"follows\"},\"filter\":\"\",\"sort\":0,\"randomizer\":0}}";

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
    int currentRow = 0;
    for (int var = 0; var < jsonArray.count(); ++var) {
        ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem("https://howlongtobeat.com/games/" + jsonArray[var].toObject()["game_image"].toString()));
        ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(jsonArray[var].toObject()["game_name"].toString()));
        ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_main"].toInt())));
        ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_plus"].toInt())));
        ui->tableWidget->setItem(currentRow, 4, new QTableWidgetItem(secondsToTime(jsonArray[var].toObject()["comp_100"].toInt())));

        currentRow = currentRow + 1;
    }
}

