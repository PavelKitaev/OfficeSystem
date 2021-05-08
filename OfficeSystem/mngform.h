#ifndef MNGFORM_H
#define MNGFORM_H

#include "chatbase.h" //Чат
#include "notes.h" //Заметки
#include "userprofile.h" //Профиль

namespace Ui { class MngForm; }

class MngForm : public QWidget
{
    Q_OBJECT

public:
    explicit MngForm(SOCKET _connection = 0, QString _login = "", QWidget *parent = nullptr);
    ~MngForm();    

private:
    Ui::MngForm     *ui;
    PopNotify       *popNotify;        //Уведомления
    Log             *lg;               //Журнализирование действий
    ChatBase        *chat;             //Чат
    Notes           *notes;            //Заметки
    UserProfile     *userProfile;      //Профиль пользователя

    SOCKET          Connection;        //Подключение к серверу
    QString         login;             //Логин текущего пользователя
    QImage          bg;                //Изображение фона

private:
    void paintEvent(QPaintEvent* e);   //Событие отрисовки фона
};

#endif // MNGFORM_H
