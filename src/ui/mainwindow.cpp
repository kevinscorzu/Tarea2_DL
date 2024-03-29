#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nrzi.h"

MainWindow* MainWindow::hammingWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList columnHeaders = { "p1", "p2", "d1", "p3", "d2", "d3", "d4", "p4", "d5", "d6", "d7", "d8", "d9", "d10", "d11", "p5", "d12", "Error" };
    QStringList rowHeaders = { "Original", "p1", "p2", "p3", "p4", "p5" , "Final"};
    ui -> tableWidget->setHorizontalHeaderLabels(columnHeaders);
    ui -> tableWidget->setVerticalHeaderLabels(rowHeaders);
    ui -> tableWidget_2->setHorizontalHeaderLabels(columnHeaders);
    ui -> tableWidget_2->setVerticalHeaderLabels(rowHeaders);
    ui -> tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Changed=1;
    //ui -> label_3->setStyleSheet("QLabel { background-color : grey; color : black; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow* MainWindow::getInstance() {
    if (!hammingWindow) {
        hammingWindow = new MainWindow;
    }
    return hammingWindow;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui ->checkBox->isChecked()){
        Hamming::init(1);
    }
    else{
        Hamming::init(0);
    }

    if (!Hamming::firstLine(ui->lineEdit->text())){
        ui ->label_3->setText("Por favor introduzca solamente 12 dígitos en binario");
        return;
    }

    Hamming::completeLines();
    Hamming::show();
    ui ->label_3->setText("Código hamming generado con éxito\nPara cambiar un bit haga doble click sobre él en la fila \"Final\"");

    int i, j;
    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
          QTableWidgetItem *newItem;
          if (Hamming::arr[i][j]==1)
            newItem= new QTableWidgetItem("1");
          else if (Hamming::arr[i][j]==0)
            newItem= new QTableWidgetItem("0");
          else
            continue;
         ui -> tableWidget->setItem(i, j, newItem);
         }
    }
    actualParity=Hamming::getParity();
    actualInput=ui->lineEdit->text();
    Changed=0;
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if (row!=6 || Changed==1){
        return;
    }
    QString newinput;
    QString newinputHamming;
    int* pares=Hamming::getPares();
    int c=0;
    int preError=-1;
    for (int i=0;i<5;i++){
        //qDebug()<<"$$$"<<pares[i];
    }
   // qDebug()<<column;
    for (int i=0;i<17;i++){
     //   qDebug()<<"READING"<<i<<c;
        if (i==column){
            if (i==pares[c]){
       //         qDebug()<<"CHANGED PARITY"<<column;
                ui ->label_3->setText("El error se identifica en las posición"+QString::number(i));
                preError=i;
                c++;
            }
            else if (actualInput.at(i-c)=='1'){
         //       qDebug()<<"CHANGED TO 0"<<column;
                newinput+='0';
            }
            else{
         //       qDebug()<<"CHANGED TO 1"<<column;
                newinput+='1';
            }
            continue;
        }
        else if (i==pares[c]){
            c++;
        }
        else{
           newinput+=actualInput.at(i-c);
        }
    }

    for (int i=0;i<17;i++){
        if (i==column){
            if (Hamming::arr[6][i]==1){
                 newinputHamming+='0';
                }
            else{
                newinputHamming+='1';
            }
        }
        else if (Hamming::arr[6][i]==0){
                newinputHamming+='0';
            }
            else{
                newinputHamming+='1';
            }
    }
   // qDebug()<<newinput;
   // qDebug()<<newinputHamming;

    Hamming::firstLine(newinput);
    Hamming::completeLines();
    Hamming::show();
    int i, j;
    for (i = 0; i < filas-1; i++) {
        for (j = 0; j < columnas; j++) {
          QTableWidgetItem *newItem;
          if (Hamming::arr[i][j]==1)
            newItem= new QTableWidgetItem("1");
          else if (Hamming::arr[i][j]==0)
            newItem= new QTableWidgetItem("0");
          else
            continue;
         ui -> tableWidget_2->setItem(i, j, newItem);
         }
    }
    for (j = 0; j < columnas; j++) {
      QTableWidgetItem *newItem;
      newItem= new QTableWidgetItem(newinputHamming.at(j));
     ui -> tableWidget_2->setItem(i, j, newItem);
     }
    Hamming::findError(newinputHamming);
   // qDebug()<<Hamming::LastParity<<Hamming::ActualParity;
    for (int i=1;i<6;i++){
        QTableWidgetItem *newItem;
        if (Hamming::LastParity.at(i-1)==Hamming::ActualParity.at(i-1)){
            newItem= new QTableWidgetItem("0");
        }
        else{
            newItem= new QTableWidgetItem("1");
        }
        //qDebug()<<Hamming::LastParity.at(i-1)<<Hamming::ActualParity.at(i-1);
        ui -> tableWidget_2->setItem(i, 17, newItem);
    }



    ui ->label_3->setText("Pariedad inicial: "+Hamming::LastParity
                          +"\n Pariedad modificada: "+Hamming::ActualParity
                          +"\nEntonces el error se identifica en la posición "+QString::number(Hamming::Error));

    Changed=1;
}

void MainWindow::toNRZIWindow(){
    nrzi* nrziWindow = nrzi::getInstance();
    nrziWindow->show();
}

void MainWindow::on_returnToNRZI_clicked(){
    toNRZIWindow();
    hide();
}

void MainWindow::setBin(char* bin) {
    ui->lineEdit->setText(QString::fromUtf8(bin));
}
