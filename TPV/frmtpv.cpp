#include "frmtpv.h"
#include "ui_frmtpv.h"
#include <QGraphicsOpacityEffect>
#include"../Almacen/articulo.h"
#include <QColor>

FrmTPV::FrmTPV(QWidget *parent) :
    MayaModule(module_zone(),module_name(),parent),
    ui(new Ui::FrmTPV),
    menuButton(QIcon(":/Icons/PNG/tpv.png"),tr("TPV"),this),
    push(new QPushButton(QIcon(":/Icons/PNG/tpv.png"),"",this))
{
    ui->setupUi(this);
    push->setStyleSheet("background-color: rgb(133, 170, 142)");
    push->setToolTip(tr("Gestión de Caja"));

    ui->btnPorc->setEnabled(false);
    ui->btnMultiplicar->setEnabled(false);
    ui->btnDividir->setEnabled(false);
    this->tipo_dto_tarifa = 0;

    //---------------
    // Eventos
    //---------------
    ui->txtCodigo->installEventFilter(this);

    cargar_ticket(1);


}

FrmTPV::~FrmTPV()
{
    delete ui;
}

void FrmTPV::cargar_ticket(int id)
{
    QMap <int,QSqlRecord> tpv;
    QString error;
    tpv = SqlCalls::SelectRecord("cab_tpv",QString("id =%1").arg(id),Configuracion_global->empresaDB,error);
    ui->lblTicket->setText(QString::number(tpv.value(id).value("ticket").toInt()));
    if(!tpv.value(id).value("nombre_cliente").toString().isEmpty())
        ui->lblCliente->setText(tpv.value(id).value("nombre_cliente").toString());
    ui->lblFecha->setText(tpv.value(id).value("fecha").toString());
    ui->lblHora->setText(tpv.value(id).value("hora").toString());
    this->id = id;
    QString usuario = SqlCalls::SelectOneField("usuarios","nombre",
                                              QString("id=%1").arg(tpv.value(id).value("id_dependiente").toInt()),
                                              Configuracion_global->globalDB,error).toString();
    ui->lblDependiente->setText(usuario);



    cargar_lineas(id);


}

void FrmTPV::cargar_lineas(int id_cab)
{
    ui->tablaLineas_tiquet_actual->clear();
    ui->tablaLineas_tiquet_actual->setColumnCount(6);
    ui->tablaLineas_tiquet_actual->setRowCount(1);
    QMap <int,QSqlRecord> lineas;
    QMap <int,QSqlRecord> lineas_extra;
    QString error;
    lineas = SqlCalls::SelectRecord("lin_tpv",QString("id_cab=%1").arg(id_cab),Configuracion_global->empresaDB,error);
    QMapIterator <int, QSqlRecord> rec_lin(lineas);

    QColor  rowcolor;
    QString codigo,descripcion;
    int id;
    float cantidad;
    double precio,importe,total_ticket;
    int pos = -1;
    int lin =0;
    //-----------------
    // formato tabla
    //-----------------
    QVariantList sizes;
    sizes << 0 << 200 <<50 <<80 <<80;
    QStringList headers;
    headers << "id" << tr("Descripción") << tr("Cant") << tr("PVP") << tr("TOTAL");
    ui->tablaLineas_tiquet_actual->setHorizontalHeaderLabels(headers);
    for(int i = 0;i<sizes.size();i++)
    {
        ui->tablaLineas_tiquet_actual->setColumnWidth(i,sizes.at(i).toInt());

    }
    ui->tablaLineas_tiquet_actual->setColumnHidden(5,true);
    while(rec_lin.hasNext()){
        rec_lin.next();
        ui->tablaLineas_tiquet_actual->setRowCount(ui->tablaLineas_tiquet_actual->rowCount()+1);
        id = rec_lin.value().value("id").toInt();
        codigo = rec_lin.value().value("codigo").toString();
        cantidad = rec_lin.value().value("cantidad").toFloat();
        descripcion = rec_lin.value().value("descripcion").toString();
        precio = rec_lin.value().value("precio").toDouble();
        importe = rec_lin.value().value("importe").toDouble();
        total_ticket += importe;

        //--------------------------------
        // Creo línea base en tablewidget
        //--------------------------------

        pos++;
        ui->tablaLineas_tiquet_actual->setRowCount(pos+1);
        lin++;
        if(lin%2 == 0)
            rowcolor.setRgb(240,240,255);
        else
            rowcolor.setRgb(255,255,255);

        QTableWidgetItem *item_col0 = new QTableWidgetItem(QString::number(id));
        item_col0->setFlags(item_col0->flags() & (~Qt::ItemIsEditable));
        item_col0->setBackgroundColor(rowcolor);

        ui->tablaLineas_tiquet_actual->setItem(pos,0,item_col0);
        ui->tablaLineas_tiquet_actual->setColumnHidden(0,true);

        QTableWidgetItem *item_col1 = new QTableWidgetItem(descripcion);
        item_col1->setFlags(item_col1->flags() & (~Qt::ItemIsEditable));
        item_col1->setBackgroundColor(rowcolor);
        ui->tablaLineas_tiquet_actual->setItem(pos,1,item_col1);

        QTableWidgetItem *item_col2 = new QTableWidgetItem(QString::number(cantidad));
        item_col2->setFlags(item_col2->flags() & (~Qt::ItemIsEditable));
        item_col2->setBackgroundColor(rowcolor);
        item_col2->setTextAlignment(Qt::AlignRight);
        ui->tablaLineas_tiquet_actual->setItem(pos,2,item_col2);

        QTableWidgetItem *item_col3 = new QTableWidgetItem(Configuracion_global->toFormatoMoneda(QString::number(precio,'f',
                                                                                         Configuracion_global->decimales)));
        item_col3->setFlags(item_col3->flags() & (~Qt::ItemIsEditable));
        item_col3->setBackgroundColor(rowcolor);
        item_col3->setTextAlignment(Qt::AlignRight);
        ui->tablaLineas_tiquet_actual->setItem(pos,3,item_col3);

        QTableWidgetItem *item_col4 = new QTableWidgetItem(Configuracion_global->toFormatoMoneda(QString::number(importe,'f',
                                                                          Configuracion_global->decimales_campos_totales)));
        item_col4->setFlags(item_col4->flags() & (~Qt::ItemIsEditable));
        item_col4->setBackgroundColor(rowcolor);
        item_col4->setTextAlignment(Qt::AlignRight);
        ui->tablaLineas_tiquet_actual->setItem(pos,4,item_col4);

        QTableWidgetItem *item_col5 = new QTableWidgetItem("*");
        item_col5->setFlags(item_col5->flags() & (~Qt::ItemIsEditable));
        item_col5->setBackgroundColor(rowcolor);
        item_col5->setTextAlignment(Qt::AlignRight);
        ui->tablaLineas_tiquet_actual->setItem(pos,5,item_col5);
        //--------------------------------------
        // Comprovamos líneas extra (dtos, etc)
        //--------------------------------------
        lineas_extra = SqlCalls::SelectRecord("lin_tpv_2",QString("id_cab=%1").arg(id),Configuracion_global->empresaDB,error);
            QMapIterator <int,QSqlRecord> lineas_e(lineas_extra);
        if(error.isEmpty())
        {
            QString descripcion_2,tipo;
            double  valor_esp;
            int id_lin;

            while(lineas_e.hasNext())
            {
                lineas_e.next();
                id_lin = lineas_e.value().value("id").toInt();
                descripcion_2 = lineas_e.value().value("descripcion").toString();
                tipo = lineas_e.value().value("tipo").toString();
                valor_esp = lineas_e.value().value("valor").toDouble();

                pos++;
                ui->tablaLineas_tiquet_actual->setRowCount(pos+1);
                QTableWidgetItem *item_s_col0 = new QTableWidgetItem(QString::number(id_lin));
                item_s_col0->setFlags(item_col0->flags() & (~Qt::ItemIsEditable));
                item_s_col0->setBackgroundColor(rowcolor);

                ui->tablaLineas_tiquet_actual->setItem(pos,0,item_s_col0);
                if(tipo =="i"){

                    QTableWidgetItem *item_s_col1 =
                            new QTableWidgetItem(QString("     %1 %2 %3").arg(descripcion_2).arg(QString::number(valor_esp)).arg("€"));
                    item_s_col1->setFlags(item_col1->flags() & (~Qt::ItemIsEditable));
                    item_s_col1->setBackgroundColor(rowcolor);
                    ui->tablaLineas_tiquet_actual->setItem(pos,1,item_s_col1);
                } else
                {
                    QTableWidgetItem *item_s_col1 =
                            new QTableWidgetItem(QString("     %1 %2 %3").arg(descripcion_2).arg(QString::number(valor_esp)).arg("%"));
                    item_s_col1->setFlags(item_col1->flags() & (~Qt::ItemIsEditable));
                    item_s_col1->setBackgroundColor(rowcolor);
                    ui->tablaLineas_tiquet_actual->setItem(pos,1,item_s_col1);
                }


                QTableWidgetItem *item_s_col2 = new QTableWidgetItem("");
                item_s_col2->setFlags(item_col2->flags() & (~Qt::ItemIsEditable));
                item_s_col2->setBackgroundColor(rowcolor);
                item_s_col2->setTextAlignment(Qt::AlignRight);

                ui->tablaLineas_tiquet_actual->setItem(pos,2,item_s_col2);

                QTableWidgetItem *item_s_col3 = new QTableWidgetItem(QString(""));
                item_s_col3->setFlags(item_col3->flags() & (~Qt::ItemIsEditable));
                item_s_col3->setBackgroundColor(rowcolor);

                ui->tablaLineas_tiquet_actual->setItem(pos,3,item_s_col3);
                valor_esp = -valor_esp;
                if(tipo == "i"){
                    QTableWidgetItem *item_s_col4 = new QTableWidgetItem(QString::number(valor_esp,'f',2));
                    item_s_col4->setFlags(item_col4->flags() & (~Qt::ItemIsEditable));
                    item_s_col4->setBackgroundColor(rowcolor);
                    item_s_col4->setTextAlignment(Qt::AlignRight);
                    ui->tablaLineas_tiquet_actual->setItem(pos,4,item_s_col4);
                } else
                {
                    valor_esp = importe *(valor_esp/100);
                    QTableWidgetItem *item_s_col4 =
                            new QTableWidgetItem(QString::number(valor_esp,'f',Configuracion_global->decimales_campos_totales));
                    item_s_col4->setFlags(item_col4->flags() & (~Qt::ItemIsEditable));
                    item_s_col4->setBackgroundColor(rowcolor);
                    item_s_col4->setTextAlignment(Qt::AlignRight);
                    ui->tablaLineas_tiquet_actual->setItem(pos,4,item_s_col4);
                }
                QTableWidgetItem *item_s_col5 =
                        new QTableWidgetItem(tipo);
                item_s_col5->setFlags(item_col5->flags() & (~Qt::ItemIsEditable));
                item_s_col5->setBackgroundColor(rowcolor);
                item_s_col5->setTextAlignment(Qt::AlignRight);
                ui->tablaLineas_tiquet_actual->setItem(pos,5,item_s_col5);

                total_ticket += valor_esp;

            }

        }else
            QMessageBox::warning(this,tr("Gestión de tickets"),
                                 tr("Falló la recuperación de sub-lineas de ticket: %1").arg(error));

    }
    this->row_tabla = pos;
    ui->tablaLineas_tiquet_actual->selectRow(pos);
    ui->lbl_total_ticket->setText(Configuracion_global->toFormatoMoneda(QString::number(total_ticket,'f',
                                                        Configuracion_global->decimales_campos_totales)));


}

void FrmTPV::on_btnClientes_clicked()
{

}



void FrmTPV::on_btnBuscar_clicked()
{

}


void FrmTPV::on_txtCodigo_editingFinished()
{
    ui->txtCodigo->blockSignals(true);
    if(ui->btnScanear->isChecked()){
        Articulo oArt;
        QMap <int,QSqlRecord> art;
        QString error,clausula;
        clausula = QString("codigo ='%1'").arg(ui->txtCodigo->text());
        art = SqlCalls::SelectRecord("vistaart_tarifa",clausula,Configuracion_global->groupDB,error);
        int id_articulo = 0;
        QMapIterator <int,QSqlRecord> iter(art);
        while(iter.hasNext()){
            iter.next();
            id_articulo = iter.value().value("id").toInt();
        }

        if(id_articulo > 0)
        {
            //-----------------------
            // Inserto lineas venta
            //-----------------------
            // CAPTURO DATOS
            //-----------------------

            QString codigo,descripcion;
            double precio;
            codigo = art.value(id_articulo).value("codigo").toString();
            descripcion = art.value(id_articulo).value("descripcion_reducida").toString();

            if(this->tipo_dto_tarifa == 1)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                         *(art.value(id_articulo).value("porc_dto1").toDouble()/100));
            else if (this->tipo_dto_tarifa == 2)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                         *(art.value(id_articulo).value("porc_dto2").toDouble()/100));
            else if (this->tipo_dto_tarifa == 3)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                         *(art.value(id_articulo).value("porc_dto3").toDouble()/100));
            else if (this->tipo_dto_tarifa == 4)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                        *(art.value(id_articulo).value("porc_dto4").toDouble()/100));
            else if (this->tipo_dto_tarifa == 5)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                         *(art.value(id_articulo).value("porc_dto5").toDouble()/100));
            else if (this->tipo_dto_tarifa == 6)
                precio = art.value(id_articulo).value("pvp").toDouble()-(art.value(id_articulo).value("pvp").toDouble()
                                                                         *(art.value(id_articulo).value("porc_dto6").toDouble()/100));
            else
                precio = art.value(id_articulo).value("pvp").toDouble();

            //--------------
            // IVA
            //--------------
            double iva_art = Configuracion_global->Devolver_iva(art.value(id_articulo).value("id_tipos_iva").toInt());

            //--------------------
            // INSERCIÓN DE DATOS
            //--------------------
            QHash <QString,QVariant> h;
            h["id_cab"] = this->id;
            h["id_articulo"] = id_articulo;
            h["codigo"] = codigo;
            h["descripcion"] = descripcion;
            h["precio"] = precio;
            h["iva"] = iva_art;
            h["cantidad"] = 1;
            h["importe"] = precio;
            h["total"] = precio;

            int new_id = SqlCalls::SqlInsert(h,"lin_tpv",Configuracion_global->empresaDB,error);
            if(new_id == -1)
                QMessageBox::warning(this,tr("Gestión de TPV"),tr("Ocurrió un error al insertar: %1").arg(error),tr("Aceptar"));
            else
            {
                cargar_lineas(this->id);
                ui->tablaLineas_tiquet_actual->selectRow(this->row_tabla);
                ui->txtCodigo->clear();
            }

        } else{
            if(!ui->txtCodigo->text().isEmpty()) {
                QMessageBox::warning(this,tr("Gestión de artículos"),tr("No se encuentra el artículo"),tr("Aceptar"));
                ui->txtCodigo->clear();
            }
        }
    } else if(ui->btnDto->isChecked())
    {
       ui->btnDto->setChecked(false);
       int id = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),0)->text().toInt();
       QHash <QString,QVariant> h;
       QString error;
       h["id_cab"]= id;
       h["descripcion"] = tr("Descuento manual:").arg(ui->txtCodigo->text());
       h["tipo"] = "p";
       h["valor"] = ui->txtCodigo->text().toDouble();
       int new_id = SqlCalls::SqlInsert(h,"lin_tpv_2",Configuracion_global->empresaDB,error);
       if(new_id == -1)
           QMessageBox::warning(this,tr("Gestión de TPV"),tr("Ocurrió un error al insertar linea descuento"),tr("Aceptar"));
       ui->txtCodigo->clear();
       cargar_lineas(this->id);
       ui->btnScanear->setChecked(true);
       on_btnScanear_clicked(true);

    } else if(ui->btnCantidad->isChecked())
    {
        int id = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),0)->text().toInt();
        double importe = Configuracion_global->MonedatoDouble(
                    ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),3)->text());
        QHash <QString,QVariant> h;
        h["cantidad"] = ui->txtCodigo->text().toDouble();
        h["importe"] = ui->txtCodigo->text().toDouble() * importe;
        QString error;
        bool updated = SqlCalls::SqlUpdate(h,"lin_tpv",Configuracion_global->empresaDB,QString("id=%1").arg(id),error);
        if(!updated)
            QMessageBox::warning(this,tr("Gestión TPV"),tr("Ocurrió un error al actualizar: %1").arg(error));
        else
        {
//            QMap <int,QSqlRecord> m;
//            m = SqlCalls::SelectRecord("lin_ptv_2",QString("id_cab=%1").arg(id),Configuracion_global->empresaDB,error);
//            QMapIterator <int,QSqlRecord> i(m);
//            while (i.hasNext()) {
//                i.next();


//            }

        }
        cargar_lineas(this->id);
    }
    ui->txtCodigo->blockSignals(false);

}

void FrmTPV::on_btnScanear_clicked(bool checked)
{
    if(checked)
    {

        ui->txtCodigo->setFocus();
        ui->txtCodigo->setStyleSheet("background-color: rgb(0,200,0); font: 12pt 'Sans Serif';");
    } else
        ui->txtCodigo->setStyleSheet("background-color:rgb(255, 255, 191); font: 12pt 'Sans Serif';");

}


void FrmTPV::on_btnbotones_clicked()
{
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp = new QPropertyAnimation(fade_effect, "opacity");
    animationOp->setEasingCurve(QEasingCurve::Linear);
    animationOp->setDuration(1000);
    animationOp->setStartValue(1.0);
    animationOp->setEndValue(0.0);
    connect(animationOp,SIGNAL(finished()),this,SLOT(fin_fade_buttons()));
    animationOp->start();


}

void FrmTPV::on_btnticket_clicked()
{
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp = new QPropertyAnimation(fade_effect, "opacity");
    animationOp->setEasingCurve(QEasingCurve::Linear);
    animationOp->setDuration(500);
    animationOp->setStartValue(1.0);
    animationOp->setEndValue(0.0);
    connect(animationOp,SIGNAL(finished()),this,SLOT(fin_fade_ticket()));
    animationOp->start();
}

void FrmTPV::fin_fade_ticket()
{
    ui->frame_ticket->setCurrentIndex(0);
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp2 = new QPropertyAnimation(fade_effect, "opacity");
    animationOp2->setEasingCurve(QEasingCurve::Linear);
    animationOp2->setDuration(500);
    animationOp2->setStartValue(0.0);
    animationOp2->setEndValue(1.0);
    animationOp2->start();

}
void FrmTPV::fin_fade_buttons()
{
    ui->frame_ticket->setCurrentIndex(2);
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp2 = new QPropertyAnimation(fade_effect, "opacity");
    animationOp2->setEasingCurve(QEasingCurve::Linear);
    animationOp2->setDuration(500);
    animationOp2->setStartValue(0.0);
    animationOp2->setEndValue(1.0);
    animationOp2->start();

}

void FrmTPV::on_btnlista_clicked()
{
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp = new QPropertyAnimation(fade_effect, "opacity");
    animationOp->setEasingCurve(QEasingCurve::Linear);
    animationOp->setDuration(500);
    animationOp->setStartValue(1.0);
    animationOp->setEndValue(0.0);
    connect(animationOp,SIGNAL(finished()),this,SLOT(fin_fade_lista()));
    animationOp->start();
}
void FrmTPV::fin_fade_lista()
{
    ui->frame_ticket->setCurrentIndex(1);
    QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect(this);
    //this->setGraphicsEffect(fade_effect);
    ui->frame_ticket->setGraphicsEffect(fade_effect);
    QPropertyAnimation *animationOp2 = new QPropertyAnimation(fade_effect, "opacity");
    animationOp2->setEasingCurve(QEasingCurve::Linear);
    animationOp2->setDuration(500);
    animationOp2->setStartValue(0.0);
    animationOp2->setEndValue(1.0);
    animationOp2->start();

}

void FrmTPV::on_btn1_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("1"));
}

void FrmTPV::on_btnBack_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().left(ui->txtCodigo->text().length()-1));
}

void FrmTPV::on_btn2_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("2"));
}

void FrmTPV::on_btn3_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("3"));
}

void FrmTPV::on_btn4_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("4"));
}

void FrmTPV::on_btn5_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("5"));
}

void FrmTPV::on_btn6_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("6"));
}

void FrmTPV::on_btn7_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("7"));
}

void FrmTPV::on_btn8_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("8"));
}

void FrmTPV::on_btn9_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("9"));
}

void FrmTPV::on_btnIntro_clicked()
{
    qreal resultado;
    if (ui->btnCalculadora->isChecked())
    {
        if(this->valor1 != 0 && !this->symbol.isEmpty())
            this->valor2 = ui->txtCodigo->text().toDouble();
            if(this->symbol == "+")
                resultado = valor1 + valor2;
            if(this->symbol == "-")
                resultado = valor1 - valor2;
            if(this->symbol == "*")
                resultado = valor1 * valor2;
            if(this->symbol == "/")
                resultado = valor1 / valor2;
            if(this->symbol == "%")
                resultado = valor1 * (valor2/100);

            ui->txtCodigo->setText(Configuracion_global->toFormatoMoneda(QString::number(resultado,'f',
                                                                  Configuracion_global->decimales_campos_totales)));

    } else
    {
        on_txtCodigo_editingFinished();
        ui->txtCodigo->clear();
    }
}

void FrmTPV::on_btnSumar_clicked()
{
    if(ui->btnCalculadora->isChecked())
    {
        this->valor1 = Configuracion_global->MonedatoDouble(ui->txtCodigo->text());
        ui->txtCodigo->clear();
        ui->txtCodigo->setFocus();
        this->symbol = "+";
    } else
    {
        int id = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),0)->text().toInt();
        QString tipo = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),5)->text();
        int cant = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),2)->text().toInt();

        if(id >0)
        {
            if(tipo =="*")
            {
                cant++;
                QHash<QString,QVariant> h;
                QString error;
                h["cantidad"] = cant;

                bool succes = SqlCalls::SqlUpdate(h,"lin_tpv",Configuracion_global->empresaDB,QString("id=%1").arg(id),error);
                if(!succes)
                    QMessageBox::warning(this,tr("Gestión TPV"),tr("Ocurrió un error al incrementar cantidad: %1").arg(error),
                                         tr("Aceptar"));
                else
                    cargar_lineas(this->id);
            }
            cargar_lineas(this->id);
        }
    }
}

void FrmTPV::on_btnRestar_clicked()
{
    this->valor1 = Configuracion_global->MonedatoDouble(ui->txtCodigo->text());
    ui->txtCodigo->clear();
    ui->txtCodigo->setFocus();
    this->symbol = "-";
}

void FrmTPV::on_btnMultiplicar_clicked()
{
    this->valor1 = Configuracion_global->MonedatoDouble(ui->txtCodigo->text());
    ui->txtCodigo->clear();
    ui->txtCodigo->setFocus();
    this->symbol = "*";
}

void FrmTPV::on_btnDividir_clicked()
{
    this->valor1 = Configuracion_global->MonedatoDouble(ui->txtCodigo->text());
    ui->txtCodigo->clear();
    ui->txtCodigo->setFocus();
    this->symbol = "/";
}

void FrmTPV::on_btnPorc_clicked()
{
    this->valor1 = Configuracion_global->MonedatoDouble(ui->txtCodigo->text());
    ui->txtCodigo->setText(0);
    ui->txtCodigo->setFocus();
    this->symbol = "%";
}


void FrmTPV::on_btnCalculadora_clicked(bool checked)
{
    ui->btnPorc->setEnabled(checked);
    ui->btnMultiplicar->setEnabled(checked);
    ui->btnDividir->setEnabled(checked);
    ui->txtCodigo->clear();
    ui->txtCodigo->setFocus();
}

bool FrmTPV::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(obj == ui->txtCodigo)
            {
            if (ui->btnCalculadora->isChecked())
            {
                if(keyEvent->key() == 42) // *
                {
                    on_btnMultiplicar_clicked();
                    return true;
                }
                if(keyEvent->key() == 47 ) // /
                {
                    on_btnDividir_clicked();
                    return true;
                }
                if(keyEvent->key() == 43) // +
                {
                    on_btnSumar_clicked();
                    return true;
                }
                if(keyEvent->key() == 45) // -
                {
                    on_btnRestar_clicked();
                    return true;
                }
                if(keyEvent->key() == 37) // %
                {
                    on_btnPorc_clicked();
                    return true;
                }
                if(keyEvent->key() == Qt::Key_Enter){
                    on_btnIntro_clicked();
                }
                if(keyEvent->key()==Qt::Key_Back)
                    on_btnBack_clicked();
            }
        }
        return false;
    }
}

void FrmTPV::on_btn0_clicked()
{
    ui->txtCodigo->setText(ui->txtCodigo->text().append("0"));
}



void FrmTPV::on_btnScanear_clicked()
{
    ui->txtCodigo->setFocus();
}

void FrmTPV::on_lblDependiente_linkActivated(const QString &link)
{

}

void FrmTPV::on_btnDto_clicked(bool checked)
{
    ui->btnScanear->setChecked(false);
    ui->btnCalculadora->setChecked(false);
    ui->btnCantidad->setChecked(false);
    if(checked)
   {
        ui->txtCodigo->setFocus();
        ui->txtCodigo->setStyleSheet("color:rgb(255,255,255);background-color: rgb(200,0,0); font: 12pt 'Sans Serif';");
    } else
        ui->txtCodigo->setStyleSheet("color:rgb(0,0,0);background-color:rgb(255, 255, 191); font: 12pt 'Sans Serif';");
}

void FrmTPV::on_btnCantidad_clicked(bool checked)
{
    ui->btnScanear->setChecked(false);
    ui->btnCalculadora->setChecked(false);
    ui->btnDto->setChecked(false);
    if(checked)
   {
        ui->txtCodigo->setFocus();
        ui->txtCodigo->setStyleSheet("color:rgb(255,255,255);background-color: rgb(0,200,0); font: 12pt 'Sans Serif';");
    } else
        ui->txtCodigo->setStyleSheet("color:rgb(0,0,0);background-color:rgb(255, 255, 191); font: 12pt 'Sans Serif';");
}


void FrmTPV::on_btnBorrar_linea_clicked()
{
    int id = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),0)->text().toInt();
    QString tipo = ui->tablaLineas_tiquet_actual->item(ui->tablaLineas_tiquet_actual->currentRow(),5)->text();
    if(id >0)
    {
        if(QMessageBox::question(this,tr("Gestión de Tickets"),tr("¿Desea borrar esta línea del ticket?"),tr("no"),
                                 tr("Sí"))==QMessageBox::Accepted)
        {
            bool succes;
            QString error;
            // TODO - Restaurar Stock y acumulados.
            if(tipo == "*")
                succes = SqlCalls::SqlDelete("lin_tpv",Configuracion_global->empresaDB,
                                              QString("id=%1").arg(id),error);
            else
                succes = SqlCalls::SqlDelete("lin_tpv_2",Configuracion_global->empresaDB,
                                              QString("id=%1").arg(id),error);
            if(!succes)
                QMessageBox::warning(this,tr("Gestión de TPV"),tr("Falló el borrado de línea"),tr("Aceptar"));
        }
        cargar_lineas(this->id);
    }
}

