#ifndef ADMFORM_H
#define ADMFORM_H

#include <QWidget>

#include <QtSql/QSqlDatabase>   //Библиотеки для работы с базой банных
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "chatbase.h" //Чат
#include "notes.h" //Заметки
#include "userprofile.h" //Профиль
#include "officesystem.h" //+
#include "safety.h" //+

namespace Ui { class AdmForm; }

class AdmForm : public QWidget
{
    Q_OBJECT

public:
    AdmForm(SOCKET _connection = 0, QString _login = "", QWidget *parent = nullptr);
    ~AdmForm();

private:
    Ui::AdmForm     *ui;
    PopNotify       *popNotify;         //Уведомления
    Log             *lg;                //Журнализирование действий
    ChatBase        *chat_class;        //Чат
    Notes           *notes_class;       //Заметки
    UserProfile     *user_profile;      //Профиль пользователя
    Safety          *safety;            //Безопасность

    QSqlTableModel  *modelDevice;       //Таблица из базы данных
    QSqlQuery       query;              //Запросы к базе данных
    SOCKET          Connection;         //Подключение к серверу
    QString         login;              //Логин текущего пользователя
    QImage bg;                          //Изображение фона


private:
    void GetServerTime();               //Получение даты и времени, когда был запущен сервер
    void CreateTable();                 //Создание таблицы
    void RefreshTable();                //Обновление таблицы
    void paintEvent(QPaintEvent* e);    //Отрисовка фона

private slots:
    void slotCustomMenuRequested(QPoint pos);   //Меню таблицы
    void slotRemoveRecord();                    //Удаление записи
    void slotResetPassword();                   //Сброс пароля

    void on_bt_reguser_clicked();               //Обработка нажатия кнопки "Зарегистрировать"
    void on_bt_refresh_clicked();               //Обработка нажатия кнопки "Обновить таблицу"
};

#endif // ADMFORM_H
