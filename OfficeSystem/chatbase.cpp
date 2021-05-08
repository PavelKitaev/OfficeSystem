#include "chatbase.h"

ChatBase::ChatBase(SOCKET _connected, QWidget* _chat, QString _login, QWidget *_pupNotify_form)
{
    ChechTableInteraction();                //Проверка таблицы
    connectedSocket = _connected;           //Подключение к серверу
    chatWidget = _chat;                     //Виджет для чата
    login = _login;                         //Логин текущего пользователя
    allUsers = 0;                           //Количество пользователей

    CreateComponents();                     //Создание основных элементов виджета
    pupNotifyForm = _pupNotify_form;        //Форма отображения уведомлений
    popNotify = new PopNotify();            //Уведомления

    slotUpdateButton();                     //Первое обновление пользователей

    QTimer *timer = new QTimer(this);       //Таймер для обновления пользователей раз в 1 минуту
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateButton()));
    timer->start(60000);                    //Обновление списка пользователей онлайн раз в минуту
}

ChatBase::~ChatBase()
{

}

QString ChatBase::NameToLogin(QString _name)
{
    QString res = "none";
    
    //Ищем в мапе совпадение, возвращаем значение
    QMapIterator<QString, QString> itMap(chatNames);
    while (itMap.hasNext())
    {
        itMap.next();

        if (itMap.key() == _name)
        {
            res = itMap.value();
            break;
        }
    }

    return res;
}

QString ChatBase::LoginToName(QString _login)
{
    QString res = "none";

    //Ищем в мапе совпадение, возвращаем ключ
    QMapIterator<QString, QString> itMap(chatNames);
    while (itMap.hasNext())
    {
        itMap.next();

        if (itMap.value() == _login)
        {
            res = itMap.key();
            break;
        }
    }

    return res;
}

void ChatBase::GetAllUsers()
{
    //Получаем количество записей в таблице
    query.exec("SELECT count(*) FROM of_user_accounts");
    query.next();
    int res = query.value(0).toInt();

    if (allUsers != res) //Если оно изменилось - обновляем даннные в векторе
    {
       allUsers = res;
       query.exec("SELECT login FROM of_user_accounts"); //Получаем все логины из таблицы
       allUsersVec.clear(); //Очищаем вектор

       while(query.next()) //Заносим новые данные
       {
           allUsersVec.push_back(query.value(0).toString());
       }

       query.exec("SELECT * FROM of_logins_to_names"); //Получаем соответствия логинов именам

       while (query.next()) //Заносим в мап
       {
           chatNames.insert(query.value(0).toString(), query.value(1).toString());
       }
    }
}

void ChatBase::ChechTableInteraction()
{
    //Проверяем существование таблицы
    query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = 'of_interaction_users')");
    query.next();

    if (!query.value(0).toBool()) //Создаем в случае отсутствия
    {
        query.exec("CREATE TABLE IF NOT EXISTS of_interaction_users (\
                   login_one character varying(30),\
                   login_first character varying(30),\
                   tabl_name character varying(60),\
                   PRIMARY KEY (tabl_name),\
                   CONSTRAINT fk_temp\
                   FOREIGN KEY (login_one) REFERENCES of_user_accounts(login)\
                   ON UPDATE CASCADE\
                   ON DELETE CASCADE,\
                   FOREIGN KEY (login_first) REFERENCES of_user_accounts(login)\
                   ON UPDATE CASCADE\
                   ON DELETE CASCADE);");
    }
}

void ChatBase::CreateComponents()
{
    player = new QMediaPlayer;                                          //Создаем плеер
    player->setMedia(QUrl("qrc:/sound/message-sound.wav"));             //Устанавливаем звук сообщений

    rightButtons = new QVBoxLayout();                                   //Столбец для кнопок пользователей
    hbox = new QHBoxLayout(chatWidget);
    QSpacerItem *hotSpac = new QSpacerItem(200, 1, QSizePolicy::Minimum, QSizePolicy::Minimum);
    rightButtons->addSpacerItem(hotSpac);

    //Кнопка обновления списка пользователей онлайн
    update = new QPushButton(chatWidget);
    update->setGeometry(1350, 50, 246, 45);
    update->setText("Обновить");
    update->setStyleSheet(OfficeSystem::GetStyleSheetButton());
    connect(update, SIGNAL(clicked()), this, SLOT(slotUpdateButton()));

    //Кнопка отправки сообщения
    QPixmap pixmap(":/image/send.png"); //Иконка для кнопки
    QIcon ButtonIcon(pixmap);

    enter = new QPushButton(chatWidget);
    enter->setGeometry(820, 800, 70, 70);
    enter->setMaximumSize(70, 70);
    enter->setIcon(ButtonIcon);
    enter->setIconSize(pixmap.rect().size());
    enter->setEnabled(false);
    enter->setStyleSheet(OfficeSystem::GetStyleSheetButtonForIcon());
    connect(enter, SIGNAL(clicked()), this, SLOT(slotEntorButton()));

    //Кнопка запроса всей истории сообщений
    allHistory = new QPushButton(chatWidget);
    allHistory->setGeometry(1350, 100, 246, 45);
    allHistory->setText("Показать всю историю");
    allHistory->setEnabled(false);
    allHistory->setStyleSheet(OfficeSystem::GetStyleSheetButton());
    connect(allHistory, SIGNAL(clicked()), this, SLOT(slotAllHistory()));

    //Отображение логина текущего собеседника
    label = new QLabel();
    label->setGeometry(50, 800, 500, 60);
    label->setText("Выберите собеседника");
    label->setStyleSheet(OfficeSystem::GetStyleSheetLabel());

    //Поле ввода сообщения
    QScrollBar *vert_scrollbar_field = new QScrollBar();
    vert_scrollbar_field->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());

    fieldMsg = new QTextEdit();
    fieldMsg->setMaximumSize(1500, 100);
    fieldMsg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    fieldMsg->setStyleSheet(OfficeSystem::GetStyleSheetTextEdit());
    fieldMsg->setVerticalScrollBar(vert_scrollbar_field);

    //Поле для вывода истории переписки
    QScrollBar *vert_scrollbar_history = new QScrollBar();
    vert_scrollbar_history->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());
    historyMsg = new QTextBrowser();
    historyMsg->setGeometry(20, 20, 200, 200);
    historyMsg->setMaximumSize(1350, 500);
    historyMsg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    historyMsg->setStyleSheet(OfficeSystem::GetStyleSheetTextBrowser());
    historyMsg->setVerticalScrollBar(vert_scrollbar_history);

    //--------------------------------------------- К О М П О Н О В К А ---------------------------------------------//

    //Основные отступы
    QSpacerItem *bottom_general = new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Fixed); //Вертикальный отступ от нижнего края формы
    QSpacerItem *top_general = new QSpacerItem(1, 210, QSizePolicy::Fixed, QSizePolicy::Fixed); //Вертикальный отступ от нижнего края формы

    //1. Шапка диалога
    QWidget *header_dialog = new QWidget(chatWidget); //Создаем виджет
    header_dialog->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top")); //Задаем стиль
    header_dialog->setMaximumSize(1350, 40); //Задаем максимальные размеры
    QHBoxLayout *header_layout = new QHBoxLayout(header_dialog); //Менеджер компоновки для шапки
    header_layout->addWidget(label, 0, Qt::AlignCenter); //Помещаем в шапку label с именем собеседника

    //2. Набор виджетов для отображения пользователей онлайн
    QVBoxLayout *user_global = new QVBoxLayout(); //Основной менеджер компоновки
    QWidget *header_user = new QWidget(chatWidget); //Создаем виджет
    header_user->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top")); //Задаем стиль
    header_user->setMaximumSize(250, 40); //Задаем максимальные размеры
    header_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QLabel *lb_user = new QLabel("Пользователи", header_user);
    lb_user->setStyleSheet(OfficeSystem::GetStyleSheetLabel());

    QHBoxLayout *user_layout = new QHBoxLayout(header_user); //Менеджер компоновки для шапки
    user_layout->addWidget(lb_user, 0, Qt::AlignCenter); //Помещаем в шапку label с текстом
    user_global->addSpacerItem(top_general);  //Отступ
    user_global->addWidget(header_user, 0, Qt::AlignTop); //Шапка

    //3. Виджет для отображения пользователей
    usersWidget = new QWidget(chatWidget);
    usersWidget->setStyleSheet(OfficeSystem::GetStyleSheetWidget("bottom"));
    usersWidget->setMaximumSize(250, 500);
    usersWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    user_global->addWidget(usersWidget, 1);

    rightButtons = new QVBoxLayout(usersWidget);
    QScrollArea * scrollArea = new QScrollArea(usersWidget);
    QScrollBar *vert_scrollbar_button = new QScrollBar();
    vert_scrollbar_button->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());

    scrollArea->setVerticalScrollBar(vert_scrollbar_button);
    scrollArea->setGeometry(0, 0, 500, 500);
    scrollArea->setWidgetResizable( true );
    rightButtons->addWidget( scrollArea );

    QWidget * container = new QWidget();

    scrollArea->setWidget( container );
    rightButtons = new QVBoxLayout(container);

    QSpacerItem *bottom_user = new QSpacerItem(1, 165, QSizePolicy::Fixed, QSizePolicy::Expanding); //Вертикальный отступ от нижнего края формы
    user_global->addSpacerItem(bottom_user);

    //4. Компоновка поля для ввода и кнопки
    QHBoxLayout *field_and_enter = new QHBoxLayout();
    field_and_enter->addWidget(fieldMsg);
    field_and_enter->addSpacing(20);
    field_and_enter->addWidget(enter);

    //5. Компоновка центральных элементов формы (Шапка, поле сообщений, поле ввода и кнопка отправки)
    QVBoxLayout *central_widgets = new QVBoxLayout();  //Вертикальный менеджер
    central_widgets->addSpacerItem(top_general); //Отступ
    central_widgets->addWidget(header_dialog); //Шапка
    central_widgets->addSpacing(1); //Пробел между виджетам
    central_widgets->addWidget(historyMsg); //Поле сообщений
    central_widgets->addSpacing(25); //Пробел между виджетам
    central_widgets->addLayout(field_and_enter);//, 0, Qt::AlignLeft); //Поле ввода сообщения
    central_widgets->addSpacerItem(bottom_general);

    //6. Компоновка левых кнопок (обновить, показать всю историю)
    QVBoxLayout *left_buttons = new QVBoxLayout(); //Вертикальный менеджер
    left_buttons->addSpacerItem(top_general); //Отступ
    left_buttons->addSpacing(45);
    left_buttons->addWidget(update, 0, Qt::AlignTop); //Кнопка обновления
    left_buttons->addSpacing(15); //Пробел между виджетами
    left_buttons->addWidget(allHistory, 1, Qt::AlignTop); //Кнопка истории сообщений

    //7. Копмоновка вертикальных менеджеров в горизонтальном менеджере
    hbox->addLayout(left_buttons); //Левые виджеты
    hbox->addSpacing(15); //Пробел
    hbox->addLayout(central_widgets); //Центральные виджеты
    hbox->addSpacing(15); //Пробел
    hbox->addLayout(user_global); //Правые виджеты (Кнопки с пользователями, которые онлайн)
}

void ChatBase::PrintMessages(int quantity)
{
    //Получаем название таблицы, где хранится история сообщений пользователей
    query.exec("SELECT tabl_name FROM of_interaction_users WHERE (login_one = '" + login
               + "' AND login_first = '" + loginDst + "') OR (login_one = '" + loginDst
               + "' AND login_first = '" + login + "')"); //Делаем запрос к БД
    query.next();

    tabName = query.value(0).toString();

    if (tabName == "") //Если таблицы не существует - создаем
    {
        tabName = login + "_" + loginDst;
        query.exec("INSERT INTO of_interaction_users VALUES ('" + login + "', '" + loginDst + "', '" + tabName.toLower() + "')");

        query.exec("CREATE TABLE " + tabName.toLower() + " (id_msg bigserial NOT NULL, dt character varying(17), login character varying(15), msg text, PRIMARY KEY(id_msg))");
    }
    else //Иначе загружаем историю сообщений из базы данных
    {
        query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = '" + tabName + "')");
        query.next();

        if (query.value(0).toBool())
        {
            query.exec("SELECT count(*) FROM " + tabName);
            query.next();
            int count = query.value(0).toInt();

            if (quantity == -1)
                query.exec("SELECT dt, login, msg FROM " + tabName);
            else
            {
                if (count <= quantity)
                    query.exec("SELECT dt, login, msg FROM " + tabName);
                else
                    query.exec("SELECT dt, login, msg FROM " + tabName + " OFFSET (SELECT count(*) FROM " + tabName + " ) - " + QString::number(quantity));
            }

            QString login_bd;
            QString msg_bd;
            QString dt;
            QString name_tb;

            historyMsg->clear(); //Очищаем поле

            while (query.next()) //Выводи полученные сообщения из базы
            {
                dt = query.value(0).toString();
                login_bd = query.value(1).toString();
                msg_bd = query.value(2).toString();
                name_tb = LoginToName(login_bd);

                if (login_bd == login)
                    historyMsg->append(dt + " Вы: " + msg_bd);
                else
                    historyMsg->append(dt + " " + name_tb + ": " + msg_bd);
            }
        }
        else
        {
            query.exec("CREATE TABLE " + tabName + " (id_msg bigserial NOT NULL, dt character varying(17), login character varying(15), msg text, PRIMARY KEY(id_msg))");
        }
    }
}

void ChatBase::UpdateButton(QString _msg)
{
    GetAllUsers(); //Получаем информацию о пользователях
    countButton = 0;
    userOnline.clear(); //Очищаем вектор с пользователями-онлайн

    //Считаем, сколько пользователей онлайн
    for (int i = 0; i < _msg.length(); i++)
    {
        if (_msg[i] == ':')
            countButton++;
    }

    int j = 0;
    for (int i = 0; i < countButton; i++)
    {
        QString temp;
        for (j; j < _msg.length(); j++) //Проходим по всему полученному тексту
        {
            if (_msg[j] !=  ':') //Если символ не разрелитель - записываем в темп
            {
                temp += _msg[j];
            }
            else if (_msg[j] == ':') //Если дошло до разделителя - записываем в вектор
            {
                j++;
                userOnline.push_back(temp);
                break;
            }
        }
    }

    if (btns.isEmpty() == false)    //Если вектор с указателями на кнопки не пуст
    {
        for (int i =0; i < btns.size(); i++)
        {
            delete btns[i];         //Удаляем указатель
        }
        btns.clear();               //Очищаем вектор
    }


    for (int i = 0; i < allUsers; i++)
    {
        QString newBtnText = allUsersVec[i]; //Пользователь из "Всех пользователей"
        QString userNameBtn;

        if (newBtnText != login)
        {
            QPushButton *t = new QPushButton(); //Создаем кнопку
            connect(t, SIGNAL(clicked()), this, SLOT(slotGetUserDst())); //Подключаем слот
            t->setMinimumSize(100, 45); //Минимальные размеры

            //Проверка, есть ли этот пользователь в списке онлайна
            QVector<QString>::iterator itOnline = std::find(userOnline.begin(), userOnline.end(), newBtnText);

            //Проверка, есть ли с этим пользователем непрочитанные сообщения
            QVector<QString>::iterator itUnread = std::find(unread.begin(), unread.end(), newBtnText);

            t->setText(LoginToName(newBtnText)); //Записываем текст в кнопку с ФИ пользователя

            if (itOnline != userOnline.end()) //Если пользователь онлайн
            {
                if (itUnread != unread.end()) //Если есть непрочитанные сообщения с этим пользователем
                {
                    t->setStyleSheet(OfficeSystem::GetStyleSheetButtonUnread());
                }
                else
                {
                    t->setStyleSheet(OfficeSystem::GetStyleSheetButton());
                }

                rightButtons->addWidget(t); //Добавляем в менеджер компоновки
                btns.push_back(t);  //Добавляем в вектор
            }
            else //Если не онлайн
            {
                if (itUnread != unread.end()) //Если есть непрочитанные сообщения с этим пользователем
                {
                    t->setStyleSheet(OfficeSystem::GetStyleSheetButtonUnread());
                }
                else
                {
                    t->setStyleSheet(OfficeSystem::GetStyleSheetButtonUserOffline());
                }

                rightButtons->addWidget(t); //Добавляем в менеджер компоновки
                btns.push_back(t);  //Добавляем в вектор
            }
        }
    }
}

void ChatBase::PrintReceivedMessage(QByteArray qb_msg)
{
    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //Кодировка
    QString _msg = codec->toUnicode(qb_msg);    //Устанавливаем кодировке

    QString login_dst; //Получатель
    QString login_srs; //Отправитель
    QString text; //Сообщение

    QDate cdate = QDate::currentDate(); //Дата
    QTime ctime = QTime::currentTime(); //Время

    int i = 0;

    //Достаем логин отправителя
    for (i; i < _msg.length(); i++)
    {
        if (_msg[i] == ":")
        {
            i = i+ 2;
            break;
        }
        login_srs += _msg[i];
    }

    //Достаем логин получателя
    for (i; i < _msg.length(); i++)
    {
        if (_msg[i] == ":")
        {
            i = i+ 2;
            break;
        }
        login_dst += _msg[i];
    }

    //Достаем само сообщение
    for (i; i < _msg.length(); i++)
    {
        text += _msg[i];
    }

   if (text != "") //Если сообщение не пустое - уведомляем
   {
       popNotify->setPopupText("Сообщение от пользователя \n" + LoginToName(login_srs));
       popNotify->show(pupNotifyForm);
       player->play();
   }

   if (loginDst == login_srs) //Если открыт диалог с этим пользователем - выводим сообщение сразу
   {
       historyMsg->append(cdate.toString(Qt::LocaleDate) + " " + ctime.toString(Qt::LocaleDate) + " " + LoginToName(loginDst) + ": " + text);
   }
   else //Если нет
   {
       unread.push_back(login_srs); //Фиксируем непрочитанное сообщение в ветор

       for (int i =0; i < btns.size(); i++) //Ищем в вектор отправителя и ставим на кнопку стиль непрочитанного сообщения
       {
           QString textBtn = NameToLogin(btns[i]->text());

           if (textBtn == login_srs)
           {
               btns[i]->setStyleSheet(OfficeSystem::GetStyleSheetButtonUnread());
           }
       }
   }
}

void ChatBase::slotUpdateButton()
{
    const char* request = "GET_USERS_ONLINE::";
    int request_size = strlen(request)+1;

    //Отправляем запрос на сервер
    send(connectedSocket, (char*)&request_size, sizeof(int), 0);
    send(connectedSocket, request, request_size, 0);
}

void ChatBase::slotGetUserDst()
{
    QPushButton *temp = (QPushButton*) sender();    //Получаем нажатую кнопку
    QString textOld = temp->text();                 //Имя (с кнопки)
    QString text = NameToLogin(textOld);            //Логин, который соответствует этому имени

    for (int i = 0; i < unread.size(); i++) //Ищем в векторе "непрочитанных" пользователя
    {
        if (unread[i] == text)
        {
            unread.remove(i); //Удаляем
            break;
        }
    }

    QVector<QString>::iterator itOnline = std::find(userOnline.begin(), userOnline.end(), text); //Проверка, есть ли этот пользователь в списке онлайна

    if (itOnline != userOnline.end()) //Если онлайн
    {
       temp->setStyleSheet(OfficeSystem::GetStyleSheetButton());
    }
    else
    {
        temp->setStyleSheet(OfficeSystem::GetStyleSheetButtonUserOffline());
    }

    label->setText(textOld); //Записываем текст из кнопки в лабел
    loginDst = text; //Записываем текст из кнопки в логин_дст

    historyMsg->clear();    //Очищаем
    PrintMessages(20);      //Выводим 20 сообщений

    allHistory->setEnabled(true); //Делаем доступным кнопки истории и отправки
    enter->setEnabled(true);
}

void ChatBase::slotAllHistory()
{
    this->PrintMessages(-1); //Вывод всей итории сообщений
}

void ChatBase::slotEntorButton()
{
    //Получаем дату и время
    QDate cdate = QDate::currentDate();
    QTime ctime = QTime::currentTime();

    QString dt = cdate.toString(Qt::LocaleDate) + " " + ctime.toString(Qt::LocaleDate);

    QString text = fieldMsg->toPlainText(); //Текст из поля

    if (text != "" && loginDst != "")
    {
        //Записываем в таблицу
        query.exec("INSERT INTO " + tabName + " (dt, login, msg) VALUES ('" + dt + "', '"  + login + "', '" + text + "')");
        historyMsg->append(dt + " Вы: " + text); //Выводим в поле переписки

        QString msgServer = "MSG::" + login + "::" + loginDst + "::" + text; //Посторойка сообщения для сервера
        QByteArray temp_r = msgServer.toUtf8();
        const char* request = temp_r.data();

        int request_size = strlen(request)+1;

        //Отправляем сообщение на сервер
        send(connectedSocket, (char*)&request_size, sizeof(int), 0);
        send(connectedSocket, request, request_size, 0);
    }

    fieldMsg->clear(); //Очищаем текст
    fieldMsg->setFocus(); //Устанавливаем фокус обратно в поле ввода
}

void ChatBase::run()
{
    int msg_size;
    char* msg;
    QString command;
    QByteArray text;
    int i;

    forever
    {
        command = nullptr;
        text = nullptr;
        i = 0;
        //Получаем сообщение

        if (recv(connectedSocket, (char*)&msg_size, sizeof(int), 0) > 0) //Принимаем размер сообщения
        {
            msg = new char[msg_size + 1]; //Создаем чар* нужного размера
            msg[msg_size] = '\0';   //Конец строки

            if (recv(connectedSocket, msg, msg_size, 0) > 0) //Принимаем данные
            {
                while (msg[i] != '\0') //Идем до конца строки
                {
                    if (msg[i] == ':') //Дошли до разделителя
                    {
                        i = i+ 2; //Переходим за разделитель
                        break; //Выходим
                    }

                    command += msg[i]; //Записываем символ
                    i++;
                }

                while (msg[i] != '\0') //Идем до конца строки
                {
                    text += msg[i]; //Записываем символ
                    i++;
                }

                if (command == "MSG") //Если получено сообщение
                {
                    QMetaObject::invokeMethod(this, "PrintReceivedMessage", Qt::QueuedConnection, Q_ARG(QByteArray, text));
                }

                if (command == "USERS_ONLINE") //Данные о пользователях онлайн
                {
                    QMetaObject::invokeMethod(this, "UpdateButton", Qt::QueuedConnection, Q_ARG(QString, text));

                }

                if (command == "SAFETY")
                {
                    emit this->sendMsg(text);
                }
            }
        }
    }
}
