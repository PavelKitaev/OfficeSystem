#ifndef MAINFORM_H
#define MAINFORM_H

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>          //Сообщения
#include <QInputDialog>         //Диалоговые окна
#include <QPaintEvent>          //Отрисовка фона
#include <QWidget>

#include <QtSql/QSqlDatabase>   //Библиотеки для работы с базой банных
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <winsock2.h>           //Для работы с сетью
#include <windows.h>

#include "popnotify.h"          //Уведомления
#include "log.h"                //Журнализирование
#include "settingfile.h"        //Работа с файлом настроек
#include "officesystem.h"       //Статические методы

QT_BEGIN_NAMESPACE
namespace Ui { class MainForm; }
QT_END_NAMESPACE

class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = nullptr);
    ~MainForm();

private:
    Ui::MainForm    *ui;                                    //Элементы формы
    Log             *lg;                                    //Журнализирование событий
    PopNotify       *popNotify;                             //Уведомления
    SettingFile     *settingFile;                           //Работа с файлом настроек
    SOCKET          Connection;                             //Подключение к серверу
    QSqlQuery       query;                                  //Запросы к базе данных
    QImage          bg;                                     //Изображение фона
    QString         login;                                  //Логин пользователя
    DWORD           threadId;                               //Для получения раскладки клавиатуры
    HKL             hkl;                                    //Идентификатор ввода данных национального языка
    int             code;                                   //Код раскладки
    bool            serverOk;                               //Наличие подключения к серверу
    bool            accountTable;                           //Наличие таблицы с данными аккаунтов пользователей

private:
    void CheckTableAccounts();                              //Проверка наличия таблицы с аккаунтами
    void EditSettingFile();                                 //Редактирование файла настроек
    int  ConnectionToServer(QString address, QString port); //Подключение к серверу
    int  ErrorConnectionServer(QString text_error);         //Обработка неудачного подключения к серверу
    bool CheckCapsLock();                                   //Проверка активности клавиши CapsLock
    void SuccessLogin(QString _login, QString role);        //Открытие основного окна программы
    void keyPressEvent(QKeyEvent *event);                   //Событие обработки нажатия клавиш (капслок, раскладка клавиатуры)
    void paintEvent(QPaintEvent* e);                        //Событие отрисовки фона

private slots:
    void on_bt_enter_clicked();                             //Обработчик сигнала кнопки "Вход"
    void on_bt_help_password_clicked();                     //Обработчик сигнала кнопки "Забыли пароль"
};
#endif // MAINFORM_H
