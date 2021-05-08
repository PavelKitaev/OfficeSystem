#ifndef USERPROGILE_H
#define USERPROGILE_H

#include <QWidget>
#include <QtSql/QSqlQuery>      //Запрос к базе данных
#include <QPushButton>          //Кнопки
#include <QLineEdit>            //Поля ввода
#include <QMessageBox>          //Сообщения
#include <QTextEdit>            //Поля ввода
#include <QInputDialog>         //Диалоговые окна
#include <QSpacerItem>          //Спайсеры

#include "popnotify.h"          //Уведомления
#include "log.h"                //Журнализирование
#include "officesystem.h"       //Статические методы

class UserProfile : public QObject
{
    Q_OBJECT
public:
    UserProfile(QWidget *_parentForm, QWidget* _profile, QString _login);

private:
    QString     login;              //Логин текущего полльзователя
    QWidget     *profile;           //Основной виджет для отрисовки
    QWidget     *parentForm;        //Виджет родителя, для вывода уведомлений
    PopNotify   *popNotify;         //Уведомления
    Log         *lg;                //Журнализирование действий

    QLineEdit *le_oldPass;          //Поле ввода старого пароля
    QLineEdit *le_newPass;          //Поле ввода нового пароля
    QPushButton *bt_change;         //Кнопка смены пароля

    QLineEdit *le_question;         //Поле ввода вопроса
    QLineEdit *le_answer;           //Поле ввода ответа
    QPushButton *bt_setQuestion;    //Кнопка установки вопроса-ответа для восстановления парооля

    QPushButton *bt_info;           //Кнопка смены информации об организации (только для администратора)

    QDialog *dialog;                //Диалог для редактирования информации об организации
    QTextEdit *te_newInfo;          //Поле с новой информацией об организации
    QWidget *orgInfo;               //Виджет для отображения информации об организации

    QLabel *lb_fio;                 //Для отображения ФИО
    QLabel *lb_dept;                //Для отображения подразделения
    QLabel *lb_role;                //Для отображения вида доступа
    QLabel *lb_orgInfo;             //Для отображения информации об организации
    QLabel *lb_question;            //Для отображен наличия вопроса (задан/не задан)

    void CreateComponents();        //Создание основных компонентов и их компоновка на форме
    void GetData();                 //Получение данных из базы данных
signals:
    void sendMsg(QByteArray const& msg);

private slots:
    void slotChangePassword();      //Смена пароля
    void slotSetQuestion();         //Смена/установка вопроса
    void slotNewInfoOrg();          //Создание и показ диалога для редактирования информации об организации (только для администратора)
    void slotSetNewInfoOrg();       //Установка новой информации об организации (только для администратора)
};

#endif // USERPROGILE_H
