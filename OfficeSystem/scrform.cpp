#include "scrform.h"
#include "ui_scrform.h"

ScrForm::ScrForm(SOCKET _connection, QString _login, QWidget *parent) : QWidget(parent), ui(new Ui::ScrForm)
{
    ui->setupUi(this);
    bg.load(":/image/background.png");  //Фон

    popNotify = new PopNotify();        //Уведомления
    lg = new Log();                     //Логирование
    Connection = _connection;           //Подключение к серверу
    login = _login;                     //Логин текущего пользователя

    if (Connection == NULL) //Если сервер недоступен
    {
        //Устанавливаем на вкладку с чатом соответствующую надпись
        QHBoxLayout *chatLayout = new QHBoxLayout(ui->chatTab);
        QLabel *noServerLabelC = new QLabel();
        noServerLabelC->setText("Нет подключения к серверу");
        noServerLabelC->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));
        chatLayout->addWidget(noServerLabelC, 0, Qt::AlignCenter);

        //Устанавливаем на вкладку безопасности соотведствующую надпись
        QHBoxLayout *safetyLayout = new QHBoxLayout(ui->safetyTab);
        QLabel *noServerLabelS = new QLabel();
        noServerLabelS->setText("Нет подключения к серверу");
        noServerLabelS->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));
        safetyLayout->addWidget(noServerLabelS, 0, Qt::AlignCenter);
    }
    else
    {
        //Создание чата
        chat = new ChatBase(Connection, ui->chatTab, login, this);
        chat->start();

        //Создание вкладки безопасности
        safety = new Safety(Connection, ui->safetyTab,this, login);

        //Создание связи сигнал-слот для передачи данных из чата в безопасность
        QObject::connect(chat, &ChatBase::sendMsg, safety, &Safety::receive);
    }

    //Создание заметок
    notes = new Notes(this, ui->noteTab, login);
    //Создание профиля
    userProfile = new UserProfile(this, ui->profileTab, login);
}

ScrForm::~ScrForm()
{
    delete ui;
}

void ScrForm::paintEvent(QPaintEvent *e) //Отрисовка фона
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
