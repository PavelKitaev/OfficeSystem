#ifndef SAFETY_H
#define SAFETY_H

//Графика и компоненты
#include <QWidget>
#include <QtGui>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QMenu>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QLineEdit>
#include <QInputDialog>

#include <QMediaPlayer>         //Звук
//Получение HTML
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
//Для работы с базой данных
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QMetaObject>          //Обработка слотов из потока

#include "popnotify.h"          //Уведомления
#include "officesystem.h"       //Статические методы
#include "settingfile.h"        //Файл настроек
#include "chatbase.h"

class Safety :  public QObject
{
    Q_OBJECT
public:
    Safety(SOCKET _connected, QWidget *_safety, QWidget *_pupNotify_form, QString _login);
    virtual ~Safety();
    void run();

private:
    const QString tableName = "of_safety_sensors";  //Имя таблицы

    QTimer *timer;

    SOCKET connectedSocket;
    QMediaPlayer *player;                           //Плеер для воспроизведения звука

    QWidget *safety;                                //Виджет
    QWidget *pupNotifyForm;                         //Виджет для отображения уведомлений
    QString login;                                  //Логин пользователя
    QString role;                                   //Вид доступа
    PopNotify *popNotify;                           //Уведомления

    QByteArray content;                             //Для записи полученных данных
    QWidget *header;                                //Шапка виджета
    QLabel *headerName;                             //Текст в шапке

    bool safetyOk;                                  //Включена ли охрана

    QPushButton *bt_safety;                         //Кнопка взятия под охрану
    QTableView *tableView;                          //Таблица
    QSqlTableModel  *modelDevice;                   //Таблица из базы данных
    QSqlQuery query;                                //Запросы к базе

    QVector<QLabel*> lbs;                           //Указатели на label
    QMap<QString, QString> dataTable;               //Соответствие ИД сексора имени

    QVBoxLayout *vboxLabel;                         //Компоновка
    QHBoxLayout *hboxGeneral;

    //Компоненты для добавления данных в таблицу (только для администратора)
    QLineEdit *idSensor;
    QLineEdit *sensorName;
    QPushButton *insert;

    QString GetRole();                              //Вид доступа пользователя
    void CreateComponents();                        //Размещение компонентов
    void GetDataTable();                            //Получение данных из таблицы и запись их в соответствующий мап
    void CheckTable();                              //Проверка наличия таблицы
    void RegServer();                               //Регестрация на сервере в качестве клиента-охранника

private slots:
    void slotInsertDataInTable();                   //Добавление данных в таблицу
    void slotCustomMenuRequested(QPoint pos);       //Меню таблицы
    void slotRemoveRecord();                        //Удаление данных из таблицы
    void slotSafetyButton();                        //Обработка сигнала с кнопки "Взятия под охрану"
    void GetSafetyData();

public slots:
    void receive(QByteArray const& str);
};

#endif // SAFETY_H
