#include "mngform.h"
#include "ui_mngform.h"

MngForm::MngForm(SOCKET _connection, QString _login, QWidget *parent) : QWidget(parent), ui(new Ui::MngForm)
{
    ui->setupUi(this);
    bg.load(":/image/background.png");      //Фон
    this->repaint();

    popNotify = new PopNotify();            //Уведомления
    lg = new Log();                         //Логирование
    Connection = _connection;               //Подключение к серверу
    login = _login;                         //Логин текущего пользователя

    //Создание чата
    chat = new ChatBase(Connection, ui->chatTab, login, this);
    chat->start();

    //Создание заметок
    notes = new Notes(this, ui->noteTab, login);
    userProfile = new UserProfile(this, ui->profileTab, login);
}

MngForm::~MngForm()
{
    delete ui;
}

void MngForm::paintEvent(QPaintEvent *e) //Обновление фона
{
    if (bg.isNull())
    {
        QWidget::paintEvent(e);
        return;
    }

    QPainter p(this);

    qreal xs = qreal(width()) / qreal(bg.width());
    qreal ys = qreal(height()) / qreal(bg.height());

    if (xs != 1 || ys != 1)
    {
        QImage img;
        int cx = qRound(e->rect().x() / xs);
        int cy = qRound(e->rect().y() / ys);
        int cw = qRound(e->rect().width() / xs);
        int ch = qRound(e->rect().height() / ys);
        cw = qMin(cw, bg.width() - cx);
        ch = qMin(ch, bg.height() - cy);
        img = bg.copy(cx, cy, cw, ch);
        cw = qRound(cw * xs);
        ch = qRound(ch * ys);
        img = img.scaled(cw, ch, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        p.drawImage(e->rect().topLeft(), img);
    }
    else
    {
        p.drawImage(e->rect().topLeft(), bg, QRect(e->rect().x(), e->rect().y(),
                           qMin(e->rect().width(), bg.width()), qMin(e->rect().height(), bg.height())));
    }
}
