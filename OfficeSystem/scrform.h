#ifndef SCRFORM_H
#define SCRFORM_H

#include <QNetworkAccessManager>    //Работа с веб-страницами

#include "chatbase.h"               //Чат
#include "notes.h"                  //Заметки
#include "safety.h"                 //Охрана
#include "userprofile.h"            //Профиль

namespace Ui { class ScrForm; }

class ScrForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScrForm(SOCKET _connection = 0, QString _login = "", QWidget *parent = nullptr);
    ~ScrForm();

private:
    Ui::ScrForm *ui;
    PopNotify       *popNotify;         //Уведомления
    Log             *lg;                //Журнализирование действий
    ChatBase        *chat;              //Чат
    Notes           *notes;             //Заметки
    UserProfile     *userProfile;       //Профиль пользователя
    Safety          *safety;

    SOCKET          Connection;         //Подключение к серверу
    QString         login;              //Логин текущего пользователя
    QImage          bg;                 //Изображение фона

private:
    void paintEvent(QPaintEvent* e);    //Событие отрисовки фона
};

#endif // SCRFORM_H
