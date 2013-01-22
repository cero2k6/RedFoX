#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QErrorMessage>
#include<QSqlError>

#include "login.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFormLayout>
#include <QDebug>
#include <QSettings>
#include "configuracion.h"
#include <QSqlQuery>
#include "frmempresas.h"

#include "frmconfiguracion.h"


#include "frmagendavisitas.h"
#include <QStyleFactory>
#include <QToolBar>

#include <QtCore/QTimer>
#include <QProgressBar>

#include "block_terra_form.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ----------------------------------------------------------------------------
    // Barra de herramientas Modulos
    // ----------------------------------------------------------------------------
    m_modulesBar = new QToolBar(tr("Modulos"), this);
    m_modulesBar->setAllowedAreas(Qt::LeftToolBarArea);
    m_modulesBar->setIconSize(QSize(80, 48));
    m_modulesBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_modulesBar->setMovable(false);
    m_modulesBar->setFloatable(false);
    m_modulesBar->setStyleSheet("background-color:rgb(219, 228, 255)");

   this->addToolBar(Qt::LeftToolBarArea, m_modulesBar);

    // MODULO MANTENIMIENTO.
    QToolButton *BtnMantenimiento = new QToolButton(m_modulesBar);
    BtnMantenimiento->setText(tr("Manten."));
    BtnMantenimiento->setIcon(QIcon(":Icons/PNG/Maintenance.png"));
    BtnMantenimiento->setToolTip(tr("Módulo de mantenimientos"));
    BtnMantenimiento->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // MODULO VENTAS/ENTRADAS.
    QToolButton *BtnEntradas = new QToolButton(m_modulesBar);
    BtnEntradas->setText(tr("Entradas"));
    BtnEntradas->setIcon(QIcon(":Icons/PNG/ventas2.png"));
    BtnEntradas->setToolTip(tr("Módulo de ventas/entradas clinica"));
    BtnEntradas->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // MODULO GASTOS/SALIDAS.
    QToolButton *BtnSalidas = new QToolButton(m_modulesBar);
    BtnSalidas->setText(tr("Salidas"));
    BtnSalidas->setIcon(QIcon(":Icons/PNG/Gastos.png"));
    BtnSalidas->setToolTip(tr("Módulo de Gastos/Salidas clinica"));
    BtnSalidas->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // INF. MEDICA.
    QToolButton *BtnMedica = new QToolButton(m_modulesBar);
    BtnMedica->setText(tr("Clínica"));
    BtnMedica->setIcon(QIcon(":Icons/PNG/PatientFile2.png"));
    BtnMedica->setToolTip(tr("Módulo de Gestión especificamente clínica"));
    BtnMedica->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // Salir
    QToolButton *BtnCerrar = new QToolButton(m_modulesBar);
    BtnCerrar->setText(tr("Salir"));
    BtnCerrar->setIcon(QIcon(":Icons/PNG/Exit.png"));
    BtnCerrar->setToolTip(tr("Salir de Terra"));
    BtnCerrar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_modulesBar->addWidget(BtnMantenimiento);
    m_modulesBar->addWidget(BtnEntradas);
    m_modulesBar->addWidget(BtnSalidas);
    m_modulesBar->addWidget(BtnMedica);
    m_modulesBar->addWidget(BtnCerrar);
    //-------------------------------
    // Cargo Barras Herramientas
    //-------------------------------
    Mantenimientos();
    Ventas();
    btnMantenimientos_clicked();

    // ----------------------------------------------------------------------------
    // Conexiones
    // ----------------------------------------------------------------------------
    connect(ui->btnSalir,SIGNAL(triggered()),this,SLOT(close()));
    connect(BtnMantenimiento,SIGNAL(clicked()),this,SLOT(btnMantenimientos_clicked()));
    connect(BtnEntradas,SIGNAL(clicked()),this,SLOT(btnVentas_clicked()));
    connect(BtnCerrar,SIGNAL(clicked()),this,SLOT(close()));



    QSqlDatabase dbEmp;
    m_config = new Configuracion();
    QSettings settings("infint", "terra");
    // Cambiar por parametros fichero configuración.
    m_config->cDriverBDTerra = settings.value("cDriverBDTerra").toString();
    m_config->cRutaBdTerra = settings.value("cRutaDBTerra").toString();
    m_config->cHostBDTerra = settings.value("cHostBDTerra").toString();
    m_config->cUsuarioBDTerra  =   settings.value("cUserBDTerra").toString();
    m_config->cPasswordBDTerra = settings.value("cPasswordBDTerra").toString();
    m_config->cPais = settings.value("cPais").toString();
    m_config->cEjercicio = settings.value("cEjercicioActivo").toString();
    m_config->nDigitosFactura = settings.value(("nDigitosFactura")).toInt();


    // Abro Base de Datos
    QSqlDatabase dbTerra  = QSqlDatabase::addDatabase(m_config->cDriverBDTerra,"terra");
    //dbTerra.addDatabase(m_config->cDriverBDTerra,"terra");

    if (m_config->cDriverBDTerra == "QSQLITE")
	{
        dbTerra.setDatabaseName(m_config->cRutaBdTerra);
        dbTerra.open();
    }
	else
	{
        dbTerra.setDatabaseName(m_config->cNombreBDTerra);
        dbTerra.setHostName(m_config->cHostBDTerra);
        dbTerra.open(m_config->cUsuarioBDTerra,m_config->cPasswordBDTerra);
        dbTerra.open(m_config->cUsuarioBDTerra,m_config->cPasswordBDTerra);
    }

    if (dbTerra.lastError().isValid())
    {
        QMessageBox::critical(0, "error:", dbTerra.lastError().text());
    }

	// Identificación de usuario
    //Widgets
    frmClientes1 = new frmClientes(m_config,this);
    frmFacturas1 = new frmFacturas(m_config,this);
    frmArticulos1 = new FrmArticulos(m_config,this);
    frmProveedores1 = new frmProveedores(this);
    frmAlbaran1 = new FrmAlbaran(this);
    frmPedidos1 = new FrmPedidos(this);
    frmPresupcli = new FrmPresupuestosCli(this);
    frmCajaMinuta = new FrmCajaMinuta(this);

    ui->stackedWidget->addWidget(frmClientes1);
    ui->stackedWidget->addWidget(frmFacturas1);
    ui->stackedWidget->addWidget(frmArticulos1);
    ui->stackedWidget->addWidget(frmProveedores1);
    ui->stackedWidget->addWidget(frmAlbaran1);
    ui->stackedWidget->addWidget(frmPedidos1);
    ui->stackedWidget->addWidget(frmPresupcli);
    ui->stackedWidget->addWidget(frmCajaMinuta);
    //intit
    QTimer::singleShot(0,this,SLOT(init()));
}
void MainWindow::init()
{
    //Login *dlg = new Login(m_config);
    //NOTE - Fixed: puntero no borrado
    QScopedPointer<Login>dlg(new Login(m_config));
	if ( dlg->exec()==QDialog::Accepted) 
	{
		// capturo usuario
		QString usuario = dlg->getUsuario();
        user = dlg->getUsuario();
        pass = dlg->getPass();
		ui->lineUsuarioActivo->setText(usuario);
		m_config->cUsuarioActivo = ui->lineUsuarioActivo->text();
		QSettings settings("infint", "terra");
		ui->txtnNivel->setText(QString::number( settings.value("nNivelAcceso").toInt()));
		ui->txtcCategoria->setText(settings.value("cCategoria").toString());
		// Oculto controles según categoría
		//TODO - Ocultar controles

		// capturo empresa
		QString Empresa = dlg->getEmpresa();
		ui->lineEmpresaActiva->setText(Empresa);

		// Configuramos valores empresa activa
		QSqlQuery QryEmpresa(QSqlDatabase::database("terra"));
		QryEmpresa.prepare("Select * from empresas where nombre = :nombre");
		QryEmpresa.bindValue(":nombre",Empresa.trimmed());
		if (QryEmpresa.exec()) 
        {
			QryEmpresa.next();
            QDesktopWidget *desktop = QApplication::desktop();
            QProgressBar progress;
            progress.setWindowFlags(Qt::FramelessWindowHint);

            progress.setAlignment(Qt::AlignCenter);
            progress.resize(600 , 40);
            progress.setMaximum(6);
            progress.setVisible(true);
            progress.show();
            progress.move(desktop->width()/2 - progress.width()/2 , desktop->height()/2 - progress.height()/2);
            QApplication::processEvents();
			QSqlRecord record = QryEmpresa.record();
			// DBEMpresa
			m_config->cDriverBDEmpresa = record.field("driverBD").value().toString();
			m_config->cHostBDEmpresa = record.field("host").value().toString();
			m_config->cNombreBDEmpresa =record.field("nombreBD").value().toString();
			m_config->cPasswordBDEmpresa =record.field("contrasena").value().toString();
			m_config->cRutaBdEmpresa = record.field("RutaBDSqLite").value().toString();
			m_config->cUsuarioBDEmpresa = record.field("user").value().toString();
            progress.setValue(1);
            QApplication::processEvents();
			//DBMedica
			m_config->cDriverBDMedica = record.field("driverBDMedica").value().toString();
			m_config->cHostBDMedica = record.field("hostBDMedica").value().toString();
			m_config->cNombreBDMedica =record.field("nombreBDMedica").value().toString();
			m_config->cPasswordBDMedica =record.field("contrasenaBDMedica").value().toString();
			m_config->cRutaBdMedica = record.field("RutaBDMedicaSqLite").value().toString();
			m_config->cUsuarioBDMedica = record.field("userBDMedica").value().toString();
            progress.setValue(2);
            QApplication::processEvents();
			// Varios
			m_config->cSerie = record.field("serie").value().toString();
			m_config->nDigitosCuentasContables = record.field("ndigitoscuenta").value().toInt();
			m_config->cCuentaAcreedores = record.field("codigocuentaacreedores").value().toString();
			m_config->cCuentaClientes = record.field("codigocuentaclientes").value().toString();
			m_config->cCuentaProveedores = record.field("codigocuentaproveedores").value().toString();
            progress.setValue(3);
            QApplication::processEvents();
			// Guardo preferencias
			QSettings settings("infint", "terra");
			settings.setValue("cSerie",m_config->cSerie);
			settings.setValue("nDigitosCuentas",m_config->nDigitosCuentasContables);
			settings.setValue("cCuentaClientes",m_config->cCuentaClientes);
			settings.setValue("cCuentaProveedores",m_config->cCuentaProveedores);
			settings.setValue("cCuentaAcreedores",m_config->cCuentaAcreedores);
			settings.setValue("Clave1",record.field("clave1").value().toString());
            settings.setValue("Clave2",record.field("clave2").value().toString());
            progress.setValue(4);
            QApplication::processEvents();
			// Abro empresa activa
			QSqlDatabase dbEmpresa = QSqlDatabase::addDatabase(m_config->cDriverBDEmpresa,"empresa");
			if (m_config->cDriverBDEmpresa =="QSQLITE") {
				dbEmpresa.setDatabaseName(m_config->cRutaBdEmpresa);
				// qDebug() << "Empresa:" << m_config->cRutaBdEmpresa;
				dbEmpresa.open();
			} else {
				dbEmpresa.setDatabaseName(m_config->cNombreBDEmpresa);
				dbEmpresa.setHostName(m_config->cHostBDEmpresa);
				dbEmpresa.open(m_config->cUsuarioBDEmpresa,m_config->cPasswordBDEmpresa);

			}
            progress.setValue(5);
            QApplication::processEvents();
			// Abro bdmedica activa
			QSqlDatabase dbMedica = QSqlDatabase::addDatabase(m_config->cDriverBDEmpresa,"dbmedica");
			if (m_config->cDriverBDMedica =="QSQLITE") {
				dbMedica.setDatabaseName(m_config->cRutaBdMedica);
                qDebug() << "Medica:" << m_config->cRutaBdMedica;
				if(!dbMedica.open())
					QMessageBox::warning(qApp->activeWindow(),tr("ERROR DB"),tr("No se ha podido abrir la BD medica"),
					tr("Aceptar"));
			} else {
				dbMedica.setDatabaseName(m_config->cNombreBDMedica);
				dbMedica.setHostName(m_config->cHostBDMedica);
				dbMedica.open(m_config->cUsuarioBDMedica,m_config->cPasswordBDMedica);
			}
			if (dbMedica.lastError().isValid())
			{
				QMessageBox::critical(0, "error:", dbMedica.lastError().text());
			}
            progress.setValue(6);
			this->show();
            QApplication::processEvents();           
		} 
		else
			qDebug() <<"Fallo la conexión al fichero Medico";
	} 
	else
        qApp->quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnMantenimientos_clicked()
{
    m_MantenimientosBar->show();
    m_VentasBar->hide();
}

void MainWindow::btnVentas_clicked()
{
    m_MantenimientosBar->hide();
    m_VentasBar->show();
}

void MainWindow::Mantenimientos()
{
    // ----------------------------------------------------------------------------
    // Barra de herramientas Mantenimientos
    // ----------------------------------------------------------------------------
    m_MantenimientosBar = new QToolBar(tr("Mantenimiento"), this);
    m_MantenimientosBar->setAllowedAreas(Qt::TopToolBarArea);
    m_MantenimientosBar->setIconSize(QSize(32,32));
    m_MantenimientosBar->setStyleSheet("background-color:rgb(255, 235, 167)");
    m_MantenimientosBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_MantenimientosBar->setMovable(false);
    m_MantenimientosBar->setFloatable(false);

    this->addToolBar(Qt::TopToolBarArea, m_MantenimientosBar);

    // PACIENTES
    m_MantenimientosBar->addAction(ui->btnClientes);

    // PROVEEDORES
    m_MantenimientosBar->addAction(ui->btnProveedores);

    // ARTICULOS
    m_MantenimientosBar->addAction(ui->btnArt_culos);

    m_MantenimientosBar->addSeparator();

    // AGENDA
    m_MantenimientosBar->addAction(ui->btnAgenda);

    //--------------------------
    // Conexiones
    //--------------------------
    connect(ui->btnClientes,SIGNAL(triggered()),this,SLOT(btnClientes_clicked()));
    connect(ui->btnArt_culos,SIGNAL(triggered()),this,SLOT(btnArticulos_clicked()));
    connect(ui->btnProveedores,SIGNAL(triggered()),this,SLOT(btnProveedores_clicked()));
    //TODO conexion boton agenda
}


void MainWindow::Ventas()
{
    // ----------------------------------------------------------------------------
    // Barra de herramientas Ventas
    // ----------------------------------------------------------------------------
    m_VentasBar = new QToolBar(tr("Ventas"), this);
    m_VentasBar->setAllowedAreas(Qt::TopToolBarArea);
    m_VentasBar->setIconSize(QSize(32,32));
    m_VentasBar->setStyleSheet("background-color:rgb(255, 235, 167)");
    m_VentasBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_VentasBar->setMovable(false);
    m_VentasBar->setFloatable(false);

    this->addToolBar(Qt::TopToolBarArea, m_VentasBar);

    // PRESUPUESTOS
    m_VentasBar->addAction(ui->actionPresupuestos);

    // PEDIDOS
    m_VentasBar->addAction(ui->actionPedidos);

    // ALBARANES
    m_VentasBar->addAction(ui->actionAlbaranes);

    // FACTURA
    m_VentasBar->addAction(ui->actionFacturas);

    // CAJA
    m_VentasBar->addAction(ui->actionVentas_Contado);

    //--------------------------
    // Conexiones
    //--------------------------
    connect(ui->actionPresupuestos,SIGNAL(triggered()),this,SLOT(btnPresup_clientes_clicked()));
    connect(ui->actionPedidos,SIGNAL(triggered()),this,SLOT(btn_Pedido_cliente_clicked()));
    connect(ui->actionAlbaranes,SIGNAL(triggered()),this,SLOT(btnAlbaran_clientes_clicked()));
    connect(ui->actionFacturas,SIGNAL(triggered()),this,SLOT(btnFacturaCliente_clicked()));
    connect(ui->actionVentas_Contado,SIGNAL(triggered()),this,SLOT(btnCajaMinuta_clicked()));
}


void MainWindow::btnClientes_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmClientes1);
}
void MainWindow::btnFacturaCliente_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmFacturas1);
}

void MainWindow::btnArticulos_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmArticulos1);
}

void MainWindow::btnProveedores_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmProveedores1);
}

void MainWindow::btnAlbaran_clientes_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmAlbaran1);
}

void MainWindow::btn_Pedido_cliente_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmPedidos1);
}

void MainWindow::btnPresup_clientes_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmPresupcli);
}

void MainWindow::btnCajaMinuta_clicked()
{
    ui->stackedWidget->setCurrentWidget(frmCajaMinuta);
}

//void MainWindow::on_btnAgenda_clicked()
//{
//    ui->btnAgenda->setEnabled(false);
//    frmAgendaVisitas *frmAgenda1 = new frmAgendaVisitas();
//    frmAgenda1->setWindowState(Qt::WindowMaximized);
//    frmAgenda1->exec();
//    cerrarSubWindows();
//    ui->btnAgenda->setEnabled(true);
//}

void MainWindow::cambiarEstilo(int estado)
{
//    QString style;

//    if (estado ==2)
//        style = "GTK+";
//    else
//        style = "fusion";

//    QApplication::setStyle(style);
}

void MainWindow::on_btn_bloquear_clicked()
{
    block_terra_form form(this);
    form.set_user(user,pass);
    this->hide();
    form.exec();
    this->show();
}
