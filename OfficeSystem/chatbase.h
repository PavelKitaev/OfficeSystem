#ifndef CHATBASE_H
#define CHATBASE_H

//Графика и компоненты
#include <QWidget>
#include <QtGui>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
//Дата и время
#include <QDate>
#include <QTime>

#include <QTextCodec>       //Кодировка текста
#include <QMediaPlayer>     //Воспроизведение звуков
#include <QMetaObject>      //Работа с слотами из другого потока
#include <QtSql/QSqlQuery>  //Запросы к базе данных

#include "popnotify.h"      //Уведомления
#include "officesystem.h"   //Статические методы
#include "safety.h"

class ChatBase : public QThread
{
    Q_OBJECT
private:
    SOCKET connectedSocket;                 //Подключение
    QWidget *chatWidget;                    //Виджет чата
    QWidget *pupNotifyForm;                 //Виджет для уведомлений
    QWidget *usersWidget;                   //Виджет с пользователями
    PopNotify *popNotify;                   //Уведомления

    QSqlQuery query;                        //Запросы к БД

    QVector<QPushButton*> btns;             //Указатели на кнопки
    QVector<QString> unread;                //Для фиксирования непрочитанных диалогов
    QVector<QString> allUsersVec;           //Все пользователи
    QVector<QString> userOnline;            //Пользователи онлайн
    QMap<QString, QString> chatNames;       //Имена для чатов

    QString login;                          //Логин текущего пользователя
    QString loginDst;                       //Логин получателя
    QString tabName;                        //Имя таблицы с историей сообщений
    int countButton = 0;                    //Количество кнопок
    int allUsers;                           //Количество всех пользователей

    QDateTime time;                         //Время

    QPushButton *update;                    //Обновление пользователей онлайн
    QPushButton *enter;                     //Отправка сообщения
    QPushButton *allHistory;                //Показ всей истории
    QLabel *label;                          //Вывод имени текущего собеседника
    QTextEdit *fieldMsg;                    //Поле для ввода сообщения
    QTextBrowser *historyMsg;               //Поле для вывода истории переписки

    QMediaPlayer *player;                   //Звук сообщения

    //Менеджеры компоновки
    QVBoxLayout *rightButtons;
    QHBoxLayout *hbox;

    QString NameToLogin(QString name);      //Получение логина по имени
    QString LoginToName(QString login);     //Получение имени по логину
    void GetAllUsers();                     //Получение всех пользователей из базы
    void ChechTableInteraction();           //Проверка наличия таблицы
    void CreateComponents();                //Создание и размещение компонентов
    void PrintMessages(int quantity);       //Показ сообщений

public:
    ChatBase(SOCKET _connected, QWidget* _chat, QString _login, QWidget *pupNotifyForm);
    virtual ~ChatBase();
    void run();                             //Цикличное слушание сокета

public slots:
    void UpdateButton(QString _msg);        //Обновление кнопок
    void PrintReceivedMessage(QByteArray _msg); //Вывод полученного сообщения от сервера

    void slotUpdateButton();                //Слот обновлдения пользователей
    void slotGetUserDst();                  //Слот получения логина собеседника
    void slotAllHistory();                  //Слот кнопки вывода всей истории сообщений
    void slotEntorButton();                 //Слот кнопки отправки сообщения

signals:
    void sendMsg(QByteArray const& msg);

};

#endif // CHATBASE_H
