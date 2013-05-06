#include "frmrecepcion_pedidos.h"
#include "ui_frmrecepcion_pedidos.h"
#include "Auxiliares/monetarydelegate.h"
#include "Almacen/frmeditar_tarifas.h"
#include "Busquedas/frmbuscarcie.h"


Frmrecepcion_pedidos::Frmrecepcion_pedidos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Frmrecepcion_pedidos)
{
    ui->setupUi(this);
    ui->txtFechaFin->setDate(QDate::currentDate());
}

Frmrecepcion_pedidos::~Frmrecepcion_pedidos()
{
    ui->tablaLineas->clear();
    delete ui;
}

void Frmrecepcion_pedidos::on_btnBuscar_clicked()
{
    QSqlQueryModel *modelPedidos = new QSqlQueryModel(this);
    QString consulta;
    if(ui->txtNumPedido->text().isEmpty())
    {
        int recibido;
        if(ui->chkPedientes->isChecked())
            recibido = 1;
        else
            recibido = 0;

        consulta = "select id,nPedido,cSerie,dFecha,cProveedor from ped_pro where dFecha >="+ui->txtFecha_ini->date().toString("yyMMdd")+
            " and dFecha <="+ui->txtFechaFin->date().toString("yyMMdd")+" and cProveedor like '"+ ui->txtproveedor->text()+
            "%' and lRecibidoCompleto =" + QString::number(recibido);
    } else
    {
        consulta = " select id,nPedido,cSerie,dFecha,cProveedor from ped_pro where nPedido = "+ui->txtNumPedido->text();
    }

    modelPedidos->setQuery(consulta,QSqlDatabase::database("empresa"));
    ui->tablaPedidos->setModel(modelPedidos);
}


void Frmrecepcion_pedidos::on_chkForzarCierre_clicked()
{
    if(QMessageBox::question(this,tr("Gestión pedidos"),
                             tr("¿desea cambiar el estado de este pedido?"),tr("No"),tr("Si")) ==QDialog::Accepted)
    {
        QModelIndex index = ui->tablaPedidos->currentIndex();
        int nid =ui->tablaPedidos->model()->data(ui->tablaPedidos->model()->index(index.row(),0),Qt::EditRole).toInt();
        QSqlQuery queryPedido(QSqlDatabase::database("empresa"));
        queryPedido.prepare("update ped_pro set lRecibidoCompleto =:estado where id = :id");
        int estado;
        if(ui->chkForzarCierre->isChecked())
            estado=1;
        else
            estado = 0;

        queryPedido.bindValue(":estado",estado);
        queryPedido.bindValue(":id",nid);
        if(!queryPedido.exec())
            QMessageBox::warning(this,tr("Gestión de pedidos"),
                                 tr("ATENCIÓN, Falló el cambio de estado: %1").arg(queryPedido.lastError().text()),
                                 tr("Aceptar"));

    }
}

void Frmrecepcion_pedidos::on_tablaPedidos_doubleClicked(const QModelIndex &index)
{
    int nid =ui->tablaPedidos->model()->data(ui->tablaPedidos->model()->index(index.row(),0),Qt::EditRole).toInt();
    QSqlQuery query_lineas(QSqlDatabase::database("empresa"));
    query_lineas.prepare("select * from lin_ped_pro where id_cab = :nid");
    query_lineas.bindValue(":nid",nid);
    int pos = 0;
    ui->tablaLineas->clear();
    ui->tablaLineas->setColumnCount(10);
    QStringList cabecera;
    cabecera <<tr("id") << tr("C.prov") << tr("Cod.") <<tr("Descripción") << tr("Cant") <<tr("Pdte.")
            <<tr("Recibidos") <<tr("Bruto") <<tr("Etiquetas");
    ui->tablaLineas->setHorizontalHeaderLabels(cabecera);

    if(query_lineas.exec())
    {
        while (query_lineas.next())
        {
            ui->tablaLineas->setRowCount(pos+1);

            QTableWidgetItem *item_columna0 = new QTableWidgetItem(query_lineas.record().value("id").toString());
            item_columna0->setFlags(item_columna0->flags() & (~Qt::ItemIsEditable));
            item_columna0->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,0,item_columna0);
            ui->tablaLineas->setColumnHidden(0,true);

            QTableWidgetItem *item_columna1 = new QTableWidgetItem(query_lineas.record().value("codigo_articulo_proveedor").toString());
            item_columna1->setFlags(item_columna1->flags() & (~Qt::ItemIsEditable));
            item_columna1->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,1,item_columna1);

            QTableWidgetItem *item_columna2 = new QTableWidgetItem(query_lineas.record().value("codigo_articulo_interno").toString());
            item_columna2->setFlags(item_columna2->flags() & (~Qt::ItemIsEditable));
            item_columna2->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,2,item_columna2);

            QTableWidgetItem *item_columna3 = new QTableWidgetItem(query_lineas.record().value("descripcion").toString());
            item_columna3->setFlags(item_columna3->flags() & (~Qt::ItemIsEditable));
            item_columna3->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,3,item_columna3);
            ui->tablaLineas->setColumnWidth(3,150);

            QTableWidgetItem *item_columna4 = new QTableWidgetItem(query_lineas.record().value("cantidad").toString());
            item_columna4->setFlags(item_columna4->flags() & (~Qt::ItemIsEditable));
            item_columna4->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,4,item_columna4);

            QTableWidgetItem *item_columna5 = new QTableWidgetItem(query_lineas.record().value("cantidad_pendiente").toString());
            item_columna5->setFlags(item_columna5->flags() & (~Qt::ItemIsEditable));
            item_columna5->setTextColor(Qt::blue); // color de los items
            ui->tablaLineas->setItem(pos,5,item_columna5);

            QTableWidgetItem *item_columna6 = new QTableWidgetItem(query_lineas.record().value("cantidad_recibida").toString());
            item_columna6->setTextColor(Qt::black); // color de los items
            item_columna6->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tablaLineas->setItem(pos,6,item_columna6);


            QTableWidgetItem *item_columna7 = new QTableWidgetItem(QString::number(query_lineas.record().value("coste_bruto").toDouble(),'f',2));
            item_columna7->setTextColor(Qt::black); // color de los items
            ui->tablaLineas->setItem(pos,7,item_columna7);
            ui->tablaLineas->setItemDelegateForColumn(7,new MonetaryDelegate);

            QTableWidgetItem *item_columna8 = new QTableWidgetItem(query_lineas.record().value("etiquetas").toString());
            item_columna8->setTextColor(Qt::black); // color de los items
            item_columna8->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tablaLineas->setItem(pos,8,item_columna8);

            QTableWidgetItem *item_columna9 = new QTableWidgetItem(query_lineas.record().value("id_articulo").toString());
            item_columna9->setTextColor(Qt::black); // color de los items
            item_columna9->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tablaLineas->setItem(pos,9,item_columna9);
            ui->tablaLineas->setColumnHidden(9,true);

        }
        connect(ui->tablaLineas,SIGNAL(cellChanged(int,int)),this,SLOT(validarcantidad(int,int)));

    }
}

void Frmrecepcion_pedidos::validarcantidad(int row,int col)
{
    if(col ==6) // Cantidad
    {
        int cant = ui->tablaLineas->item(row,4)->text().toInt();
        int rec = ui->tablaLineas->item(row,col)->text().toInt();
        int pend = cant-rec;
        ui->tablaLineas->item(row,5)->setText(QString::number(pend));
    }
    if(col ==7) // Coste
    {
        int id = ui->tablaLineas->item(row,9)->text().toInt();
        QSqlQuery queryProducto(QSqlDatabase::database("Maya"));
        queryProducto.prepare("select rCoste from articulos where id =:nid");
        queryProducto.bindValue(":nid",id);
        if(queryProducto.exec())
        {
            queryProducto.next();
            double coste = queryProducto.record().value("rCoste").toDouble();
            double valor = ui->tablaLineas->item(row,7)->text().toDouble();
            if(coste != valor)
            {
                if( QMessageBox::question(this,tr("Entrada de Pedidos"),tr("¿Desea actualizar coste en la ficha del producto?"),
                                      tr("No"),tr("Sí")) == QMessageBox::Accepted)
                {
                    queryProducto.prepare("update articulos set rCoste = :coste where id =:id");
                    queryProducto.bindValue(":coste",valor);
                    queryProducto.bindValue(":id",id);
                    if(!queryProducto.exec())
                    {
                        QMessageBox::warning(this,tr("ATENCIÓN"),
                                             tr("Ocurrió un error al cambiar el coste del producto: %1").arg(queryProducto.lastError().text()),
                                             tr("Aceptar"));

                    } else
                    {
                        if (QMessageBox::question(this,tr("Entrada de productos"),
                                                  tr("¿Desea modificar las tarifas del producto?"),
                                                  tr("No"),tr("Si"))==QMessageBox::Accepted)
                        {
                            frmEditar_tarifas frmtarifas;
                            frmtarifas.setFiltro(id);
                            frmtarifas.exec();
                        }
                    }
                }

            }
        }

    }
}

void Frmrecepcion_pedidos::on_pushButton_3_clicked()
{
    FrmBuscarCIE editar;
    editar.exec();
}