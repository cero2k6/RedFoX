#include "apuntes.h"

apuntes::apuntes(QObject *parent) :
    QObject(parent)
{
}

int apuntes::nuevo_numero_apunte()
{
    QSqlQuery queryApuntes(QSqlDatabase::database("dbconta"));
    if(queryApuntes.exec("select asiento from diario order by asiento desc limit 0,1"))
    {
        queryApuntes.next();
        int asiento = queryApuntes.record().value("asiento").toInt();
        return asiento+1;
    } else
    {
        return -1;
    }

}

bool apuntes::nuevalinea()
{
    QSqlQuery query_apunte(QSqlDatabase::database("dbconta"));
    query_apunte.prepare("INSERT INTO diario id_cuenta, id_documento,DH,cuentaD,descripcionD,"
                         "cuentaH,descripcionH,importeD,importeH,asiento,id_cuentaD,id_cuentaH,"
                         "fechaAsiento,posenasiento,cta_principal) "
                         "VALUES (:id_cuenta,:id_documento,:DH,:cuentaD,:descripcionD,:cuentaH,"
                         ":descripcionH,:importeD,:importeH,:asiento,:id_cuentaD,:id_cuentaH,"
                         ":fechaAsiento,:posenasiento,:cta_principal);");
    query_apunte.bindValue(":id_cuenta",this->id_cuenta);
    query_apunte.bindValue(":id_documento",this->id_documento);
    query_apunte.bindValue(":DH",this->DH);
    query_apunte.bindValue(":cuentaD",this->cuentaD);
    query_apunte.bindValue(":descripcionD",this->descripcionD);
    query_apunte.bindValue(":cuentaH",this->cuentaH);
    query_apunte.bindValue(":descripcionH",this->descripcionH);
    query_apunte.bindValue(":importeD",this->importeD);
    query_apunte.bindValue(":importeH",this->importeH);
    query_apunte.bindValue(":asiento",this->asiento);
    query_apunte.bindValue(":id_cuentaD",this->id_cuentaD);
    query_apunte.bindValue(":id_cuentaH",this->id_cuentaH);
    query_apunte.bindValue(":fechaAsiento",this->fechaAsiento);
    query_apunte.bindValue(":posenasiento",this->posenasiento);
    query_apunte.bindValue(":cta_principal",this->cta_principal);
    if(!query_apunte.exec())
    {
        QMessageBox::warning(qApp->activeWindow(),tr("Gestión de apuntes"),
                             tr("Ocurrió un error al insertar %1").arg(query_apunte.lastError().text()),tr("Aceptar"));

        return false;
    } else
    {
        return true;
    }
}