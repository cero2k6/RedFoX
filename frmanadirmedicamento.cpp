#include "frmanadirmedicamento.h"
#include "ui_frmanadirmedicamento.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QXmlStreamReader>
#include <QtSql>
#include "vademecum.h"
#include <QDomDocument>
#include <QDomElement>
#include <QStandardItemModel>


FrmAnadirMedicamento::FrmAnadirMedicamento(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmAnadirMedicamento)
{
    ui->setupUi(this);
}

FrmAnadirMedicamento::~FrmAnadirMedicamento()
{
    delete ui;
}

void FrmAnadirMedicamento::on_pushButton_clicked()
{
    QString cUrl = "http://demo.vademecumdata.es/vweb/xml/ws_drug/SearchByName?value="+
    ui->txtBuscar->text().trimmed();


    // Recupero valores conexión Vademecum
    QSettings settings("infint", "terra");
    QString cClave1 = settings.value("Clave1").toString();
    QString cClave2 = settings.value("Clave2").toString();

    cUrl = cUrl + "&id_ent=" + cClave1;



        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(finishedSlotMedicamento(QNetworkReply*)));

       manager->get(QNetworkRequest(QUrl(cUrl)));


}
void FrmAnadirMedicamento::finishedSlotMedicamento(QNetworkReply* reply)
{
    //qDebug()<<reply->readAll();
     QString data=(QString)reply->readAll();
     QString cXML = data;

     // Extract values from XML
     QDomDocument document("XmlDoc");
     document.setContent(cXML);

     QDomElement root = document.documentElement();
     if (root .tagName() != "object")
         qDebug("Bad root element.");

     QDomNode drugList = root.firstChild();

     QDomNode n = drugList.firstChild();

       QStringList list;
       list <<"NOMBRE"<<"ID"<<"DOSIFICACIÓN"<<"EMPAQUETADO";

     ui->tablamedicamentos->setColumnCount(4);
     ui->tablamedicamentos->setColumnWidth(0,400);
     ui->tablamedicamentos->setColumnWidth(1,0);
     ui->tablamedicamentos->setColumnWidth(2,450);
     ui->tablamedicamentos->setColumnWidth(3,200);
     ui->tablamedicamentos->setHorizontalHeaderLabels(list);



    int nrow = 0;
    int pos = 0;
     while (!n.isNull()) {
         if (n.isElement()) {
             QDomNodeList attributes = n.childNodes();

             for (int i = 0; i < attributes.count(); i ++) {
                 QDomElement e = attributes.at(i).toElement();

                 if (e.tagName() == "name_speciality") {
                     //medicamentos.append(e.text());
                     ui->tablamedicamentos->setRowCount(pos+1);
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::blue); // color de los items

                     ui->tablamedicamentos->setItem(pos,0,newItem);


                 }
                 if (e.tagName() == "id_speciality") {
                     //id.append(e.text());
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::red); // color de los items
                     ui->tablamedicamentos->setItem(pos,1,newItem);
                 }
                 if (e.tagName() == "dosage_form") {
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::red); // color de los items
                     ui->tablamedicamentos->setItem(pos,2,newItem);
                 }
                 if (e.tagName() == "package") {
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::green); // color de los items
                     ui->tablamedicamentos->setItem(pos,3,newItem);
                 }
             }

             pos++;

             //data.append(s);
         }
         n = n.nextSibling();
     }


     ui->textEdit->setText((cXML));

}
// BUSCAR MEDICAMENTO POR PATOLOGÍA

void FrmAnadirMedicamento::on_btnBuscarMedicamentoporPatologia_clicked()
{
   // ui->tablaPatologias->setCurrentCell(1,ui->tablaPatologias->currentRow(),NULL);
    QString cUrl = "http://demo.vademecumdata.es/vweb/xml/ws_indication/DrugsList?id_indication="+
    ui->tablaPatologias->currentItem()->text();


    // Recupero valores conexión Vademecum
    QSettings settings("infint", "terra");
    QString cClave1 = settings.value("Clave1").toString();
    QString cClave2 = settings.value("Clave2").toString();

    cUrl = cUrl + "&id_ent=" + cClave1;


        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(finishedSlotBuscarMedicamentoPorPatologia(QNetworkReply*)));

       manager->get(QNetworkRequest(QUrl(cUrl)));

}

void FrmAnadirMedicamento::finishedSlotBuscarMedicamentoPorPatologia(QNetworkReply* reply)
{
    //qDebug()<<reply->readAll();
     QString data=(QString)reply->readAll();
     QString cXML = data;

     // Extract values from XML
     QDomDocument document("XmlDoc");
     document.setContent(cXML);

     QDomElement root = document.documentElement();
     if (root .tagName() != "object")
         qDebug("Bad root element.");

     QDomNode drugList = root.firstChild();

     QDomNode n = drugList.firstChild();


     ui->tablamedicamentosPatologia->setColumnCount(3);
     ui->tablamedicamentosPatologia->setColumnWidth(0,400);
     ui->tablamedicamentosPatologia->setColumnWidth(1,0);
     ui->tablamedicamentosPatologia->setColumnWidth(2,450);
     ui->tablamedicamentosPatologia->setColumnWidth(3,200);

    int nrow = 0;
    int pos = 0;
     while (!n.isNull()) {
         if (n.isElement()) {
             QDomNodeList attributes = n.childNodes();

             for (int i = 0; i < attributes.count(); i ++) {
                 QDomElement e = attributes.at(i).toElement();

                 if (e.tagName() == "name_speciality") {
                     //medicamentos.append(e.text());
                     ui->tablamedicamentosPatologia->setRowCount(pos+1);
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::blue); // color de los items

                     ui->tablamedicamentosPatologia->setItem(pos,0,newItem);


                 }
                 if (e.tagName() == "id_speciality") {
                     //id.append(e.text());
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::red); // color de los items
                     ui->tablamedicamentosPatologia->setItem(pos,1,newItem);
                 }

                 if (e.tagName() == "package") {
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::green); // color de los items
                     ui->tablamedicamentosPatologia->setItem(pos,2,newItem);
                 }
             }

             pos++;

             //data.append(s);
         }
         n = n.nextSibling();
     }


     ui->textEditPatologia->setText((cXML));

}
// Buscar Patología.

void FrmAnadirMedicamento::on_btnBuscarPatologia_clicked()
{
    QString cUrl = "http://demo.vademecumdata.es/vweb/xml/ws_indication/SearchByName?value="+
    ui->txtBuscarPatologia->text().trimmed();


    // Recupero valores conexión Vademecum
    QSettings settings("infint", "terra");
    QString cClave1 = settings.value("Clave1").toString();
    QString cClave2 = settings.value("Clave2").toString();

    cUrl = cUrl + "&id_ent=" + cClave1;


        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(finishedSlotBuscarPatologia(QNetworkReply*)));

       manager->get(QNetworkRequest(QUrl(cUrl)));
}


void FrmAnadirMedicamento::finishedSlotBuscarPatologia(QNetworkReply* reply)
{
    //qDebug()<<reply->readAll();
     QString data=(QString)reply->readAll();
     QString cXML = data;

     // Extract values from XML
     QDomDocument document("XmlDoc");
     document.setContent(cXML);

     QDomElement root = document.documentElement();
     if (root .tagName() != "object")
         qDebug("Bad root element.");

     QDomNode drugList = root.firstChild();

     QDomNode n = drugList.firstChild();


     ui->tablaPatologias->setColumnCount(2);
     ui->tablaPatologias->setColumnWidth(0,200);
     ui->tablaPatologias->setColumnWidth(1,50);


    int nrow = 0;
    int pos = 0;
     while (!n.isNull()) {
         if (n.isElement()) {
             QDomNodeList attributes = n.childNodes();

             for (int i = 0; i < attributes.count(); i ++) {
                 QDomElement e = attributes.at(i).toElement();

                 if (e.tagName() == "name_indication") {
                     //medicamentos.append(e.text());
                     ui->tablaPatologias->setRowCount(pos+1);
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::blue); // color de los items

                     ui->tablaPatologias->setItem(pos,0,newItem);


                 }
                 if (e.tagName() == "id_indication") {
                     //id.append(e.text());
                     QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                     // para que los elementos no sean editables
                     newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
                     newItem->setTextColor(Qt::blue); // color de los items
                     ui->tablaPatologias->setItem(pos,1,newItem);
                 }

             }

             pos++;

             //data.append(s);
         }
         n = n.nextSibling();
     }


     ui->textEditPatologia->setText((cXML));

}
