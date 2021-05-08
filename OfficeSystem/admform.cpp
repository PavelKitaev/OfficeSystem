#include "admform.h"
#include "ui_admform.h"

AdmForm::AdmForm(SOCKET _connection, QString _login, QWidget *parent) : QWidget(parent), ui(new Ui::AdmForm)
{
    ui->setupUi(this);
    bg.load(":/image/background.png");          //Фон
    this->repaint();

    popNotify = new PopNotify();                //Уведомления
    lg = new Log();                             //Логирование
    Connection = _connection;                   //Подключение к серверу
    login = _login;                             //Логин текущего пользователя

    CreateTable();                              //Создание таблицы

    ui->lb_serverTime->setText("Сервер недоступен"); //Первичная установка текста В Label, где в дальшейшем будет информация о времени сервера

    if (Connection == NULL) //Если сервер недоступен
    {
        //Устанавливаем на вкладку с чатом соответствующую надпись
        QHBoxLayout *chatLayout = new QHBoxLayout(ui->chat);
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
        GetServerTime(); //Получение даты и времени запуска сервера

        //Создание чата
        chat_class = new ChatBase(Connection, ui->chat, login, this);
        chat_class->start();

        //Создание вкладки безопасности
        safety = new Safety(Connection, ui->safetyTab,this, login);

        //Создание связи сигнал-слот для передачи данных из чата в безопасность
        QObject::connect(chat_class, &ChatBase::sendMsg, safety, &Safety::receive);
    }

    //Создание заметок
    notes_class = new Notes(this, ui->notes, login);
    //Создание профиля
    user_profile = new UserProfile(this, ui->profile, login);
}

AdmForm::~AdmForm()
{
    delete ui;
}

void AdmForm::GetServerTime()
{
    QString dataTime; //Время работы сервера
    int msgSize;      //Ответ с сервера (размер)
    char* msg;        //Текст ответа

    const char* request = "SERVER_DATATIME::"; //Запрос на сервер
    int requestSize = strlen(request)+1;

    //Отправляем запрос на сервер
    send(Connection, (char*)&requestSize, sizeof(int), 0);
    send(Connection, request, requestSize, 0);

    if (recv(Connection, (char*)&msgSize, sizeof(int), 0) > 0) //Принимаем размер сообщения
    {
        msg = new char[msgSize + 1]; //Создаем чар* нужного размера
        msg[msgSize] = '\0';   //Конец строки

        if (recv(Connection, msg, msgSize, 0) > 0) //Принимаем данные
        {
            //Отделяем служебные данные от полезной информации
            int i = 0;
            while (msg[i] != '\0')
            {
                if (msg[i] == ':')
                {
                    i = i + 2;
                    while (msg[i] != '\0')
                    {
                        dataTime += msg[i];
                        i++;
                    }
                    break;
                }
                i++;
            }
        }

        ui->lb_serverTime->setText("Сервер работает с " + dataTime); //Устанавливаем текст
    }
}

void AdmForm::CreateTable() //Создание таблицы
{
    modelDevice = new QSqlTableModel(this);         //Модель таблицы из базы банных
    modelDevice->setTable("of_user_accounts");      //Имя таблицы
    modelDevice->select();

    ui->tableView->setModel(modelDevice);           //Установка таблицы в виджет
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu); //Создание меню
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    //Устанавливаем нужную ширину на каждый столбец
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //Ширина столбцов по содержимому
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);   //Заполнение пустого пространства
    ui->tableView->horizontalHeader()->resizeSection(6, 180);   //Фиксированная ширина
    ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ui->tableView->hideColumn(0); //Скрываем столбцы с вопросом, ответом и паролем
    ui->tableView->hideColumn(2);
    ui->tableView->hideColumn(3);
    ui->tableView->hideColumn(4);

    QScrollBar *vert_scrollbar = new QScrollBar(); //Пользовательский скроллбар для таблицы
    vert_scrollbar->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar()); //Поулчаем стиль

    ui->tableView->setVerticalScrollBar(vert_scrollbar); //Устанавливаем скроллбар

    //Красивые имена столбцов
    modelDevice->setHeaderData(1, Qt::Horizontal, "Логин");
    modelDevice->setHeaderData(5, Qt::Horizontal, "ФИО");
    modelDevice->setHeaderData(6, Qt::Horizontal, "Вид доступа");
    modelDevice->setHeaderData(7, Qt::Horizontal, "Подразделение");

    modelDevice->select(); //Обновление
}

void AdmForm::RefreshTable() //Обновление таблицы
{
    modelDevice->select(); //Обновление

    popNotify->setPopupText("Таблица обновлена"); //Уведомление
    popNotify->show(this);
}

void AdmForm::slotCustomMenuRequested(QPoint pos) //Слот меню в таблице
{
    QMenu * menu = new QMenu(this); //Создание меню
    QAction * deleteDevice = new QAction(("Удалить запись из таблицы"), this); //Создае действий
    QAction * resetPassword = new QAction(("Сбросить пароль пользователя"), this);
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); // Обработчик удаления записи
    connect(resetPassword, SIGNAL(triggered()), this, SLOT(slotResetPassword())); // Обработчик сброса пароля
    menu->addAction(deleteDevice); //Добавление действий в меню
    menu->addAction(resetPassword);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void AdmForm::slotRemoveRecord() //Слот удаледения таблицы
{
    int row = ui->tableView->selectionModel()->currentIndex().row(); //Выясняем, какая из строк была выбрана
    if (row >= 0)
    {
        if (QMessageBox::warning(this, ("Удаление записи"), ("Вы уверены, что хотите удалить эту запись?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            //При отрицательном ответе делаем откат действий и закрываем диалог без удаления записи
            QSqlDatabase::database().rollback();
            return;
        }
        else
        {
            //В противном случае производим удаление записи
            if(!modelDevice->removeRow(row))
            {
                QMessageBox::warning(this, ("Уведомление"),
                                     ("Не удалось удалить запись\n"
                                      "Возможно она используется другими таблицами\n"));
            }
            else
            {
                lg->WriteMsg("Удалена запись из таблицы.");
                RefreshTable();
            }

        }
    }
}

void AdmForm::slotResetPassword() //Сброс пароля
{
    //Выясняем, какая из строк была выбрана
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if(row >= 0)
    {
        if (QMessageBox::warning(this, ("Сброс пароля"), ("Вы уверены, что хотите сбросить пароль у данного пользователя?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            //При отрицательном ответе делаем откат действий и закрываем диалог без удаления записи
            QSqlDatabase::database().rollback();
            return;
        }
        else
        {
            //В противном случае производим сброс пароля
            int column = 0; //Номер столбца с логином пользователя
            QVariant login_from_table = modelDevice->index(row, column).data();
            QString loginT = login_from_table.toString();

            int size_pass = 10; //Размер пароля
            QString new_password;


            for(int i = 0; i < size_pass; i++) //Создание нового пароля
            {
                int sumbol = QRandomGenerator::global()->bounded(48, 90); //48-90 - диапазон индексов символов по таблице ASCII
                char temp = sumbol;
                new_password += temp;
            }

            if (query.exec("UPDATE of_user_accounts SET password = '" + new_password + "' WHERE login = '" + loginT + "'")) //Меняем пароль
            {
                query.exec("UPDATE of_user_accounts SET question = NULL WHERE login = '" + loginT + "'"); //Сбрасываем секретный вопрос
                query.exec("UPDATE of_user_accounts SET answer = NULL WHERE login = '" + loginT + "'"); //И ответ

                lg->WriteMsg("Для пользователя " + login + " сброшены пароль и данные для восстановления.");

                QMessageBox msg;
                msg.setText("Новый пароль - " + new_password);
                msg.exec();
            }
            else
            {
                popNotify->setPopupText("Ошибка при сбросе пароля. Попробуйте еще раз");
                popNotify->show(this);
            }

        }
    }
}

void AdmForm::on_bt_reguser_clicked()
{
    QString loginForm = ui->le_login->text();
    QString passForm = ui->le_pass->text();
    QString role; //Заполняется из выпадающего списка

    QString fioForm = ui->le_fio->text();   //ФИО
    QString deptFrom = ui->le_unit->text(); //Отдел

    bool controlLogin = loginForm.length() > 4;
    bool controlData = true;

    for (int i = 0; i < loginForm.length(); i++) //Проверка логина на пробелы и двоеточия
    {
        if (loginForm[i] == " " || loginForm[i] == ":")
        {
            controlLogin = false;
            break;
        }
    }

    if (!controlLogin)
    {
        controlData = false;
        popNotify->setPopupText("Некорректный логин");
        popNotify->show(this);
    }
    else  //Проверка логина на уникальность
    {
        QString query_login = "SELECT login FROM of_user_accounts WHERE login = '" + loginForm + "'";
        query.exec(query_login);
        query.next();
        QString loginBd = query.value(0).toString();

        if  (loginBd != "")
        {
            popNotify->setPopupText("Такой логин уже зарегистрирован");
            popNotify->show(this);
            controlData = false;
        }
    }

    if (passForm == "")
    {
        controlData = false;
        popNotify->setPopupText("Некорректный пароль");
        popNotify->show(this);
    }

    if (ui->cb_role->currentIndex() == 0)
    {
        controlData = false;
        popNotify->setPopupText("Выберите вид доступа");
        popNotify->show(this);
    }
    else
    {
        role = ui->cb_role->itemText(ui->cb_role->currentIndex()); //Получение текста по индексу
    }

    if (controlData) //Если все хороршо - регистрируем
    {
        QString query_text = "INSERT INTO of_user_accounts (login, password, role, fio, dept) "
                             "VALUES ('" + loginForm + "', '" + passForm + "', '" + role + "', '" + fioForm + "', '" + deptFrom + "')";

        if (!query.exec(query_text)) //Если ошибка
        {
            lg->WriteMsg(query.lastError().text());
            popNotify->setPopupText(query.lastError().text());
            popNotify->show(this);
        }
        else
        {
            //Проверяем существование таблицы с именами для чата
            query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = 'of_logins_to_names')");
            query.next();

            if (!query.value(0).toBool()) //Если не существует - создаем
            {
                query.exec("CREATE TABLE IF NOT EXISTS of_logins_to_names (\
                           chat_name character varying(50),\
                           login character varying(30),\
                           PRIMARY KEY (chat_name),\
                           CONSTRAINT fk_ltn\
                           FOREIGN KEY (login) REFERENCES of_user_accounts(login)\
                           ON UPDATE CASCADE\
                           ON DELETE CASCADE)");
            }

            //Добавляем логин с имя для чата в отдельную таблицы
            QString chatName;
            int i = 0, count = 0;

            while(fioForm[i] != '\0') //Забираем фамилию и имя
            {
                if (fioForm[i] == ' ')
                {
                    count++;
                }

                if (count == 2)
                {
                    break;
                }

                chatName += fioForm[i];
                i++;
            }

            query.exec("INSERT INTO of_logins_to_names VALUES ('" + chatName + "', '" + loginForm + "')"); //Добавляем данные в таблицу

            lg->WriteMsg("Пользователь " + loginForm + " зарегистрирован.");
            RefreshTable(); //Вызываем отрисовку таблицы
        }
    }
}

void AdmForm::on_bt_refresh_clicked() //Отрисовка таблицы
{
    RefreshTable();
}

void AdmForm::paintEvent(QPaintEvent *e) //Фон
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
