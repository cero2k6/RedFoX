#include "frmfacturas.h"
#include "ui_frmfacturas.h"
#include "factura.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QErrorMessage>
#include<QSqlError>
#include<QDate>
#include <QSqlQueryModel>
#include <QDebug>
#include "frmbuscarcliente.h"
#include <QTableView>
#include <QHeaderView>
#include "articulo.h"
#include "cliente.h"


Factura *oFactura = new Factura();
frmFacturas::frmFacturas(Configuracion *o_config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmFacturas)
{
    o_configuracion =o_config;
    ui->setupUi(this);
    // Pongo valores por defecto
    ui->lblContabilizada->setVisible(false);
    ui->lblFacturaCobrada->setVisible(false);
    ui->lblFacturaImpresa->setVisible(false);
    // Rellenar formas de pago
    modelFP = new QSqlQueryModel();
    modelFP->setQuery("Select cFormaPago,id from FormPago",QSqlDatabase::database("empresa"));
    ui->txtcFormaPago->setModel(modelFP);
    // valores edicion
    this->Altas = false;
    ui->txtcCodigoArticulo->setFocus();
}

frmFacturas::~frmFacturas()
{
    delete ui;
}

void frmFacturas::lineasVentas()
{
    // lineas de ventas
    QString cSQL;
    QString cId;
    cId = cId.number(oFactura->Getid());
     cSQL ="select id,cCodigo,nCantidad,cDescripcion,rPvp,rSubtotal,nDto,rDto,rTotal, nPorcIva from lin_fac "
             " where id_Cab = "+cId;
     ModelLin_fac = new QSqlQueryModel();
     ModelLin_fac->setQuery(cSQL,QSqlDatabase::database("empresa"));
     ui->Lineas->setModel(ModelLin_fac);
      //Creamos Objeto de la clase Cabecera para las cabeceras horizontales
     Cabecera = new QHeaderView(Qt::Horizontal,this);
     // Le decimos a nuestro objeto QTableView  que use la instancia de QHeaderView que acabamos de crear.
     ui->Lineas->setHorizontalHeader(Cabecera);
     /*Ponemos el tamaño deseado para cada columna, teniendo en cuenta que la primera columna es la "0". (en nuestro caso está oculta ya que muestra el id de la tabla y esto no nos interesa que lo vea el usuario */
     Cabecera->setResizeMode(0,QHeaderView::Fixed);
     Cabecera->resizeSection(0,0);
     Cabecera->setResizeMode(1,QHeaderView::Fixed);
     Cabecera->resizeSection(1,85);
     Cabecera->setResizeMode(2,QHeaderView::Fixed);
     Cabecera->resizeSection(2,65);
     Cabecera->setResizeMode(3,QHeaderView::Fixed);
     Cabecera->resizeSection(3,213);
     Cabecera->setResizeMode(4,QHeaderView::Fixed);
     Cabecera->resizeSection(4,120);
     Cabecera->setResizeMode(9,QHeaderView::Fixed);
     Cabecera->resizeSection(9,35);
     ModelLin_fac->setHeaderData(1, Qt::Horizontal, QObject::tr("CÓDIGO"));
     ModelLin_fac->setHeaderData(2, Qt::Horizontal, QObject::tr("CANTIDAD"));
     ModelLin_fac->setHeaderData(3, Qt::Horizontal, QObject::tr("DESCRIPCIÓN"));
     ModelLin_fac->setHeaderData(4, Qt::Horizontal, QObject::tr("PVP"));
     ModelLin_fac->setHeaderData(5, Qt::Horizontal, QObject::tr("SUBTOTAL"));
     ModelLin_fac->setHeaderData(6, Qt::Horizontal, QObject::tr("%DTO"));
     ModelLin_fac->setHeaderData(7, Qt::Horizontal, QObject::tr("DTO"));
     ModelLin_fac->setHeaderData(8, Qt::Horizontal, QObject::tr("TOTAL"));
     ModelLin_fac->setHeaderData(9, Qt::Horizontal, QObject::tr("%IVA"));

    // hacemos visible la cabecera
     Cabecera->setVisible(true);
     ui->Lineas->setItemDelegateForColumn(1, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(2, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(3, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(4, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(5, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(6, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(7, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(8, new ColumnaGrid(this));
     ui->Lineas->setItemDelegateForColumn(9, new ColumnaGrid(this));
}

ColumnaGrid::ColumnaGrid(QObject *parent)
    :QItemDelegate(parent)
{
    //Configuracion *o_configuracion = new Configuracion();
}

void ColumnaGrid::paint(QPainter *painter,
                 const QStyleOptionViewItem & option,
                 const QModelIndex & index) const
{

    QString texto = index.model()->data(index, Qt::DisplayRole).toString();
    int tamano, posDec;
    tamano = 0;
    posDec = 0;
    /* Verificamos el Index */
    if (index.column() == 4 || index.column() == 5 || index.column() == 7 || index.column() == 8) {
        QString ctexto = texto;
        // Cambio . por , o por signo elegido por los usuarios
        int tamano = ctexto.length();
        int posDec;
        posDec = tamano -3;
        if ((ctexto.midRef(posDec,1) !=".") && (ctexto.midRef(posDec,1) != ",") &&  (ctexto.midRef((posDec+1),1) !=".")
                &&  (ctexto.midRef((posDec+1),1) !=".")) {

            ctexto.append(",00") ;
        } else {
            if ((ctexto.midRef((posDec+1),1) =="."))
                ctexto.append("0");
        }

        ctexto.replace(".",",");
        //qDebug() << "Tamaño: " << tamano << "PosDec" <<posDec << ctexto.midRef(posDec,1);

        if (ctexto.length()==14) {
            ctexto.insert(2,".");
            ctexto.insert(6,".");
            ctexto.insert(10,".");
        }
        if (ctexto.length()==13) {
            ctexto.insert(1,".");
            ctexto.insert(5,".");
            ctexto.insert(9,".");
        }
        if (ctexto.length()==12) {
            ctexto.insert(3,".");
            ctexto.insert(7,".");
        }
        if (ctexto.length()==11) {
            ctexto.insert(2,".");
            ctexto.insert(6,".");
        }
        if (ctexto.length()== 10) {
            ctexto.insert(1,".");
            ctexto.insert(5,".");
        }
        if (ctexto.length() == 9 ) {

            ctexto.insert(3, ".");
        }
        if (ctexto.length() == 8 ) {

            ctexto.insert(2, ".");
        }
        if (ctexto.length() == 7 ) {

            ctexto.insert(1, ".");
        }
        texto = ctexto;
    }
    QStyleOptionViewItem myOption = option;
    myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    drawDisplay(painter, myOption, myOption.rect,texto);
    drawFocus(painter, myOption, myOption.rect);
}


void frmFacturas::LLenarCampos() {
    int lEstado;
    ui->txtcCodigoCliente->setText(oFactura->getcCodigoCliente());
    ui->txtcFactura->setText(oFactura->getcFactura());
    ui->txtdFecha->setDate(oFactura->getdFecha());
    ui->txtdFechaCobro->setDate(oFactura->getdFechaCobro());
    ui->txtcCliente->setText(oFactura->getcCliente());
    ui->txtcDireccion->setText(oFactura->getcDireccion());
    ui->txtcDireccion2->setText(oFactura->getcDireccion2());
    ui->txtcCp->setText(oFactura->getcCp());
    ui->txtcPoblacion->setText(oFactura->getcPoblacion());
    ui->txtcProvincia->setText(oFactura->getcProvincia());
    ui->txtcPais->setText(oFactura->getcPais());
    ui->txtcCif->setText(oFactura->getcCif());
     lEstado = oFactura->getlRecargoEquivalencia();
    if ((lEstado= 1)) {
        ui->chklRecargoEquivalencia->setChecked(true);
    } else {
        ui->chklRecargoEquivalencia->setChecked(false);
    }
    ui->txtrSubtotal->setText(o_configuracion->FormatoNumerico( QString::number(oFactura->getrSubtotal(),'f',2)));
    ui->txtnDto->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnDto(),'f',2)));
    ui->txtnDtoPP->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnDtoPP(),'f',2)));
    ui->txtrImporteDescuento->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteDescuento(),'f',2)));
    ui->txtrImporteDescuentoPP->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteDescuentoPP(),'f',2)));
    ui->txtrBase->setText(o_configuracion->FormatoNumerico(QString::number( oFactura->getrBase(),'f',2)));
    ui->txtnIva_2->setText(o_configuracion->FormatoNumerico(QString::number( oFactura->getnIva(),'f',2)));
    ui->txtrImporteIva->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteIva(),'f',2)));
    ui->txtrTotal->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal(),'f',2)));
    lEstado = oFactura->getlImpresa();
    if((lEstado == 1)) {
        ui->lblFacturaImpresa->setVisible(true);
    } else {
        ui->lblFacturaImpresa->setVisible(false);
    }
    lEstado = oFactura->getlCobrada();
    if ((lEstado == 1)) {
        ui->lblFacturaCobrada->setVisible(true);
    } else {
        ui->lblFacturaCobrada->setVisible(false);
    }
    lEstado = oFactura->getlContablilizada();

    if((lEstado == 1)) {
        ui->lblContabilizada->setVisible(true);
    } else {
        ui->lblContabilizada->setVisible(false);
    }
    int indice=ui->txtcFormaPago->findText(oFactura->getcFormaPago());
    if(indice!=-1)
     ui->txtcFormaPago->setCurrentIndex(indice);
    ui->txttComentario->setText(oFactura->gettComentario());
    ui->txtrBase1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase1(),'f',2)));
    ui->txtrBase2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase2(),'f',2)));
    ui->txtrBase3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase3(),'f',2)));
    ui->txtrBase4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase4(),'f',2)));
    ui->txtnPorcentajeIva1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnPorcentajeIVA1(),'f',2)));
    ui->txtnPorcentajeIva2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnPorcentajeIVA2(),'f',2)));
    ui->txtnPorcentajeIva3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnPorcentajeIVA3(),'f',2)));
    ui->txtnPorcentajeIva4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnPorcentajeIVA4(),'f',2)));
    ui->txtrIVA1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA1(),'f',2)));
    ui->txtrIVA2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA2(),'f',2)));
    ui->txtrIVA3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA3(),'f',2)));
    ui->txtrIVA4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA4(),'f',2)));
    ui->txtrTotal1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal1(),'f',2)));
    ui->txtrTotal2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal2(),'f',2)));
    ui->txtrTotal3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal3(),'f',2)));
    ui->txtrTotal4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal4(),'f',2)));
    ui->txtnRec1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnRec1(),'f',2)));
    ui->txtnRec2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnRec2(),'f',2)));
    ui->txtnRec3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnRec3(),'f',2)));
    ui->txtnRec4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getnRec4(),'f',2)));
    ui->txtrRecargoEq1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq1(),'f',2)));
    ui->txtrRecargoEq2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq2(),'f',2)));
    ui->txtrRecargoEq3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq3(),'f',2)));
    ui->txtrRecargoEq4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq4(),'f',2)));
    ui->txtrTotalRecargoEq->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotalRecargoEq(),'f',2)));
    ui->txtrEntregadoaCuenta->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrEntregadoaCuenta(),'f',2)));
    ui->txtrImportePendiente->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImportePendiente(),'f',2)));
    ui->txtcCodigoEntidad->setText(oFactura->getcCodigoEntidad());
    ui->txtcOficinaEntidad->setText(oFactura->getcOficinaEntidad());
    ui->txtcDCCuenta->setText(oFactura->getcDCCuenta());
    ui->txtcNumeroCuenta->setText(oFactura->getcNumeroCuenta());
    ui->txtcPedidoCliente->setText(oFactura->getcPedidoCliente());
    // cargamos líneas de ventas
   lineasVentas();


}

void frmFacturas::VaciarCampos() {
    QDate dFecha;
    ui->txtcCodigoCliente->setText("");
    ui->txtcFactura->setText("");
    ui->txtdFecha->setDate(dFecha.currentDate());
    ui->txtdFechaCobro->setDate(dFecha.currentDate());
    ui->txtcCliente->setText("");
    ui->txtcDireccion->setText("");
    ui->txtcDireccion2->setText("");
    ui->txtcCp->setText("");
    ui->txtcPoblacion->setText("");
    ui->txtcProvincia->setText("");
    ui->txtcPais->setText("");
    ui->txtcCif->setText("");
    ui->chklRecargoEquivalencia->setChecked(false);
    ui->txtrSubtotal->setText(0);
    ui->txtnDto->setText(0);
    ui->txtnDtoPP->setText(0);
    ui->txtrImporteDescuento->setText("0,00");
    ui->txtrImporteDescuentoPP->setText("0,00");
    ui->txtrBase->setText("0,00");
    ui->txtnIva_2->setText(0);
    ui->txtrImporteIva->setText("0,00");
    ui->txtrTotal->setText("0,00");
    ui->lblFacturaImpresa->setVisible(false);
    ui->lblFacturaCobrada->setVisible(false);
    ui->lblContabilizada->setVisible(false);
    int indice=ui->txtcFormaPago->findText("Contado");
    if(indice!=-1) ui->txtcFormaPago->setCurrentIndex(indice);
    ui->txttComentario->setText("");
    ui->txtrBase1->setText(0);
    ui->txtrBase2->setText(0);
    ui->txtrBase3->setText(0);
    ui->txtrBase4->setText(0);
    QSqlQuery tiposiva(QSqlDatabase::database("empresa"));
    // IVA NORMAL
    tiposiva.prepare("Select nIva from tiposiva where cTipo = 'NORMAL'");
    if (!tiposiva.exec()) {
            QMessageBox::critical(0, "error:", tiposiva.lastError().text());
    } else {
        tiposiva.next();
        ui->txtnPorcentajeIva1->setText(QString::number(tiposiva.value(0).toInt()));
    }
    // IVA REDUCIDO
    tiposiva.prepare("Select nIva from tiposiva where cTipo = 'REDUCIDO'");
    if (!tiposiva.exec()) {
            QMessageBox::critical(0, "error:", tiposiva.lastError().text());
    } else {
        tiposiva.next();
        ui->txtnPorcentajeIva2->setText(QString::number(tiposiva.value(0).toInt()));
    }
    // IVA SUPER-REDUCIDO
    tiposiva.prepare("Select nIva from tiposiva where cTipo = 'SUPER-REDUCIDO'");
    if (!tiposiva.exec()) {
            QMessageBox::critical(0, "error:", tiposiva.lastError().text());
    } else {
        tiposiva.next();
        ui->txtnPorcentajeIva3->setText(QString::number(tiposiva.value(0).toInt()));
    }
    // IVA EXCENTO
    tiposiva.prepare("Select nIva from tiposiva where cTipo = 'EXENTO'");
    if (!tiposiva.exec()) {
            QMessageBox::critical(0, "error:", tiposiva.lastError().text());
    } else {
        tiposiva.next();
        ui->txtnPorcentajeIva4->setText(QString::number(tiposiva.value(0).toInt()));
    }

    ui->txtrIVA1->setText(0);
    ui->txtrIVA2->setText(0);
    ui->txtrIVA3->setText(0);
    ui->txtrIVA4->setText(0);
    ui->txtrTotal1->setText(0);
    ui->txtrTotal2->setText(0);
    ui->txtrTotal3->setText(0);
    ui->txtrTotal4->setText(0);
    ui->txtnRec1->setText(0);
    ui->txtnRec2->setText(0);
    ui->txtnRec3->setText(0);
    ui->txtnRec4->setText(0);
    ui->txtrRecargoEq1->setText(0);
    ui->txtrRecargoEq2->setText(0);
    ui->txtrRecargoEq3->setText(0);
    ui->txtrRecargoEq4->setText(0);
    ui->txtrTotalRecargoEq->setText(0);
    ui->txtrEntregadoaCuenta->setText("0,00");
    ui->txtrImportePendiente->setText("0,00");
    ui->txtcCodigoEntidad->setText("");
    ui->txtcOficinaEntidad->setText("");
    ui->txtcDCCuenta->setText("");
    ui->txtcNumeroCuenta->setText("");
    ui->txtcPedidoCliente->setText("");
}

void frmFacturas::BloquearCampos()
{
    QList<QLineEdit *> lineEditList = this->findChildren<QLineEdit *>();
    QLineEdit *lineEdit;
    foreach (lineEdit, lineEditList) {
        lineEdit->setReadOnly(true);
        //qDebug() << lineEdit->objectName();
    }
    // ComboBox
    QList<QComboBox *> ComboBoxList = this->findChildren<QComboBox *>();
    QComboBox *ComboBox;
    foreach (ComboBox, ComboBoxList) {
        ComboBox->setEnabled(false);
        //qDebug() << lineEdit->objectName();
    }
//    // SpinBox
//    QList<QSpinBox *> SpinBoxList = this->findChildren<QSpinBox *>();
//    QSpinBox *SpinBox;
//    foreach (SpinBox, SpinBoxList) {
//        SpinBox->setReadOnly(true);
//        //qDebug() << lineEdit->objectName();
//    }
//    // DoubleSpinBox
//    QList<QDoubleSpinBox *> DSpinBoxList = this->findChildren<QDoubleSpinBox *>();
//    QDoubleSpinBox *DSpinBox;
//    foreach (DSpinBox, DSpinBoxList) {
//        DSpinBox->setReadOnly(true);
//        //qDebug() << lineEdit->objectName();
//    }
    // CheckBox
    QList<QCheckBox *> CheckBoxList = this->findChildren<QCheckBox *>();
    QCheckBox *CheckBox;
    foreach (CheckBox, CheckBoxList) {
        CheckBox->setEnabled(false);
        //qDebug() << lineEdit->objectName();
    }
    // QTextEdit
    QList<QTextEdit *> textEditList = this->findChildren<QTextEdit *>();
    QTextEdit *textEdit;
    foreach (textEdit,textEditList) {
        textEdit->setReadOnly(true);
        //qDebug() << lineEdit->objectName();
    }
    // QDateEdit
    QList<QDateEdit *> DateEditList = this->findChildren<QDateEdit *>();
    QDateEdit *DateEdit;
    foreach (DateEdit, DateEditList) {
        DateEdit->setEnabled(false);
        //qDebug() << lineEdit->objectName();
    }

    ui->btnAnadir->setEnabled(true);
    ui->btnAnterior->setEnabled(true);
   // ui->btnBorrar->setEnabled(true);
    ui->btnBuscar->setEnabled(true);
    ui->btnCerrar->setEnabled(true);
    ui->btnDeshacer->setEnabled(false);
    ui->btnEditar->setEnabled(true);
    ui->btnGuardar->setEnabled(false);
    ui->btnSiguiente->setEnabled(true);

}

void frmFacturas::DesbloquearCampos()
{
    // LineEdit
    QList<QLineEdit *> lineEditList = this->findChildren<QLineEdit *>();
    QLineEdit *lineEdit;
    foreach (lineEdit, lineEditList) {
        lineEdit->setReadOnly(false);
        //qDebug() << lineEdit->objectName();
    }
    // ComboBox
    QList<QComboBox *> ComboBoxList = this->findChildren<QComboBox *>();
    QComboBox *ComboBox;
    foreach (ComboBox, ComboBoxList) {
        ComboBox->setEnabled(true);
        //qDebug() << lineEdit->objectName();
    }
//    // SpinBox
//    QList<QSpinBox *> SpinBoxList = this->findChildren<QSpinBox *>();
//    QSpinBox *SpinBox;
//    foreach (SpinBox, SpinBoxList) {
//        SpinBox->setReadOnly(false);
//        //qDebug() << lineEdit->objectName();
//    }
//    // DoubleSpinBox
//    QList<QDoubleSpinBox *> DSpinBoxList = this->findChildren<QDoubleSpinBox *>();
//    QDoubleSpinBox *DSpinBox;
//    foreach (DSpinBox, DSpinBoxList) {
//        DSpinBox->setReadOnly(false);
//        //qDebug() << lineEdit->objectName();
//    }
    // CheckBox
    QList<QCheckBox *> CheckBoxList = this->findChildren<QCheckBox *>();
    QCheckBox *CheckBox;
    foreach (CheckBox, CheckBoxList) {
        CheckBox->setEnabled(true);
        //qDebug() << lineEdit->objectName();
    }
    // QTextEdit
    QList<QTextEdit *> textEditList = this->findChildren<QTextEdit *>();
    QTextEdit *textEdit;
    foreach (textEdit,textEditList) {
        textEdit->setReadOnly(false);
        //qDebug() << lineEdit->objectName();
    }
    // QDateEdit
    QList<QDateEdit *> DateEditList = this->findChildren<QDateEdit *>();
    QDateEdit *DateEdit;
    foreach (DateEdit, DateEditList) {
        DateEdit->setEnabled(true);
        //qDebug() << lineEdit->objectName();
    }
    ui->btnAnadir->setEnabled(false);
    ui->btnAnterior->setEnabled(false);
   // ui->btnBorrar->setEnabled(false);
    ui->btnBuscar->setEnabled(false);
    ui->btnCerrar->setEnabled(false);
    ui->btnDeshacer->setEnabled(true);
    ui->btnEditar->setEnabled(false);
    ui->btnGuardar->setEnabled(true);
    ui->btnSiguiente->setEnabled(false);
}

void frmFacturas::LLenarFactura() {
    oFactura->setcCodigoCliente(ui->txtcCodigoCliente->text());
    oFactura->setcFactura(ui->txtcFactura->text());
    oFactura->setdFecha(ui->txtdFecha->date());
    oFactura->setdFechaCobro(ui->txtdFechaCobro->date());
    oFactura->setcCliente(ui->txtcCliente->text());
    oFactura->setcDireccion(ui->txtcDireccion->text());
    oFactura->setcDireccion2(ui->txtcDireccion2->text());
    oFactura->setcCp(ui->txtcCp->text());
    oFactura->setcPoblacion(ui->txtcPoblacion->text());
    oFactura->setcProvincia(ui->txtcProvincia->text());
    oFactura->setcPais(ui->txtcPais->text());
    oFactura->setcCif(ui->txtcCif->text());
    if (ui->chklRecargoEquivalencia->isChecked()) {
        oFactura->setlRecargoEquivalencia(1);
    } else {
        oFactura->setlRecargoEquivalencia(0);
    }
    oFactura->setrSubtotal(ui->txtrSubtotal->text().toDouble());
    oFactura->setnDto(ui->txtnDto->text().toDouble());
    oFactura->setnDtoPP(ui->txtnDtoPP->text().toDouble());
    oFactura->setrImporteDescuento(ui->txtrImporteDescuento->text().toDouble());
    oFactura->setrImporteDescuentoPP(ui->txtrImporteDescuentoPP->text().toDouble());
    oFactura->setrBase(ui->txtrBase->text().toDouble());
    oFactura->setrImporteIva(ui->txtrImporteIva->text().toDouble());
    oFactura->setrTotal(ui->txtrTotal->text().toDouble());
    oFactura->setcFormaPago(ui->txtcFormaPago->currentText());
    oFactura->settComentario(ui->txttComentario->toPlainText());
    oFactura->setrBase1(ui->txtrBase1->text().toDouble());
    oFactura->setrBase2(ui->txtrBase2->text().toDouble());
    oFactura->setrBase3(ui->txtrBase3->text().toDouble());
    oFactura->setrBase4(ui->txtrBase4->text().toDouble());
    oFactura->setnPorcentajeIVA1(ui->txtnPorcentajeIva1->text().toDouble());
    oFactura->setnPorcentajeIVA2(ui->txtnPorcentajeIva2->text().toDouble());
    oFactura->setnPorcentajeIVA3(ui->txtnPorcentajeIva3->text().toDouble());
    oFactura->setnPorcentajeIVA4(ui->txtnPorcentajeIva4->text().toDouble());
    oFactura->setrIVA1(ui->txtrIVA1->text().toDouble());
    oFactura->setrIVA2(ui->txtrIVA2->text().toDouble());
    oFactura->setrIVA3(ui->txtrIVA3->text().toDouble());
    oFactura->setrIVA4(ui->txtrIVA4->text().toDouble());
    oFactura->setrTotal1(ui->txtrTotal1->text().toDouble());
    oFactura->setrTotal2(ui->txtrTotal2->text().toDouble());
    oFactura->setrTotal3(ui->txtrTotal3->text().toDouble());
    oFactura->setrTotal4(ui->txtrTotal4->text().toDouble());
    oFactura->setnRec1(ui->txtnRec1->text().toDouble());
    oFactura->setnRec2(ui->txtnRec2->text().toDouble());
    oFactura->setnRec3(ui->txtnRec3->text().toDouble());
    oFactura->setnRec4(ui->txtnRec4->text().toDouble());
    oFactura->setrRecargoEq1(ui->txtrRecargoEq1->text().toDouble());
    oFactura->setrRecargoEq2(ui->txtrRecargoEq2->text().toDouble());
    oFactura->setrRecargoEq3(ui->txtrRecargoEq3->text().toDouble());
    oFactura->setrRecargoEq4(ui->txtrRecargoEq4->text().toDouble());
    oFactura->setrTotalRecargoEq(ui->txtrTotalRecargoEq->text().toDouble());
    oFactura->setrEntregadoaCuenta(ui->txtrEntregadoaCuenta->text().toDouble());
    oFactura->setrImportePendiente(ui->txtrImportePendiente->text().toDouble());
    oFactura->setcCodigoEntidad(ui->txtcCodigoEntidad->text());
    oFactura->setcOficinaEntidad(ui->txtcOficinaEntidad->text());
    oFactura->setcDCCuenta(ui->txtcDCCuenta->text());
    oFactura->setcNumeroCuenta(ui->txtcNumeroCuenta->text());
    oFactura->setcPedidoCliente(ui->txtcPedidoCliente->text());
}

void frmFacturas::on_btnSiguiente_clicked()
{
    QString cId = QString::number(oFactura->Getid());
    oFactura->RecuperarFactura("Select * from cab_fac where id >"+cId+" order by id limit 0,1 ");
    LLenarCampos();

}
void frmFacturas::on_btnAnterior_clicked()
{
    QString cId = QString::number(oFactura->Getid());
    oFactura->RecuperarFactura("Select * from cab_fac where id <"+cId+" order by id desc limit 0,1 ");
    LLenarCampos();
}


void frmFacturas::on_btnGuardar_clicked()
{

    LLenarFactura();
    BloquearCampos();
    if(this->Altas) {
       this->Altas = false;
       oFactura->NuevoNumeroFactura();
       oFactura->AnadirFactura();
    } else {

        oFactura->GuardarFactura(oFactura->Getid());
    }
    //bloquearCampos();
}

void frmFacturas::on_btnAnadir_clicked()
{
    this->Altas = true;
    DesbloquearCampos();
    VaciarCampos();
    ui->txtcCodigoCliente->setFocus();

}


void frmFacturas::on_btnEditar_clicked()
{
    DesbloquearCampos();
}



void frmFacturas::on_txtPVPArticulo_lostFocus()
{
    if (!ui->txtPVPArticulo->text().isEmpty()) {
        bool ok;
        ok = o_configuracion->EsNumero(ui->txtPVPArticulo->text());
        if(!ok) {
           QMessageBox::critical(NULL,tr("Entrada de Importe"),
                                 tr("No puede entrar letras en un campo monetario. \n"
                                    "Sólo se aceptan los valores  (-)  (0123456789) (,)  (.)  "),tr("&Aceptar"));
           ui->txtPVPArticulo->setText("0,00");
           ui->txtPVPArticulo->setSelection(0,4);
           ui->txtPVPArticulo->setFocus();
         } else {
            ui->txtPVPArticulo->setText(o_configuracion->FormatoNumerico(ui->txtPVPArticulo->text()) );
        }
        calcularTotalLinea();
    }
}



void frmFacturas::on_txtcCodigoArticulo_lostFocus()
{
    if (!ui->txtcCodigoArticulo->text().isEmpty()) {
        if (ui->txtDescripcionArticulo->text().isEmpty()) {
            Articulo *oArt =  new Articulo();
            Cliente *oCliente = new Cliente();
            oArt->Recuperar("Select * from articulos where cCodigo = '"+ui->txtcCodigoArticulo->text()+"'");
            ui->btnAnadirLinea->setToolTip("Código: "+ oArt->getcCodigo()+"<br>Descripción: "+oArt->getcDescripcion()+
                                           "<br><b>Stock:</b><font color = 'red'>"+QString::number(oArt->getnStockReal())+"</color>");
            ui->txtcCodigoArticulo->setText(oArt->getcCodigo());
            ui->txtDescripcionArticulo->setText(oArt->getcDescripcion());
            ui->txtPVPArticulo->setText(o_configuracion->FormatoNumerico(QString::number(oArt->getrTarifa1(),'f',2)));
            ui->txtcCantidadArticulo->setText("1");
            ui->txtSubtotalArticulo->setText(o_configuracion->FormatoNumerico(QString::number(oArt->getrTarifa1(),'f',2)));
            // Recupero datos cliente para determinar descuento en factura
            oCliente->Recuperar("select * from clientes where id="+ QString::number(oFactura->getiId_Cliente()) );
            ui->txtPorcDtoArticulo->setText(QString::number(oCliente->getnPorcDtoCliente(),'f',0));
            // Asigno el descuento mayor seleccionando entre dto ficha artículo y descuento ficha cliente
            if (oArt->getrDto() > oCliente->getnPorcDtoCliente()) {
                ui->txtPorcDtoArticulo->setText(o_configuracion->FormatoNumerico(QString::number(oArt->getrDto(),'f',0)));
            }
            ui->txtPorcIVAArticulo->setText(QString::number(oArt->MostrarTipoIVA(oArt->getid_TipoIva()),'f',0));

        }
        calcularTotalLinea();
    }

}

void frmFacturas::on_txtcCantidadArticulo_lostFocus()
{
    double nSubtotal;
    nSubtotal = ui->txtcCantidadArticulo->text().toDouble() * ui->txtPVPArticulo->text().toDouble();
    ui->txtSubtotalArticulo->setText(o_configuracion->FormatoNumerico( QString::number(nSubtotal,'f',2)));
    calcularTotalLinea();
}

void frmFacturas::calcularTotalLinea()
{
    // Calculo totales línea
    double impDto,impTot,impSubtotal;
    impSubtotal = (ui->txtcCantidadArticulo->text().toDouble() * ui->txtPVPArticulo->text().toDouble());
    ui->txtSubtotalArticulo->setText(o_configuracion->FormatoNumerico(QString::number(impSubtotal,'f',2)));
    impDto = (ui->txtSubtotalArticulo->text().toDouble() * ui->txtPorcDtoArticulo->text().toDouble())/100 ;
    ui->txtDtoArticulo->setText(o_configuracion->FormatoNumerico(QString::number(impDto,'f',2)));
    impTot = ui->txtSubtotalArticulo->text().toDouble() - ui->txtDtoArticulo->text().toDouble();
    ui->txtTotalArticulo->setText(o_configuracion->FormatoNumerico(QString::number(impTot,'f',2)));

}


void frmFacturas::on_txtPorcDtoArticulo_lostFocus()
{
    calcularTotalLinea();
    ui->txtcCodigoArticulo->setFocus();
}


void frmFacturas::on_btnAnadirLinea_clicked()
{
    if (!ui->txtDescripcionArticulo->text().isEmpty()) {
        oFactura->AnadirLineaFactura(oFactura->Getid(),ui->txtcCodigoArticulo->text(),ui->txtcCantidadArticulo->text().toDouble(),
                                     ui->txtDescripcionArticulo->text(),ui->txtPVPArticulo->text().toDouble(),
                                     ui->txtSubtotalArticulo->text().toDouble(),ui->txtPorcDtoArticulo->text().toDouble(),
                                     ui->txtDtoArticulo->text().toDouble(),ui->txtTotalArticulo->text().toDouble(),
                                     ui->txtPorcIVAArticulo->text().toDouble());
        ui->txtcCodigoArticulo->setText("");
        ui->txtcCantidadArticulo->setText(0);
        ui->txtDescripcionArticulo->setText("");
        ui->txtPVPArticulo->setText(0);
        ui->txtSubtotalArticulo->setText(0);
        ui->txtPorcDtoArticulo->setText(0);
        ui->txtDtoArticulo->setText(0);
        ui->txtTotalArticulo->setText(0);
        ui->txtPorcIVAArticulo->setText(0);
        ui->txtcCodigoArticulo->setFocus();
    } else {
        QMessageBox::critical(NULL,tr("Insertar Línea"),tr("Debe especificar un artículo y una cantidad"),tr("&Aceptar"));
        ui->txtcCodigoArticulo->setFocus();
    }
    lineasVentas();
    // Calculo totales factura
    oFactura->calcularFactura();
    ui->txtrSubtotal->setText(o_configuracion->FormatoNumerico( QString::number(oFactura->getrSubtotal(),'f',2)));
    ui->txtrImporteDescuento->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteDescuento(),'f',2)));
    ui->txtrImporteDescuentoPP->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteDescuentoPP(),'f',2)));
    ui->txtrBase->setText(o_configuracion->FormatoNumerico(QString::number( oFactura->getrBase(),'f',2)));
    ui->txtnIva_2->setText(o_configuracion->FormatoNumerico(QString::number( oFactura->getnIva(),'f',2)));
    ui->txtrImporteIva->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrImporteIva(),'f',2)));
    ui->txtrTotal->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal(),'f',2)));
    ui->txtrBase1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase1(),'f',2)));
    ui->txtrBase2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase2(),'f',2)));
    ui->txtrBase3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase3(),'f',2)));
    ui->txtrBase4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrBase4(),'f',2)));
    ui->txtrIVA1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA1(),'f',2)));
    ui->txtrIVA2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA2(),'f',2)));
    ui->txtrIVA3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA3(),'f',2)));
    ui->txtrIVA4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrIVA4(),'f',2)));
    ui->txtrTotal1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal1(),'f',2)));
    ui->txtrTotal2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal2(),'f',2)));
    ui->txtrTotal3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal3(),'f',2)));
    ui->txtrTotal4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotal4(),'f',2)));
    ui->txtrRecargoEq1->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq1(),'f',2)));
    ui->txtrRecargoEq2->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq2(),'f',2)));
    ui->txtrRecargoEq3->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq3(),'f',2)));
    ui->txtrRecargoEq4->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrRecargoEq4(),'f',2)));
    ui->txtrTotalRecargoEq->setText(o_configuracion->FormatoNumerico(QString::number(oFactura->getrTotalRecargoEq(),'f',2)));

}

void frmFacturas::on_btnDeshacer_clicked()
{
    BloquearCampos();
    QString cId = QString::number(oFactura->Getid());
    oFactura->RecuperarFactura("Select * from cab_fac where id ="+cId+" order by id limit 0,1 ");
    LLenarCampos();
}

