#ifndef NOTES_H
#define NOTES_H

//Графика и компоненты
#include <QWidget>
#include <QMenuBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QMessageBox>
//Дата время
#include <QDate>
#include <QTime>
//Библиотеки для работы с базой банных
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>

#include "officesystem.h"       //Статические методы
#include "popnotify.h"          //Уведомления

class Notes : public QWidget
{
    Q_OBJECT
public:
    Notes(QWidget *_parent_form, QWidget* _notes, QString _login);
    ~Notes();

private:
    QWidget *notes;                             //Виджет, куда будут отрисованы элементы заметок
    QWidget *parentForm;                        //Виджет, где будут отображаться уведомления
    QString login;                              //Логин текущего пользователя
    QString tabName;                            //Имя таблицы с заметками

    QSqlTableModel *modelDevice;                //Таблица
    QSqlQuery query;                            //Запрос к SQL

    PopNotify *popNotify;                       //Уведомления
    QPushButton *bt_enter;                      //Кнопка добавления
    QLabel *lb_date;                            //Отображение даты
    QTextEdit *te_text;                         //Текст заметки
    QTextEdit *te_term;                         //Текст срока
    QTableView *tv_table;                       //Таблица с заметками
    QVBoxLayout *vbox;                          //Компоновка

    void CreateComponents();                    //Создание и размещение компонентов
    void RefreshTable();                        //Обновление таблицы
    QString GetTabName();                       //Поулчение тимени таблицы

private slots:
    void slotEnterButton();                     //Слот кнопки добавления заметки
    void slotCustomMenuRequested(QPoint pos);   //Слот меню в таблице
    void slotRemoveRecord();                    //Слот удаледения записи из таблицы
};

#endif // NOTES_H
