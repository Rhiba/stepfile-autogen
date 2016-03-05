#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processAudio.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setWindowTitle("Stepmania Stepfile Autogenerator");
    MainWindow::ui->progressBar->setValue(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString fileName = dialog.getExistingDirectory(this, tr("Choose Folder"),"~");
    MainWindow::ui->textEdit->setText(fileName);
}


void MainWindow::on_pushButton_3_clicked()
{
    QFileDialog dialog;
    QString fileName = dialog.getOpenFileName(this, tr("Choose Song"),"~");
    MainWindow::ui->textEdit_2->setText(fileName);
}

void MainWindow::on_pushButton_2_clicked()
{
    //GO BUTTON
    QString text = MainWindow::ui->textEdit->toPlainText();
    if (text.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Please enter the Stepmania file path.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        if(msgBox.exec() == QMessageBox::Ok){
            // do something
            return;
        }
    } else {
        struct stat info;
        QByteArray ba = text.toLatin1();
        const char * text_str = ba.data();
        if( stat( text_str, &info ) != 0 ) {
            //printf( "cannot access %s\n", text );
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Cannot find Stepmania directory.  ");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            if(msgBox.exec() == QMessageBox::Ok){
                // do something
                return;
            }
        } else if( info.st_mode & S_IFDIR ) {
            //printf( "%s is a directory\n", text );
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Cannot find Stepmania directory.  ");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            if(msgBox.exec() == QMessageBox::Ok){
                // do something
                return;
            }
        }
    }

    text = MainWindow::ui->textEdit_2->toPlainText();
    if (text.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Please enter the song file path.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        if(msgBox.exec() == QMessageBox::Ok){
            // do something
            return;
        }
    } else {
        struct stat info;
        QByteArray ba = text.toLatin1();
        const char * text_str = ba.data();
        if( stat( text_str, &info ) != 0 ) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Cannot find song path.  ");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            if(msgBox.exec() == QMessageBox::Ok){
                // do something
                return;
            }
        } else if( info.st_mode & S_IFDIR ) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Cannot find song path.  ");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            if (msgBox.exec() == QMessageBox::Ok) {
                // do something
                return;
            }
        } else {
        }
    }
    text = MainWindow::ui->textEdit_3->toPlainText();
    if (text.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Please enter the song title.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        if(msgBox.exec() == QMessageBox::Ok){
            // do something
            return;
        }
    }
    text = MainWindow::ui->textEdit_4->toPlainText();
    if (text.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Please enter the artist name.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        if(msgBox.exec() == QMessageBox::Ok){
            // do something
            return;
        }
    }
    std::string stepmaniaPath = MainWindow::ui->textEdit->toPlainText().toStdString();
    std::string songPath = MainWindow::ui->textEdit_2->toPlainText().toStdString();
    std::string artist = MainWindow::ui->textEdit_3->toPlainText().toStdString();
    std::string title = MainWindow::ui->textEdit_4->toPlainText().toStdString();
    std::vector<std::string> arr;
    arr.push_back(songPath);
    arr.push_back(stepmaniaPath);
    arr.push_back(artist);
    arr.push_back(title);
    process(arr);

}
