#include "frmtarifas.h"
#include "ui_frmtarifas.h"

FrmTarifas::FrmTarifas(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmTarifas)
{
    ui->setupUi(this);
    QSqlQueryModel *modelo = new QSqlQueryModel(this);
    modelo->setQuery("Select descripcion from codigotarifa",QSqlDatabase::database("terra"));
    ui->listaTarifa->setModel(modelo);


    //-----------
    // CONEXIONES
    //-----------
    connect(ui->listaTarifa,SIGNAL(clicked(QModelIndex)),this,SLOT(cargarDatosTarifa(QModelIndex)));
}

FrmTarifas::~FrmTarifas()
{
    delete ui;
}

void FrmTarifas::cargarDatosTarifa(QModelIndex indice)
{
    QSqlQueryModel* modelo = (QSqlQueryModel*)ui->listaTarifa->model();
    QString tarifa = modelo->record(indice.row()).value("descripcion").toString();
    QSqlQuery qTarifa(QSqlDatabase::database("terra"));
    qTarifa.prepare("select * from codigotarifa where descripcion ='"+tarifa+"'");

    if(qTarifa.exec()){
        qTarifa.next();

        ui->txtCodTarifa->setText(qTarifa.record().field("codigo_tarifa").value().toString());
        ui->txtDescTarifa->setText(qTarifa.record().field("descripcion").value().toString());
        ui->txtPais->setText(Configuracion_global->Devolver_pais(qTarifa.record().field("id_pais").value().toInt()));
        ui->txtMoneda->setText(Configuracion_global->Devolver_moneda(qTarifa.record().field("id_monedas").value().toInt()));
        this->id_tarifa = qTarifa.record().field("id").value().toInt();
        this->id_pais = qTarifa.record().field("id_pais").value().toInt();
        this->id_moneda = qTarifa.record().field("id_monedas").value().toInt();
        this->codigoTarifa = qTarifa.record().field("codigo_tarifa").value().toString();
        this->margen = ui->spinMargen->value();
        this->margen_min = ui->spinMargenMinimo->value();
        this->pvp =ui->txtPVPDivisa->text().toDouble();

    }
}
